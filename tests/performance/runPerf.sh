#!/bin/bash
# Times requests through Varnish, each making one detection, and writes the
# results to results.json in the current folder. Run it from the folder CMake
# built in.
#
# The figure is the clock time for every request, the same measure as the
# console performance examples. A run that failed says why and writes no
# results.
#
# ENGINE is hash for device detection, the default, or ipi for IP Intelligence.
# DATA_FILE is the data file to use. BUILD_PATH is the folder holding the built
# modules. EVIDENCE is the evidence records file for device detection.
set -u

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
ENGINE=${ENGINE:-hash}
BUILD_PATH=${BUILD_PATH:-$HERE/../../..}
AB=${AB:-./ApacheBench-prefix/src/ApacheBench-build/bin/ab}
HOST=127.0.0.1:3000
CONCURRENCY=${CONCURRENCY:-$(nproc)}
PASSES=$((${PASSES_PER_WORKER:-20000} * CONCURRENCY))
VCL="$PWD/perf.vcl"
WORK="$PWD/varnish-work"

rm -f results.json

if [ "$ENGINE" == "ipi" ]; then
	DATA_FILE=${DATA_FILE:-$BUILD_PATH/cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi}
	# The tool can only vary the User-Agent, so the addresses are sent in it.
	SOURCE=(-U "${EVIDENCE:-$BUILD_PATH/cxx/ip-intelligence-data/evidence.csv}")
	IMPORT="import fiftyonedegrees_ipi;"
	INIT='fiftyonedegrees_ipi.set_performance_profile("IN_MEMORY");
	fiftyonedegrees_ipi.start("'"$DATA_FILE"'");'
	LOOKUP='set req.http.x-result = fiftyonedegrees_ipi.match("AsnName", req.http.user-agent);'
else
	DATA_FILE=${DATA_FILE:-$BUILD_PATH/cxx/device-detection-data/51Degrees-LiteV4.1.hash}
	SOURCE=(-E "${EVIDENCE:?EVIDENCE must be the evidence records file}")
	IMPORT="import fiftyonedegrees;"
	INIT='fiftyonedegrees.set_performance_profile("IN_MEMORY");
	fiftyonedegrees.set_properties("IsMobile");
	fiftyonedegrees.start("'"$DATA_FILE"'");'
	LOOKUP='set req.http.x-result = fiftyonedegrees.match_all("IsMobile");'
fi

cat > "$VCL" <<EOF
vcl 4.0;
$IMPORT
backend default { .host = "127.0.0.1"; .port = "9"; }
sub vcl_init {
	$INIT
}
sub vcl_recv {
	$LOOKUP
	return (synth(200));
}
EOF

rm -rf "$WORK"
varnishd -F -n "$WORK" -a "$HOST" -f "$VCL" \
	-p "vmod_path=$BUILD_PATH/src/dd/.libs:$BUILD_PATH/src/ipi/.libs" \
	-s malloc,64m > varnishd.out 2>&1 &
VARNISH_PID=$!

tries=0
until [ "$(curl -o /dev/null -s -w '%{http_code}' "http://$HOST/")" == "200" ]; do
	tries=$((tries + 1))
	if [ "$tries" -gt 120 ] || ! kill -0 "$VARNISH_PID" 2>/dev/null; then
		echo "Varnish did not start"
		cat varnishd.out
		kill "$VARNISH_PID" 2>/dev/null
		exit 1
	fi
	sleep 1
done

"$AB" "${SOURCE[@]}" -q -c "$CONCURRENCY" -n "$PASSES" "http://$HOST/" > process.out

kill "$VARNISH_PID"
wait "$VARNISH_PID" 2>/dev/null

FAILED=$(sed -En 's/Failed requests: *([0-9]+)/\1/p' process.out)
NON200=$(sed -En 's/Non-2xx responses: *([0-9]+)/\1/p' process.out)
TAKEN=$(sed -En 's/Time taken for tests: *([0-9.]+) seconds/\1/p' process.out)
if [ "${FAILED:-1}" != "0" ] || [ -n "$NON200" ] ||
	! awk -v taken="${TAKEN:-0}" 'BEGIN { exit !(taken > 0) }'; then
	echo "No results: ${TAKEN:-unknown} seconds," \
		"${FAILED:-unknown} failed requests, ${NON200:-0} non-2xx responses"
	exit 1
fi

awk -v taken="$TAKEN" -v requests="$PASSES" -v concurrency="$CONCURRENCY" 'BEGIN {
	seconds = taken / requests
	printf "{\n"
	printf "  \"HigherIsBetter\": { \"DetectionsPerSecond\": %.2f },\n", 1 / seconds
	printf "  \"LowerIsBetter\": { \"AvgMillisecsPerDetection\": %.6f },\n", seconds * 1000
	printf "  \"Seconds\": %s,\n", taken
	printf "  \"Requests\": %d,\n", requests
	printf "  \"Concurrency\": %d\n", concurrency
	printf "}\n"
}' > results.json
cat results.json
