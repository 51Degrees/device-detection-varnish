/* *********************************************************************
 * This Original Work is copyright of 51 Degrees Mobile Experts Limited.
 * Copyright 2020 51 Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY.
 *
 * This Original Work is the subject of the following patents and patent
 * applications, owned by 51 Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816; and 
 * United States Patent Nos. 9,332,086 and 9,350,823.
 *
 * This Original Work is licensed under the European Union Public Licence (EUPL) 
 * v.1.2 and is subject to its terms as set out below.
 *
 * If a copy of the EUPL was not distributed with this file, You can obtain
 * one at https://opensource.org/licenses/EUPL-1.2.
 *
 * The 'Compatible Licences' set out in the Appendix to the EUPL (as may be
 * amended by the European Commission) shall be deemed incompatible for
 * the purposes of the Work and the provisions of the compatibility
 * clause in Article 5 of the EUPL shall not apply.
 * 
 * If using the Work as, or as part of, a network application, by 
 * including the attribution notice(s) required under Article 5 of the EUPL
 * in the end user terms of the application under an appropriate heading, 
 * such notice(s) shall fulfill the requirements of that article.
 * ********************************************************************* */

#ifndef VMOD_FIFTYONEDEGREES_INTERFACE_H
#define VMOD_FIFTYONEDEGREES_INTERFACE_H

#include <stdio.h>
#include <string.h>

#ifndef FIFTYONEDEGREES_PROPERTY_NOT_FOUND
#define FIFTYONEDEGREES_PROPERTY_NOT_FOUND "N/A"
#endif

#include "device-detection-cxx/src/hash/fiftyone.h"
#include "device-detection-cxx/src/hash/hash.h"
#include "vmod_fiftyonedegrees_extra.h"

#define CONFIG_OPTION(t) \
typedef struct vmodfod_config_##t##_t{ \
	bool set; \
	t value; \
} vmodfod_config_##t;

CONFIG_OPTION(int)
CONFIG_OPTION(bool)

/**
 * Global structure used by the module. This contains elements
 * which are required in multiple parts of the module.
 */
typedef struct vmodfod_global_t {
	ResourceManager *manager; /* Single manager instance constructed at startup. */
	ConfigHash *config;
	vmodfod_config_int drift; /* Drift value that allows substrings to be matched in a 
								 wider range of character positions. */
	vmodfod_config_int difference; /* Difference value to allow User-Agents where the character
									  is slightly different from what is expected. */
	vmodfod_config_int maxConcurrency; /* Max number of concurrent requests to be expected. */
	vmodfod_config_bool allowUnmatched; /* Allow unmatched results. */
	vmodfod_config_bool usePerformanceGraph; /* Use performance optimized graph. */
	vmodfod_config_bool usePredictiveGraph; /* Use predictive optimized graph. */
	const char *performanceProfile; /* The performance profile name. */
	const char *requiredProperties; /*  The required properties. */
	const char *propertyDelimiter; /* The delimiter to use when returning value strings. */
	StatusCode status; /* Status returned when the provider is initialised. */
} vmodfod_global;

/* Default global, set before being updated by the VCL. */
#define VMODFOD_DEFAULTGLOBAL \
{ \
	NULL, \
	NULL, \
	{false, 0}, \
	{false, 0}, \
	{false, 0}, \
	{false, false}, \
	{false, true}, \
	{false, true}, \
	"DEFAULT", \
	NULL, \
	",", \
	NOT_SET \
}

/* Convert string 'YES'|'NO' to corresponding boolean value */
#define VMODFOD_GET_BOOL(s) strcmp(s, "YES") == 0 ? true : false

/* Forward declarations */
bool RESULTS_HASH_GET_HAS_VALUES(
	ResultsHash *results,
	int requiredPropertyIndex,
	Exception *exception);

ResultsHash *OBTAIN_RESULTS_FROM_AVAILABLE_HEADERS(
	const struct vrt_ctx *ctx,
	Exception *exception);

#endif
