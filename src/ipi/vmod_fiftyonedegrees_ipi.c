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
#include <stdbool.h>
#include <string.h>

#include "cache/cache.h"
#include "vrt_obj.h"
#include "vcc_if.h"
#include "../vmod_fiftyonedegrees_varnish.h"
#include "../../cxx/src/ipi/fiftyone.h"

/**
 * @defgroup vmod_fiftyonedegrees_ipi 51Degrees IP Intelligence VMOD Internals
 *
 * @{
 */

/** Value returned for a property with no value, or when there is no match. */
#define VMODIPI_NOT_AVAILABLE "N/A"
/** Separates the values of a property which has more than one. */
#define VMODIPI_VALUE_SEPARATOR "|"
/** Start of every message from this module. */
#define VMODIPI_LOG_PREFIX "vmod_fiftyonedegrees_ipi: "

/**
 * State of the module for one VCL. Each VCL loads its own data set, so one
 * VCL can be discarded whilst another keeps running.
 */
typedef struct vmodipi_state_t {
	ResourceManager manager; /**< Holds the data set */
	ConfigIpi config; /**< Configuration for the data set */
	bool started; /**< True once the data set has loaded */
	char *requiredProperties; /**< Properties to load, or NULL for all */
	char *delimiter; /**< Separates the values of different properties */
	char *performanceProfile; /**< Name of the profile, or NULL for default */
	int maxConcurrency; /**< Expected concurrent requests, or 0 for default */
} vmodipi_state;

/**
 * Called when the VCL is discarded.
 * @param ptr the state of the module for the VCL
 */
static void stateFree(void *ptr)
{
	vmodipi_state *state = (vmodipi_state *)ptr;
	if (state->started) {
		ResourceManagerFree(&state->manager);
	}
	free(state->requiredProperties);
	free(state->delimiter);
	free(state->performanceProfile);
	free(state);
}

VMODFOD_PRIV_FREE(stateFree)

/**
 * Allocates the state of the module when a VCL is loaded.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param e the event identifier
 * @return zero, or non-zero if the VCL must not be loaded
 */
int VMODFOD_EVENT(init_function)(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	enum vcl_event_e e)
{
	vmodipi_state *state;
	if (e == VCL_EVENT_LOAD) {
		state = (vmodipi_state *)calloc(1, sizeof(vmodipi_state));
		if (state == NULL) {
			fprintf(stderr,
				VMODIPI_LOG_PREFIX "Insufficient memory allocated.\n");
			return (-1);
		}
		priv->priv = state;
		VMODFOD_PRIV_SET_FREE(priv, stateFree);
	}
	return (0);
}

/**
 * Records a problem with the current request. The request carries on.
 * @param ctx the context
 * @param message the problem
 * @param detail further detail, such as the value which caused the problem
 */
static void logProblem(
	const struct vrt_ctx *ctx,
	const char *message,
	const char *detail)
{
	if (ctx->vsl != NULL) {
		VSLb(ctx->vsl, SLT_VCL_Error,
			VMODIPI_LOG_PREFIX "%s (%s)", message, detail);
	}
	else {
		fprintf(stderr, VMODIPI_LOG_PREFIX "%s (%s)\n", message, detail);
	}
}

/**
 * Records the message held in an exception.
 * @param ctx the context
 * @param message what was being done
 * @param exception the exception
 */
static void logException(
	const struct vrt_ctx *ctx,
	const char *message,
	Exception *exception)
{
	const char *detail = ExceptionGetMessage(exception);
	logProblem(ctx, message, detail == NULL ? "unknown" : detail);
	if (detail != NULL) {
		fiftyoneDegreesFree((void *)detail);
	}
}

/**
 * Replaces a saved setting with a copy of a string, as VCL only keeps the
 * strings it is given for as long as it needs them.
 * @param ctx the context
 * @param state the state of the module for the VCL
 * @param setting the setting to replace
 * @param value the new value, which can be NULL
 */
static void setString(
	const struct vrt_ctx *ctx,
	vmodipi_state *state,
	char **setting,
	const char *value)
{
	char *copy = NULL;
	if (state->started) {
		logProblem(ctx, "Settings must be made before start", "ignored");
		return;
	}
	if (value != NULL) {
		copy = strdup(value);
		if (copy == NULL) {
			VRT_fail(ctx, VMODIPI_LOG_PREFIX "Insufficient memory allocated.");
			return;
		}
	}
	free(*setting);
	*setting = copy;
}

/**
 * VMOD Function
 * Set the properties to load when the data set starts.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param properties the list of required properties
 */
VCL_VOID vmod_set_properties(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_STRING properties)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	setString(ctx, state, &state->requiredProperties, properties);
}

/**
 * VMOD Function
 * Set the delimiter to use when returning values for multiple properties.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param delimiter the delimiter string to use
 */
VCL_VOID vmod_set_delimiter(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_STRING delimiter)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	setString(ctx, state, &state->delimiter, delimiter);
}

/**
 * VMOD Function
 * Set the performance profile which sets how much of the data set is held in
 * memory.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param performanceProfile the name of the performance profile
 */
VCL_VOID vmod_set_performance_profile(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_STRING performanceProfile)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	setString(ctx, state, &state->performanceProfile, performanceProfile);
}

/**
 * VMOD Function
 * Set the maximum number of expected concurrent requests.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param concurrency the maximum number of expected concurrent requests
 */
VCL_VOID vmod_set_max_concurrency(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_INT concurrency)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	if (state->started) {
		logProblem(ctx, "Settings must be made before start", "ignored");
		return;
	}
	state->maxConcurrency = (int)concurrency;
}

/**
 * Sets the configuration from the settings made in the VCL.
 * @param ctx the context
 * @param state the state of the module for the VCL
 */
static void initConfig(const struct vrt_ctx *ctx, vmodipi_state *state)
{
	const char *profile = state->performanceProfile;
	if (profile == NULL || strcmp(profile, "DEFAULT") == 0) {
		state->config = IpiDefaultConfig;
	}
	else if (strcmp(profile, "IN_MEMORY") == 0) {
		state->config = IpiInMemoryConfig;
	}
	else if (strcmp(profile, "HIGH_PERFORMANCE") == 0) {
		state->config = IpiHighPerformanceConfig;
	}
	else if (strcmp(profile, "LOW_MEMORY") == 0) {
		state->config = IpiLowMemoryConfig;
	}
	else if (strcmp(profile, "BALANCED") == 0) {
		state->config = IpiBalancedConfig;
	}
	else {
		logProblem(ctx, "Unrecognised performance profile, using DEFAULT", profile);
		state->config = IpiDefaultConfig;
	}

	if (state->maxConcurrency > 0) {
		uint16_t concurrency = (uint16_t)state->maxConcurrency;
		state->config.strings.concurrency = concurrency;
		state->config.components.concurrency = concurrency;
		state->config.maps.concurrency = concurrency;
		state->config.properties.concurrency = concurrency;
		state->config.values.concurrency = concurrency;
		state->config.profiles.concurrency = concurrency;
		state->config.graphs.concurrency = concurrency;
		state->config.profileGroups.concurrency = concurrency;
		state->config.profileOffsets.concurrency = concurrency;
		state->config.propertyTypes.concurrency = concurrency;
		state->config.graph.concurrency = concurrency;
	}
}

/**
 * VMOD Function
 * Load the data set from a data file. If the file cannot be used the VCL does
 * not load, and Varnish carries on with the VCL it was already using.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param filePath the path to the data file to be used
 */
VCL_VOID vmod_start(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_STRING filePath)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	PropertiesRequired properties = PropertiesDefault;
	StatusCode status;
	const char *message;
	EXCEPTION_CREATE;

	if (state->started) {
		VRT_fail(ctx, VMODIPI_LOG_PREFIX "start has already been called.");
		return;
	}
	if (filePath == NULL || *filePath == '\0') {
		VRT_fail(ctx, VMODIPI_LOG_PREFIX "start needs the path of a data file.");
		return;
	}

	initConfig(ctx, state);
	properties.string = state->requiredProperties;
	status = IpiInitManagerFromFile(
		&state->manager,
		&state->config,
		&properties,
		filePath,
		exception);
	if (status != SUCCESS) {
		message = StatusGetMessage(status, filePath);
		VRT_fail(ctx, VMODIPI_LOG_PREFIX "%s",
			message == NULL ? "The data file could not be used." : message);
		if (message != NULL) {
			fiftyoneDegreesFree((void *)message);
		}
		return;
	}
	state->started = true;
}

/**
 * VMOD Function
 * Reload the data set from a data file. If that fails the data set already
 * loaded stays in use.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @param filePath the path to the data file. If this is empty the data set is
 * reloaded from the file it was loaded from.
 */
VCL_VOID vmod_reload(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	VCL_STRING filePath)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	StatusCode status;
	const char *message;
	EXCEPTION_CREATE;

	if (!state->started) {
		logProblem(ctx, "reload needs start to have been called", "ignored");
		return;
	}
	if (filePath != NULL && *filePath != '\0') {
		status = IpiReloadManagerFromFile(
			&state->manager,
			filePath,
			exception);
	}
	else {
		filePath = "the original data file";
		status = IpiReloadManagerFromOriginalFile(
			&state->manager,
			exception);
	}
	if (status != SUCCESS) {
		message = StatusGetMessage(status, filePath);
		logProblem(ctx, "Reload failed, the data set is unchanged",
			message == NULL ? filePath : message);
		if (message != NULL) {
			fiftyoneDegreesFree((void *)message);
		}
	}
}

/**
 * Cuts a comma separated string of property names into names, by replacing the
 * commas with null terminators. An empty name counts, so that each value stays
 * in the same position as the name it is for.
 * @param names the string to cut
 * @return the number of names
 */
static int splitProperties(char *names)
{
	int count;
	if (*names == '\0') {
		return 0;
	}
	for (count = 1; *names != '\0'; names++) {
		if (*names == ',') {
			*names = '\0';
			count++;
		}
	}
	return count;
}

/**
 * Writes the values of one property.
 * @param ctx the context
 * @param results the results of the match, or NULL if there is no match
 * @param name the name of the property
 * @param p where to write
 * @param u the space available, which is more than zero
 * @return the number of characters written, excluding the null terminator, or
 * u if the values do not fit
 */
static unsigned writeValue(
	const struct vrt_ctx *ctx,
	ResultsIpi *results,
	const char *name,
	char *p,
	unsigned u)
{
	int index;
	bool hasValues;
	size_t count;
	unsigned length;
	EXCEPTION_CREATE;

	p[0] = '\0';
	p[u - 1] = '\0';

	if (results != NULL) {
		index = PropertiesGetRequiredPropertyIndexFromName(
			((DataSetIpi *)results->b.dataSet)->b.b.available,
			name);
		if (index >= 0) {
			hasValues = ResultsIpiGetHasValues(results, index, exception);
			if (EXCEPTION_FAILED) {
				logException(ctx, "Could not read a property", exception);
			}
			else if (hasValues) {
				count = ResultsIpiGetValuesStringByRequiredPropertyIndex(
					results,
					index,
					p,
					u,
					VMODIPI_VALUE_SEPARATOR,
					exception);
				if (EXCEPTION_FAILED) {
					logException(ctx, "Could not read a property", exception);
				}
				else {
					length = vmodfodValueLength(p, u, count);
					if (length > 0) {
						return length;
					}
				}
			}
		}
	}

	length = (unsigned)snprintf(p, u, "%s", VMODIPI_NOT_AVAILABLE);
	return length >= u ? u : length;
}

/**
 * Counts the bytes needed to escape a string for use as a header value, in the
 * way the nginx module does, so the same request gives the same value.
 * @param c the character
 * @return the number of bytes the character takes once escaped
 */
static unsigned escapedLength(unsigned char c)
{
	if (c > 0x1f) {
		return (c == '\\' || c == '"') ? 2 : 1;
	}
	return (c == '\n' || c == '\r' || c == '\t' || c == '\b' || c == '\f') ? 2 : 6;
}

/**
 * Escapes a string for use as a header value where it is held, from the end so
 * that nothing is overwritten before it has been moved.
 * @param p the string
 * @param length the length of the string
 * @param u the space available including the null terminator
 * @return the new length of the string, excluding the null terminator, or u if
 * it does not fit
 */
static unsigned escapeValue(char *p, unsigned length, unsigned u)
{
	static const char hex[] = "0123456789abcdef";
	unsigned i, escaped = 0;
	char *to;
	unsigned char c;

	for (i = 0; i < length; i++) {
		escaped += escapedLength((unsigned char)p[i]);
	}
	if (escaped >= u) {
		return u;
	}

	p[escaped] = '\0';
	to = p + escaped;
	for (i = length; i > 0; i--) {
		c = (unsigned char)p[i - 1];
		switch (escapedLength(c)) {
		case 1:
			*--to = (char)c;
			break;
		case 2:
			switch (c) {
			case '\n': *--to = 'n'; break;
			case '\r': *--to = 'r'; break;
			case '\t': *--to = 't'; break;
			case '\b': *--to = 'b'; break;
			case '\f': *--to = 'f'; break;
			default: *--to = (char)c; break;
			}
			*--to = '\\';
			break;
		default:
			*--to = hex[c & 0x0f];
			*--to = hex[c >> 4];
			*--to = '0';
			*--to = '0';
			*--to = 'u';
			*--to = '\\';
			break;
		}
	}
	return escaped;
}

/**
 * Gets the results for an address. A problem with the address is not a problem
 * with the request, so it gives no results rather than a failure.
 * @param ctx the context
 * @param state the state of the module for the VCL
 * @param ip the address, or NULL if there is none
 * @return results to free with ResultsIpiFree, or NULL if there are none
 */
static ResultsIpi *getResults(
	const struct vrt_ctx *ctx,
	vmodipi_state *state,
	const char *ip)
{
	ResultsIpi *results;
	EXCEPTION_CREATE;

	if (ip == NULL || *ip == '\0') {
		logProblem(ctx, "No IP address to look up", "using " VMODIPI_NOT_AVAILABLE);
		return NULL;
	}
	results = ResultsIpiCreate(&state->manager);
	if (results == NULL) {
		logProblem(ctx, "Insufficient memory allocated", ip);
		return NULL;
	}
	ResultsIpiFromIpAddressString(results, ip, strlen(ip), exception);
	if (EXCEPTION_FAILED) {
		logException(ctx, "Could not look up the IP address", exception);
		ResultsIpiFree(results);
		return NULL;
	}
	return results;
}

/**
 * VMOD Function
 * Look up an IP address, and return the values of the requested properties.
 * @param ctx the context
 * @param a the arguments, with the properties to return and the optional
 * address. With no address the address of the client is used.
 * @return the values, or NULL if they do not fit in the workspace
 */
VCL_STRING vmod_match(const struct vrt_ctx *ctx, struct VARGS(match) *a)
{
	vmodipi_state *state = (vmodipi_state *)a->arg1->priv;
	const char *ip = NULL;
	const char *delimiter = state->delimiter == NULL ? "," : state->delimiter;
	const char *name;
	ResultsIpi *results = NULL;
	char *p, *returnString;
	unsigned u, v = 0, length;
	int count, i;

	if (!state->started) {
		logProblem(ctx, "match needs start to have been called", "no data set");
		return (NULL);
	}
	if (a->properties == NULL) {
		logProblem(ctx, "match needs the names of the properties", "none given");
		return (NULL);
	}

	// VRT_IP_string allocates from the workspace, so this has to happen before
	// the workspace is reserved.
	if (a->valid_ip) {
		ip = a->ip;
	}
	else if (ctx->sp != NULL) {
		ip = VRT_IP_string(ctx, VRT_r_client_ip(ctx));
	}

	u = vmodfodReserve(ctx->ws, &p);

	// The names are copied, as cutting them up changes them.
	v = (unsigned)snprintf(p, u, "%s", a->properties);
	if (v >= u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Skip over the null terminator.
	v++;
	count = splitProperties(p);
	returnString = p + v;

	results = getResults(ctx, state, ip);

	// The string is empty if there are no properties.
	if (v < u) {
		returnString[0] = '\0';
	}
	length = 0;
	name = p;
	for (i = 0; i < count; i++) {
		if (i > 0) {
			length += (unsigned)snprintf(
				returnString + length,
				u - v - length,
				"%s",
				delimiter);
		}
		if (v + length >= u) {
			length = u - v;
			break;
		}
		length += writeValue(
			ctx,
			results,
			name,
			returnString + length,
			u - v - length);
		if (v + length >= u) {
			length = u - v;
			break;
		}
		name += strlen(name) + 1;
	}

	if (results != NULL) {
		ResultsIpiFree(results);
	}

	if (v >= u || v + length >= u) {
		// No space, reset and leave.
		logProblem(ctx, "The values do not fit in the workspace", a->properties);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	length = escapeValue(returnString, length, u - v);
	if (v + length >= u) {
		logProblem(ctx, "The values do not fit in the workspace", a->properties);
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	// Skip over the null terminator.
	WS_Release(ctx->ws, v + length + 1);
	return (returnString);
}

/**
 * Copies a string from the data set's strings to the workspace.
 * @param ctx the context
 * @param state the state of the module for the VCL
 * @param offset the offset of the string
 * @return the string, or NULL if it could not be read
 */
static VCL_STRING getDataSetString(
	const struct vrt_ctx *ctx,
	DataSetIpi *dataset,
	uint32_t offset)
{
	Item item;
	const StoredBinaryValue *value;
	VCL_STRING result = NULL;
	EXCEPTION_CREATE;

	DataReset(&item.data);
	value = StoredBinaryValueGet(
		dataset->strings,
		offset,
		FIFTYONE_DEGREES_PROPERTY_VALUE_TYPE_STRING,
		&item,
		exception);
	if (EXCEPTION_FAILED || value == NULL) {
		logProblem(ctx, "Could not read from the data set", "metadata");
		return (NULL);
	}
	result = WS_Copy(ctx->ws, STRING(item.data.ptr), -1);
	COLLECTION_RELEASE(dataset->strings, &item);
	return (result);
}

/**
 * VMOD function
 * Get the name of the data set.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @return the name, or NULL if there is no data set
 */
VCL_STRING vmod_get_dataset_name(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	DataSetIpi *dataset;
	VCL_STRING result;
	if (!state->started) {
		return (NULL);
	}
	dataset = DataSetIpiGet(&state->manager);
	result = getDataSetString(ctx, dataset, dataset->header.nameOffset);
	DataSetIpiRelease(dataset);
	return (result);
}

/**
 * VMOD function
 * Get the format of the data set.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @return the format, or NULL if there is no data set
 */
VCL_STRING vmod_get_dataset_format(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	DataSetIpi *dataset;
	VCL_STRING result;
	if (!state->started) {
		return (NULL);
	}
	dataset = DataSetIpiGet(&state->manager);
	result = getDataSetString(ctx, dataset, dataset->header.formatOffset);
	DataSetIpiRelease(dataset);
	return (result);
}

/**
 * VMOD function
 * Get the date the data set was published, as year-month-day.
 * @param ctx the context
 * @param priv the private memory pointer for the VCL
 * @return the date, or NULL if there is no data set
 */
VCL_STRING vmod_get_dataset_published_date(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv)
{
	vmodipi_state *state = (vmodipi_state *)priv->priv;
	DataSetIpi *dataset;
	VCL_STRING result;
	if (!state->started) {
		return (NULL);
	}
	dataset = DataSetIpiGet(&state->manager);
	result = WS_Printf(ctx->ws, "%d-%d-%d",
		dataset->header.published.year,
		(int)dataset->header.published.month,
		(int)dataset->header.published.day);
	DataSetIpiRelease(dataset);
	return (result);
}

/**
 * @}
 */
