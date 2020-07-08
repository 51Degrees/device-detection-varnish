/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2020 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "cache/cache.h"
#include "string.h"
#include "vcc_if.h"
#include "vmod_fiftyonedegrees_interface.h"

/*
 * Global test object structure. This is used to hold expected test results
 */
typedef struct vmodfod_global_test_t {
	vmodfod_config_int expectedDrift;
	vmodfod_config_int expectedDifference;
	vmodfod_config_bool expectedHasValues;
} vmodfod_global_test;

/* Set default test expected value */
vmodfod_global_test globalTest = {{false, 0}, {false, 0}};

/* Global structure available to the module. */
extern vmodfod_global global;

/* Forward declaration of the main function*/
ResultsHash *obtainResultsFromAvailableHeaders(
	const struct vrt_ctx *ctx,
	Exception *exception);

bool ResultsHashGetHasValuesDecor(
	ResultsHash *results,
	int requiredPropertyIndex,
	Exception *exception)
{
	bool hasValues = ResultsHashGetHasValues(results, requiredPropertyIndex, exception);
	if (globalTest.expectedHasValues.set)
		hasValues = globalTest.expectedHasValues.value;
	return hasValues;
}

ResultsHash *obtainResultsFromAvailableHeadersDecor(
	const struct vrt_ctx *ctx,
	Exception *exception)
{
	// Obtain results as does in release version
	ResultsHash *results = obtainResultsFromAvailableHeaders(ctx, exception);
	// Modify drift and difference
	if (globalTest.expectedDrift.set)
		results->items->drift = globalTest.expectedDrift.value;
	if (globalTest.expectedDifference.set)
		results->items->difference = globalTest.expectedDifference.value;
	return results;
}

void vmod_set_expected_drift(const struct vrt_ctx *ctx, VCL_INT drift)
{
	globalTest.expectedDrift.value = drift;
	globalTest.expectedDrift.set = true;
}

void vmod_set_expected_difference(const struct vrt_ctx *ctx, VCL_INT difference)
{
	globalTest.expectedDifference.value = difference;
	globalTest.expectedDifference.set = true;
}

void vmod_set_expected_has_values(const struct vrt_ctx *ctx, VCL_STRING hasValues)
{
	globalTest.expectedHasValues.value = VMODFOD_GET_BOOL(hasValues);
	globalTest.expectedHasValues.set = true;
}

VCL_STRING vmod_get_performance_profile(const struct vrt_ctx *ctx)
{
	// The pointer in workspace memory to print to
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively
	unsigned u, v;

	// Reserved some workspace
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the work space
	p = ctx->ws->f;

	if (global.config->strings.loaded == HashInMemoryConfig.strings.loaded
		&& global.config->strings.capacity == HashInMemoryConfig.strings.capacity
		&& global.config->strings.concurrency == HashInMemoryConfig.strings.concurrency)
	{
		v = snprintf(p, u, "IN_MEMORY");
	}
	else if (global.config->strings.loaded == HashHighPerformanceConfig.strings.loaded
		&& global.config->strings.capacity == HashHighPerformanceConfig.strings.capacity
		&& global.config->strings.concurrency == HashHighPerformanceConfig.strings.concurrency)
	{
		v = snprintf(p, u, "HIGH_PERFORMANCE");
	}
	else if (global.config->strings.loaded == HashLowMemoryConfig.strings.loaded
		&& global.config->strings.capacity == HashLowMemoryConfig.strings.capacity
		&& global.config->strings.concurrency == HashLowMemoryConfig.strings.concurrency)
	{
		v = snprintf(p, u, "LOW_MEMORY");
	}
	else
	{
		v = snprintf(p, u, "DEFAULT");
	}
	// Skip over the null terminator
	v++;
	if (v == 0 || v > u) {
		// No space, reset and leave
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (p);
}

VCL_INT vmod_get_drift(const struct vrt_ctx *ctx)
{
	return global.config->drift;
}

VCL_INT vmod_get_difference(const struct vrt_ctx *ctx)
{
	return global.config->difference;
}

VCL_INT vmod_get_max_concurrency(const struct vrt_ctx *ctx)
{
	return global.config->strings.concurrency;
}

VCL_BOOL vmod_get_allow_unmatched(const struct vrt_ctx *ctx)
{
	return global.config->b.allowUnmatched;
}

VCL_BOOL vmod_get_use_performance_graph(const struct vrt_ctx *ctx)
{
	return global.config->usePerformanceGraph;
}

VCL_BOOL vmod_get_use_predictive_graph(const struct vrt_ctx *ctx)
{
	return global.config->usePredictiveGraph;
}
