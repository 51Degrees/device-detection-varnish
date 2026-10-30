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
#include "vcc_if.h"
#include "../vmod_fiftyonedegrees_varnish.h"
#include "../../cxx/src/ipi/fiftyone.h"

/**
 * @defgroup vmod_fiftyonedegrees_ipi 51Degrees IP Intelligence VMOD Internals
 *
 * @{
 */

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

	state->config = IpiDefaultConfig;
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
 * @}
 */
