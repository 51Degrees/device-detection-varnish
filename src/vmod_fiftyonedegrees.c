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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "cache/cache.h"
#include "string.h"
#include "vcc_if.h"
#include "vmod_fiftyonedegrees_interface.h"


/**
 * @defgroup vmod_fiftyonedegrees 51Degrees VMOD Internals
 *
 * @{
 */

/**
 * Global structure available to the module.
 */
vmodfod_global global = VMODFOD_DEFAULTGLOBAL;

/**
 * Method used on shutdown to free the 51Degrees ResourceManager
 * and ConfigHash.
 * @param ptr pointer to the allocated memory to free.
 */
void privResourceFree(void *ptr)
{
	if (global.status == SUCCESS)
		ResourceManagerFree(global.manager);
	free(ptr);
}

/**
 * VMOD init
 * Initialise the 51Degrees ResourceManager and ConfigHash by allocating
 * memory ready to be allocated.
 * @param ctx the context
 * @param priv the private memory pointer for the module
 * @param e the event identifier
 * @return return code
 */
int init_function(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	enum vcl_event_e e)
{
	switch (e) {
		case VCL_EVENT_LOAD:
			// Allocate memory for both ResourceManager and ConfigHash objects
			priv->priv = malloc(sizeof(ResourceManager) + sizeof(ConfigHash));
			if (priv->priv == NULL)
			{
				fprintf(stderr,
					"vmod_fiftyonedegrees: Insufficient memory allocated.\n");
				return (-1);
			}
			else
			{
				// Point manager object to its allocate memory
				global.manager = (ResourceManager *)priv->priv;
				// Point config object to its allocate memory
				global.config = (ConfigHash *)(global.manager + 1);
				// Set the callback function which will be called when the
				// module is discarded
				priv->free = privResourceFree;
				return (0);
			}
		default:
			return (0);
	}
	return (0);
}

/**
 * Method used to get string from the strings collection of the 51Degrees dataset.
 * @param offset the offset of the string in the strings collection
 * @param buffer the buffer to store the string
 * @param bufferSize the size of the buffer
 * @return the number of written characters or 0 if no characters
 * has been written.
 */
static unsigned getString(const int32_t offset, char *buffer, unsigned bufferSize)
{
	// Number of written characters
	unsigned n;

	DataSetHash *dataset;
	Item item;
	String *itemData;

	// Reset the data structure in the item
	DataReset(&item.data);
	// Fetch a safe reference to the dataset
	dataset = DataSetHashGet(global.manager);
	
	// Obtain the string item
	EXCEPTION_CREATE;
	itemData = StringGet(
		dataset->strings,
		offset,
		&item,
		exception);
	if (EXCEPTION_FAILED) {
		// Release the safe reference
		DataSetHashRelease(dataset);
		return 0;
	}

	// Copy the obtained string to the work space memory
	n = snprintf(buffer, bufferSize, "%s", STRING(itemData));
	// Release the string item
	COLLECTION_RELEASE(dataset->strings, &item);
	// Release the safe reference to the dataset
	DataSetHashRelease(dataset);

	// Return the number of written characters including the null terminator
	return (++n);
}

/**
 * VMOD function
 * Get the name of the dataset
 * @param ctx the context
 * @return dataset name
 */
VCL_STRING vmod_get_dataset_name(const struct vrt_ctx *ctx)
{
	// The pointer in workspace memory to print to
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively
	unsigned u, v;

	DataSetHash *dataset;

	// Reserved some workspace
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the work space
	p = ctx->ws->f;

	// Get a safe reference to the dataset
	dataset = DataSetHashGet(global.manager);

	v = getString(dataset->header.nameOffset, p, u);
	if (v == 0 || v > u) {
		// Release the safe reference
		DataSetHashRelease(dataset);
		// No space, reset and leave
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	// Release the safe reference
	DataSetHashRelease(dataset);
	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD function
 * Get the format of the dataset
 * @param ctx the context
 * @return format
 */
VCL_STRING vmod_get_dataset_format(const struct vrt_ctx *ctx)
{
	// The pointer in workspace memory to print to
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively
	unsigned u, v;

	DataSetHash *dataset;

	// Reserved some workspace
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the work space
	p = ctx->ws->f;

	// Get a safe reference to the dataset
	dataset = DataSetHashGet(global.manager);

	v = getString(dataset->header.formatOffset, p, u);
	if (v == 0 || v > u) {
		// Release the safe reference
		DataSetHashRelease(dataset);
		// No space, reset and leave
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	// Release the safe reference
	DataSetHashRelease(dataset);
	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD function
 * Get the published date of the dataset
 * @param ctx the context
 * @return published date
 */
VCL_STRING vmod_get_dataset_published_date(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	DataSetHash *dataset;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	dataset = DataSetHashGet(global.manager);
	v = snprintf(p, u, "%d-%d-%d",
		dataset->header.published.year,
		(int)dataset->header.published.month,
		(int)dataset->header.published.day);
	// Skip over the null terminator
	v++;
	if (v > u) {
		DataSetHashRelease(dataset);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	DataSetHashRelease(dataset);
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Set the properties which should be initialised in the dataset.
 * @param ctx the context
 * @param properties the list of required properties
 */
void vmod_set_properties(const struct vrt_ctx *ctx, VCL_STRING properties)
{
	global.requiredProperties = properties;
}

/**
 * VMOD Function
 * Set the delimiter to use when returning values for multiple properties.
 * @param ctx the context
 * @param delimiter the delimiter string to use
 */
void vmod_set_delimiter(const struct vrt_ctx *ctx, VCL_STRING delimiter)
{
	global.propertyDelimiter = delimiter;
}

/**
 * VMOD Function
 * Set the performance profile which dictates how Device Detection 
 * allocates memory and manage data.
 * @param ctx the context
 * @param performanceProfile the performance profile string
 */
void vmod_set_performance_profile(const struct vrt_ctx *ctx, VCL_STRING performanceProfile)
{
	global.performanceProfile = performanceProfile;
}

/**
 * VMOD Function
 * Set the drift value to allow substrings to be matched at a wider range.
 * @param ctx the context
 * @param drift the drift value
 */
void vmod_set_drift(const struct vrt_ctx *ctx, VCL_INT drift)
{
	global.drift.value = drift;
	global.drift.set = true;
}

/**
 * VMOD Function
 * Set the difference value to allow User-Agents where characters are slightly
 * different from what is expected.
 * @param ctx the context
 * @param difference the difference value
 */
void vmod_set_difference(const struct vrt_ctx *ctx, VCL_INT difference)
{
	global.difference.value = difference;
	global.difference.set = true;
}

/**
 * VMOD Function
 * Set whether unmatched result should be allowed.
 * @param ctx the context
 * @param allowUnmatched the indication string (YES/NO)
 */ 
void vmod_set_allow_unmatched(const struct vrt_ctx *ctx, VCL_STRING allowUnmatched)
{
	global.allowUnmatched.value = VMODFOD_GET_BOOL(allowUnmatched);
	global.allowUnmatched.set = true;
}

/**
 * VMOD Function
 * Set whether performance optimized graph should be used.
 * @param ctx the context
 * @param usePerformanceGraph the indication string (YES/NO)
 */
void vmod_set_use_performance_graph(const struct vrt_ctx *ctx, VCL_STRING usePerformanceGraph)
{
	global.usePerformanceGraph.value = VMODFOD_GET_BOOL(usePerformanceGraph);
	global.usePerformanceGraph.set = true;
}

/**
 * VMOD Function
 * Set whether predictive optimized graph should be used.
 * @param ctx the context
 * @param usePredictiveGraph the indication string (YES/NO)
 */
void vmod_set_use_predictive_graph(const struct vrt_ctx *ctx, VCL_STRING usePredictiveGraph)
{
	global.usePredictiveGraph.value = VMODFOD_GET_BOOL(usePredictiveGraph);
	global.usePredictiveGraph.set = true;
}

/**
 * VMOD Function
 * Set the maximum number of expected concurrent requests.
 * @param ctx the context
 * @param concurrency the maximum number of expected concurrent requests
 */
void vmod_set_max_concurrency(const struct vrt_ctx *ctx, VCL_INT concurrency)
{
	global.maxConcurrency.value = concurrency;
	global.maxConcurrency.set = true;
}

/**
 * Method used to log the exception message to the standard error
 * @param exception the exception
 */
static void logExceptionMessage(Exception *exception)
{
	const char *message = ExceptionGetMessage(exception);
	if (message != NULL) {
		fputs(message, stderr);
		fiftyoneDegreesFree((void*)message);
	}
}

/**
 * Error message format
 */
#define VMODFOD_MSG_FORMAT "vmod_fiftyonedegrees: %s\n"
/**
 * Property has no values message format
 */
#define VMODFOD_PROP_NO_VALUES_MSG_FORMAT \
"vmod_fiftyonedegrees: Property %s has no values (Reason: %s)\n"
/**
 * Performance profile is not recognized
 */
#define VMODFOD_UNRECOGNIZED_PERF_PROFILE_MSG_FORMAT \
"vmod_fiftyonedegrees: Cannot recognize the performance profile %s. Revert to DEFAULT\n"

/**
 * Method to print appropriate error message
 * @param filePath path to the data file
 */
static void loadFileError(const char *filePath)
{
	// Get message corresponding to the global status
	fprintf(stderr,
		VMODFOD_MSG_FORMAT,
		StatusGetMessage(global.status, filePath));
}

/**
 * Method to print appropriate error message and then terminate
 * @param filePath path to the data file
 */
static void loadFileErrorAbort(const char *filePath)
{
	loadFileError(filePath);
	abort();
}

/**
 * Method used to init a ConfigHash object
 */
static void initConfig()
{
	// Init the base config using the performance profile
	if (strcmp(global.performanceProfile, "IN_MEMORY") == 0)
	{
		*global.config = HashInMemoryConfig;
	}
	else if (strcmp(global.performanceProfile, "HIGH_PERFORMANCE") == 0)
	{
		*global.config = HashHighPerformanceConfig;
	}
	else if (strcmp(global.performanceProfile, "LOW_MEMORY") == 0)
	{
		*global.config = HashLowMemoryConfig;
	}
	else if (strcmp(global.performanceProfile, "BALANCED") == 0)
	{
		*global.config = HashBalancedConfig;
	}
	else
	{
		if (strcmp(global.performanceProfile, "DEFAULT") != 0)
			fprintf(stderr,
				VMODFOD_UNRECOGNIZED_PERF_PROFILE_MSG_FORMAT,
				global.performanceProfile);
		*global.config = HashDefaultConfig;
	}

	// Update the config options with explicitly set values
	if (global.drift.set) {
		global.config->drift = global.drift.value;
	}
	if (global.difference.set) {
		global.config->difference = global.difference.value;
	}
	if (global.usePerformanceGraph.set) {
		global.config->usePerformanceGraph = global.usePerformanceGraph.value;
	}
	if (global.usePredictiveGraph.set) {
		global.config->usePredictiveGraph = global.usePredictiveGraph.value;
	}
	if (global.allowUnmatched.set) {
		global.config->b.allowUnmatched = global.allowUnmatched.value;
	}
	if (global.maxConcurrency.set) {
		global.config->strings.concurrency = global.maxConcurrency.value;
		global.config->components.concurrency = global.maxConcurrency.value;
		global.config->maps.concurrency = global.maxConcurrency.value;
		global.config->properties.concurrency = global.maxConcurrency.value;
		global.config->values.concurrency = global.maxConcurrency.value;
		global.config->profiles.concurrency = global.maxConcurrency.value;
		global.config->nodes.concurrency = global.maxConcurrency.value;
		global.config->profileOffsets.concurrency = global.maxConcurrency.value;
	}
}

/**
 * VMOD Function
 * Start the 51Degrees module using the data file provided.
 * @param ctx the context
 * @param filePath the path to the data file to be used
 */
void vmod_start(const struct vrt_ctx *ctx, VCL_STRING filePath)
{
	EXCEPTION_CREATE;
	PropertiesRequired properties = PropertiesDefault;
	properties.string = global.requiredProperties;

	// Initialise the config object
	initConfig();

	// Initialise the resource manager
	global.status = HashInitManagerFromFile(
		global.manager,
		global.config,
		&properties,
		filePath,
		exception);
	EXCEPTION_THROW;
	
	if (global.status != SUCCESS) {
		// The initialisation was unsuccessful,
		// throw the correct error message and abort
		loadFileErrorAbort(filePath);
	}
}

/**
 * VMOD Function
 * Reload resource manager using the input file path. If no file path is given,
 * the original data file will be used.
 * @param ctx the context
 * @param a the vmod argument which contains the file path. 
 */
void vmod_reload(const struct vrt_ctx *ctx, struct vmod_reload_arg *a) {
	const char *filePath;

	EXCEPTION_CREATE
	// Check if a file path is given
	if (a->valid_filePath) {
		filePath = a->filePath;
		// Reload the manager from the given file
		global.status = HashReloadManagerFromFile(
			global.manager,
			filePath,
			exception);
	}
	else
	{
		// Get a safe reference to dataset
		DataSetBase *dataset = DataSetGet(global.manager);
		filePath = dataset->masterFileName;
		// Reload the manager from the original file
		global.status = HashReloadManagerFromOriginalFile(
			global.manager,
			exception);
		// Release the safe reference
		DataSetRelease(dataset);
	}
	if (EXCEPTION_FAILED) {
		// Log an exception message
		logExceptionMessage(exception);
	}

	if (global.status != SUCCESS) {
		// Print out an error.
		loadFileError(filePath);
		// Reload failed but the old one will still be left in place
		// Thus, reset the status to success so the manager can be
		// freed when the module is discarded.
		global.status = SUCCESS;
	}
}

/**
 * Get the number of separators contained in a string. Separators include
 * ',', '|', ' ', and '\t'.
 * @param input to search for separators
 * @return the number of separators found
 */
static int32_t getSeparatorCount(const char* input) {
	int32_t index = 0, count = 0;
	if (input != NULL && *input != 0) {
		while (*(input + index) != 0) {
			if (*(input + index) == ',' ||
				*(input + index) == '|' ||
				*(input + index) == ' ' ||
				*(input + index) == '\t')
				count++;
			index++;
		}
		return count + 1;
	}
	return 0;
}

/**
 * Populate an array of strings from a comma separated string. The comma
 * characters are replaced with null terminators to allow the array elements
 * to point to the strings directly.
 * @param propertiesString the string containing property names
 * @param propertiesArray memory allocated for the array
 * @param propertiesCount the number of properties contained in the string
 * @return a newly allocated array pointing to the property names
 */
static void stringToArray(
	char *propertiesString,
	char **propertiesArray,
	int propertiesCount)
{
	int charPos, index;
	int propertiesStringLength = strlen(propertiesString);

	index = 0;
	propertiesArray[index++] = propertiesString;

	for (charPos = 0; charPos < propertiesStringLength; charPos++)
	{
		if (propertiesString[charPos] == ',') {
			propertiesString[charPos] = '\0';
			propertiesArray[index++] = propertiesString + charPos + 1;
		}
	}
}

/**
 * Method used to get the number of required properties
 * @param results
 * @return number of required properties
 */
static int getRequiredPropertyCount(
	ResultsHash *results) {
	return ((DataSetHash *)results->b.b.dataSet)->b.b.available->count;
}

/**
 * Method used to get the name of the required property at the given index
 * @param results the match results
 * @param index index of the target property
 * @return the required property name
 */
static const char *getRequiredPropertyName(
	ResultsHash *results,
	int index) {
	return STRING(((DataSetHash *)results->b.b.dataSet)->b.b.available->items[index].name.data.ptr);
}

/**
 * Method used to obtain the required property value by using its' index
 * @param results the match results
 * @param requiredPropertyIndex the index of the required property
 * @param p buffer which will hold the value
 * @param u the size of the buffer
 * @return the number of characters added, excluding the null terminator
 */
static unsigned writeValue(
	ResultsHash *results,
	int requiredPropertyIndex,
	char *p,
	unsigned u) {
	size_t charactersAdded = 0;
	bool hasValues = false;

	EXCEPTION_CREATE;
	// Get property hasValues status
	hasValues = RESULTS_HASH_GET_HAS_VALUES(results, requiredPropertyIndex, exception);
	if (EXCEPTION_OKAY) {
		// Check if property has values
		if (hasValues) {
			// Print value to buffer
			charactersAdded = ResultsHashGetValuesStringByRequiredPropertyIndex(
				results,
				requiredPropertyIndex,
				p,
				u,
				"|",
				exception);
			if (EXCEPTION_FAILED) {
				logExceptionMessage(exception);
				charactersAdded = 0;
			}
		}
		else
		{
			// Get the reason message for property having no values
			const char *reasonMessage = fiftyoneDegreesResultsHashGetNoValueReasonMessage(
					fiftyoneDegreesResultsHashGetNoValueReason(results, 
						requiredPropertyIndex,
						exception));
			// Check exception and set message appropriately.
			if (EXCEPTION_FAILED)
				reasonMessage = "Failed to obtain the reason";

			// Print a message to indicate that property has no values.
			fprintf(stderr,
				VMODFOD_PROP_NO_VALUES_MSG_FORMAT,
				getRequiredPropertyName(results, requiredPropertyIndex),
				reasonMessage);
			
			// Print a null terminator to indicate that the value is empty
			snprintf(p, u, "%c", '\0');
		}
	}
	else
	{
		logExceptionMessage(exception);
	}

	return charactersAdded;
}

/**
 * Write the values for the required property to the workspace memory.
 * @param results the results to get the values from
 * @param requiredPropertyName name of the required property
 * @param p pointer to the point in workspace memory to print the values
 * @param u the length of the available memory to write to
 * @return the number of bytes written
 */
unsigned getValue(
	ResultsHash *results,
	char *requiredPropertyName,
	char *p,
	unsigned u)
{
	int i;
	unsigned v;
	char *currentPropertyName;
	bool found = false;

	if (StringCompare(requiredPropertyName, "Iterations") == 0)
	{
		v = snprintf(p, u, "%d", results->items->iterations);
	}
	else if (StringCompare(requiredPropertyName, "Drift") == 0)
	{
		v = snprintf(p, u, "%d", results->items->drift);
	}
	else if (StringCompare(requiredPropertyName, "Difference") == 0)
	{
		v = snprintf(p, u, "%d", results->items->difference);
	}
	else if (StringCompare(requiredPropertyName, "Method") == 0)
	{
		const char *method;
		switch (results->items->method) {
			case FIFTYONE_DEGREES_HASH_MATCH_METHOD_PERFORMANCE:
				method = "PERFORMANCE";
				break;
			case FIFTYONE_DEGREES_HASH_MATCH_METHOD_COMBINED:
				method = "COMBINED";
				break;
			case FIFTYONE_DEGREES_HASH_MATCH_METHOD_PREDICTIVE:
				method = "PREDICTIVE";
				break;
			case FIFTYONE_DEGREES_HASH_MATCH_METHOD_NONE:
			default:
				method = "NONE";
				break;
		}
		v = snprintf(p, u, "%s", method);	
	}
	else if (StringCompare(requiredPropertyName, "UserAgents") == 0)
	{
		v = snprintf(p, u, "%s", results->items->b.matchedUserAgent);
	}
	else if (StringCompare(requiredPropertyName, "MatchedNodes") == 0)
	{
		v = snprintf(p, u, "%d", results->items->matchedNodes);
	}
	else if (StringCompare(requiredPropertyName, "DeviceId") == 0)
	{
		EXCEPTION_CREATE;
		char deviceId[40];
		HashGetDeviceIdFromResults(
			results,
			deviceId,
			sizeof(deviceId),
			exception);
		if (EXCEPTION_FAILED) {
			logExceptionMessage(exception);
			// Print a null terminator to indicate that the value is empty
			snprintf(p, u, "%c", '\0');
			v = 0;
		}
		else
		{
			v = snprintf(p, u, "%s", deviceId);
		}	
	}
	else {
		v = 0;
		// Property is not a match metric, so search the required properties.
		for (i = 0; i < getRequiredPropertyCount(results); i++)
		{
			currentPropertyName =
				(char*)getRequiredPropertyName(results, i);
			if (StringCompare(currentPropertyName, requiredPropertyName) == 0)
			{
				// This if the property we want, so write the values to the
				// workspace
				v += writeValue(
					results,
					i,
				    p,
					u);
				// Found the property, so stop looking.
				found = true;
				break;
			}
		}
		if (!found)
			// Property was not found, so set value accordingly.
			v = snprintf(p, u, FIFTYONEDEGREES_PROPERTY_NOT_FOUND);
	}
	return v;
}

/**
 * Write the values for all the requested properties to the workspace memory.
 * @param p pointer to the point in workspace memory to print the values
 * @param u the length of the available memory to write to
 * @param results the matched results to get the values from
 * @param propertiesArray the array of property names to write the values of
 * @param propertiesCount the number of properties in the array
 * @return the number of bytes written
 */
static unsigned printValuesToWorkspace(
	char *p,
	unsigned u,
	ResultsHash *results,
	char **propertiesArray,
	int propertiesCount)
{
	int i;
	unsigned v = 0;
	for (i = 0; i < propertiesCount; i++)
	{
		if (i != 0) {
			v += snprintf(p + v, u, "%s", global.propertyDelimiter);
			if (v > u) {
				// Break now as we will only be printing to another workspace.
				return v;
			}
		}
		v += getValue(results, propertiesArray[i], p + v, u);
		if (v > u) {
			// Break now as we will only be printing to another workspace.
			return v;
		}
	}

	return v;
}

/**
 * VMOD Function
 * Match the User-Agent provided, and return the values for the properties
 * requested for the matched device
 * @param ctx the context (the User-Agent does not come from here in this
 * instance, it is instead provided as an argument)
 * @param userAgent the User-Agent string to match
 * @param propertyInputString the list of property names to return the values
 * @return the values of the requested properties for the device matched with
 * the supplied User-Agent
 */
VCL_STRING vmod_match_single(
	const struct vrt_ctx *ctx,
	VCL_STRING userAgent,
	VCL_STRING propertyInputString)
{
	// The pointer in workspace memory to print to.
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively.
	unsigned u, v = 0;

	int propertiesCount;
	char **propertiesArray;
	char *propertiesString, *returnString;

	// Reserve some workspace.
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the workspace.
	p = ctx->ws->f;

	// Copy the properties string to the workspace memorys.
	v += snprintf(p, u, "%s", propertyInputString);
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Skip over the null terminator.
	v++;

	// This string is modifiable unlike propertyInputString, so getProperties
	// can replace separators with null terminators.
	propertiesString = p;

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pionters to the correct point in the
	// properties string.
	propertiesArray = (char **)p + v;
	v += sizeof(char **) * propertiesCount;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	stringToArray(propertiesString, propertiesArray, propertiesCount);
	// Move over the array memory.
	returnString = p + v;

	// Get a results instance to work on.
	ResultsHash *results = ResultsHashCreate(global.manager, 1, 0);

	EXCEPTION_CREATE;
	// Carries out a match for the input User-Agent.
	ResultsHashFromUserAgent(
		results,
		userAgent,
		strlen(userAgent),
		exception);
	// Check exception and return
	if (EXCEPTION_FAILED) {
		logExceptionMessage(exception);
		ResultsHashFree(results);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	v += printValuesToWorkspace(
		p + v,
		u,
		results,
		propertiesArray,
		propertiesCount);
	// Skip over the null terminator
	v++;

	ResultsHashFree(results);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	WS_Release(ctx->ws, v);
	return (returnString);
}

/**
 * Method to obtain the number of unique headers
 * @return the number of unique headers
 */
static int getHeaderCount()
{
	DataSetBase *dataset = DataSetGet(global.manager);
	int count = dataset->uniqueHeaders->count;
	DataSetRelease(dataset);
	return count;
}

/**
 * Method to obtain header name at a given index
 * @param headerIndex the index of the header
 * @return header name at the given index
 */
static const char *getHeaderName(
	int headerIndex) {
	DataSetBase *dataset = DataSetGet(global.manager);
	const char *headerName = STRING(dataset->uniqueHeaders->items[headerIndex].name.data.ptr);
	DataSetRelease(dataset);
	return headerName;
}

/**
 * Get the value of the header for the request context.
 * @param ctx the context
 * @param headerName the name of the header to get the value for
 * @return the value of the header
 */
static char *searchHeaders(const struct vrt_ctx *ctx, const char *headerName)
{
	char *currentHeader;
	int i;
	for (i = 0; i < ctx->http_req->nhd; i++)
	{
		currentHeader = (char*)ctx->http_req->hd[i].b;
		if (currentHeader != NULL
			&& StringCompareLength(currentHeader, headerName, strlen(headerName)) == 0)
		{
			return currentHeader + strlen(headerName) + 2;
		}
	}
	return NULL;
}

/**
 * Method to fill evidence with all available HTTP request header
 * @param ctx the context
 * @param evidence the evidence object
 */
static void setImportantHeaders(
	const struct vrt_ctx *ctx,
	EvidenceKeyValuePairArray *evidence)
{
	int headerIndex;
	char *headerValue;
	const char *headerName;

	// Loop over all important headers.
	for (headerIndex = 0;
		 headerIndex < getHeaderCount(global.manager);
		 headerIndex++)
	{
		// Get the header name at the current index
		headerName = getHeaderName(headerIndex);
		// Look for the current header in the request.
		headerValue = searchHeaders(
			ctx,
			headerName);
		if (headerValue) {
			// The request contains the header, so add it to the important
			// headers to be matched.
			EvidenceAddString(
				evidence,
				FIFTYONE_DEGREES_EVIDENCE_HTTP_HEADER_STRING,
				headerName,
				headerValue);
		}
	}
}

/**
 * Obtain the results from the available headers.
 * The results should be freed by ResultsHashFree to prevent memory leak
 * @param ctx the context
 * @param exception the exception object
 */
ResultsHash *obtainResultsFromAvailableHeaders(
	const struct vrt_ctx *ctx,
	Exception *exception)
{
	// Get number of headers being used
	int noOfHeaders = getHeaderCount();

	// Create a results instance to work on
	ResultsHash *results = ResultsHashCreate(global.manager, noOfHeaders, noOfHeaders);

	// Create evidence to hold important headers
	EvidenceKeyValuePairArray *evidence = EvidenceCreate(noOfHeaders);

	// Fill the evidence with values
	setImportantHeaders(ctx, evidence);

	// Get a match for the headers that have just been added and stored
	// in the evidence.
	ResultsHashFromEvidence(results, evidence, exception);

	// Free resource held in evidence
	EvidenceFree(evidence);

	return results;
}

/**
 * VMOD Function
 * Match the HTTP request headers, and return the values for the properties
 * requests for the matched device.
 * @param ctx the context
 * @param propertyInputString the list of property names to return the values
 * @return the values of the requested properties for the device matched with
 * the request headers
 */
VCL_STRING vmod_match_all(
	const struct vrt_ctx *ctx,
	VCL_STRING propertyInputString)
{
	// The pointer in workspace memory to print to.
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively.
	unsigned u, v = 0;

	char *propertiesString, *returnString;
	int propertiesCount;
	char **propertiesArray;

	// Reserve some workspace.
	u = WS_Reserve(ctx->ws, 0);
	// Pointer to the front of workspace area.
	p = ctx->ws->f;

	// Copy the properties string to the workspace memory.
	v += snprintf(p, u, "%s", propertyInputString);
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Skip over the null terminator.
	v++;

	// Create exception
	EXCEPTION_CREATE;

	// Create a results instance to work on
	ResultsHash *results = OBTAIN_RESULTS_FROM_AVAILABLE_HEADERS(ctx, exception);

	// Check exception and return
	if (EXCEPTION_FAILED) {
		logExceptionMessage(exception);
		if (results != NULL) 
			ResultsHashFree(results);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	// This string is modifiable unlike propertyInputString, so getProperties
	// can replace separators with null terminators.
	propertiesString = p;

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pointers to the correct point in the
	// properties string.
	propertiesArray = (char **)p + v;
	v += sizeof(char **) * propertiesCount;
	if (v > u) {
		ResultsHashFree(results);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	stringToArray(propertiesString, propertiesArray, propertiesCount);
	// Move over the array memory.
	returnString = p + v;

	// Print the values to the workspace memory that has been reserved.
	v += printValuesToWorkspace(
		p + v,
		u,
		results,
		propertiesArray,
		propertiesCount);
	
	// Skip over the null terminator.
	v++;

	// Release resource held in results instance
	ResultsHashFree(results);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (returnString);
}

/**
 * @}
 */