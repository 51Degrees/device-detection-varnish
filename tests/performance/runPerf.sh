#!/bin/bash
# Measures how long the modules add to a request, by timing the same number of
# requests with no lookups and then with lookups, and writes the results to
# results.json in the current folder. Run it from the folder CMake built in.
#
# A lookup is quick next to a whole request, so each request makes several of
# them, which keeps the difference between the two times well above the
# difference between two runs of the same thing. A difference which is still not
# above zero measures nothing, so the script fails rather than write a figure.
#
# ENGINE is hash for device detection, the default, or ipi for IP Intelligence.
# DATA_FILE is the data file to use. BUILD_PATH is the folder holding the built
# modules. EVIDENCE is the evidence records file for device detection.
# LOOKUPS is the number of lookups in each request.
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

if [ "$ENGINE" == "ipi" ]; then
	LOOKUPS=${LOOKUPS:-20}
	DATA_FILE=${DATA_FILE:-$BUILD_PATH/cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi}
	# The tool can only vary the User-Agent, so the addresses are sent in it.
	SOURCE=(-U "${EVIDENCE:-$BUILD_PATH/cxx/ip-intelligence-data/evidence.csv}")
	IMPORT="import fiftyonedegrees_ipi;"
	INIT='fiftyonedegrees_ipi.set_performance_profile("IN_MEMORY");
	fiftyonedegrees_ipi.start("'"$DATA_FILE"'");'
	LOOKUP='set req.http.x-result = fiftyonedegrees_ipi.match("AsnName", req.http.user-agent);'
else
	LOOKUPS=${LOOKUPS:-5}
	DATA_FILE=${DATA_FILE:-$BUILD_PATH/cxx/device-detection-data/51Degrees-LiteV4.1.hash}
	SOURCE=(-E "${EVIDENCE:?EVIDENCE must be the evidence records file}")
	IMPORT="import fiftyonedegrees;"
	INIT='fiftyonedegrees.set_performance_profile("IN_MEMORY");
	fiftyonedegrees.set_properties("IsMobile,BrowserName,PlatformName");
	fiftyonedegrees.start("'"$DATA_FILE"'");'
	LOOKUP='set req.http.x-result = fiftyonedegrees.match_all("IsMobile,BrowserName,PlatformName");'
fi

{
	echo "vcl 4.0;"
	echo "$IMPORT"
	echo 'backend default { .host = "127.0.0.1"; .port = "9"; }'
	echo "sub vcl_init {"
	echo "	$INIT"
	echo "}"
	echo "sub vcl_recv {"
	echo '	if (req.url == "/process") {'
	for ((i = 0; i < LOOKUPS; i++)); do
		echo "		$LOOKUP"
	done
	echo "	}"
	echo "	return (synth(200));"
	echo "}"
} > "$VCL"

rm -rf "$WORK"
varnishd -F -n "$WORK" -a "$HOST" -f "$VCL" \
	-p "vmod_path=$BUILD_PATH/src/dd/.libs:$BUILD_PATH/src/ipi/.libs" \
	-s malloc,64m > varnishd.out 2>&1 &
VARNISH_PID=$!

tries=0
until [ "$(curl -o /dev/null -s -w '%{http_code}' "http://$HOST/calibrate")" == "200" ]; do
	tries=$((tries + 1))
	if [ "$tries" -gt 120 ] || ! kill -0 "$VARNISH_PID" 2>/dev/null; then
		echo "Varnish did not start"
		cat varnishd.out
		kill "$VARNISH_PID" 2>/dev/null
		exit 1
	fi
	sleep 1
done

"$AB" "${SOURCE[@]}" -q -c "$CONCURRENCY" -n "$PASSES" "http://$HOST/calibrate" > calibrate.out
"$AB" "${SOURCE[@]}" -q -c "$CONCURRENCY" -n "$PASSES" "http://$HOST/process" > process.out

kill "$VARNISH_PID"
wait "$VARNISH_PID" 2>/dev/null

for f in calibrate.out process.out; do
	failed=$(sed -En 's/Failed requests: *([0-9]+)/\1/p' "$f")
	non200=$(sed -En 's/Non-2xx responses: *([0-9]+)/\1/p' "$f")
	if [ "${failed:-1}" != "0" ] || [ -n "$non200" ]; then
		echo "$f has failed requests: ${failed:-unknown}, non-2xx responses: ${non200:-0}"
		exit 1
	fi
done

CAL=$(sed -En 's/Time taken for tests: *([0-9.]+) seconds/\1/p' calibrate.out)
PRO=$(sed -En 's/Time taken for tests: *([0-9.]+) seconds/\1/p' process.out)

awk -v cal="$CAL" -v pro="$PRO" -v passes="$PASSES" -v lookups="$LOOKUPS" 'BEGIN {
	ms = (pro - cal) * 1000 / passes / lookups
	if (ms <= 0) {
		printf "Lookups added nothing measurable. Calibration took %s seconds and processing took %s seconds.\n", cal, pro
		exit 1
	}
	printf "{\n"
	printf "  \"HigherIsBetter\": { \"DetectionsPerSecond\": %.2f },\n", 1000 / ms
	printf "  \"LowerIsBetter\": { \"AvgMillisecsPerDetection\": %.6f },\n", ms
	printf "  \"CalibrationSeconds\": %s,\n", cal
	printf "  \"ProcessingSeconds\": %s,\n", pro
	printf "  \"Requests\": %d,\n", passes
	printf "  \"LookupsPerRequest\": %d\n", lookups
	printf "}\n"
}' > results.json
status=$?
cat results.json
exit $status
