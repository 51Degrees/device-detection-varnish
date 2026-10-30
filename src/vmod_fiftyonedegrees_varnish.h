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

#ifndef VMOD_FIFTYONEDEGREES_VARNISH_H
#define VMOD_FIFTYONEDEGREES_VARNISH_H

#include <string.h>

/**
 * @file
 * Differences between the versions of Varnish the modules build against, kept
 * in one place. VRT_MAJOR_VERSION is 7 for Varnish 6.0, 21 for 7.7 and 22 for
 * 8.0. Include this after cache/cache.h and vcc_if.h.
 */

/**
 * Name of the function the $Event line in the .vcc file declares.
 */
#ifdef VPFX
#define VMODFOD_EVENT(name) VPFX(name)
#else
#define VMODFOD_EVENT(name) name
#endif

/**
 * Name of the structure holding a function's optional arguments.
 */
#ifndef VARGS
#define VARGS(name) vmod_##name##_arg
#endif

/**
 * Declares what is needed for VMODFOD_PRIV_SET_FREE to call a function which
 * takes a pointer to the private memory, when the module is discarded.
 */
#if VRT_MAJOR_VERSION >= 13U
#define VMODFOD_PRIV_FREE(freeFunction) \
static void freeFunction##Fini(const struct vrt_ctx *ctx, void *ptr) \
{ \
	(void)ctx; \
	freeFunction(ptr); \
} \
static const struct vmod_priv_methods freeFunction##Methods[1] = {{ \
	.magic = VMOD_PRIV_METHODS_MAGIC, \
	.type = #freeFunction, \
	.fini = freeFunction##Fini \
}};
#define VMODFOD_PRIV_SET_FREE(priv, freeFunction) \
	(priv)->methods = freeFunction##Methods
#else
#define VMODFOD_PRIV_FREE(freeFunction)
#define VMODFOD_PRIV_SET_FREE(priv, freeFunction) \
	(priv)->free = freeFunction
#endif

/**
 * Reserve all the remaining workspace. Release it with WS_Release.
 * @param ws the workspace to reserve
 * @param p set to the start of the reserved memory
 * @return the number of bytes reserved
 */
static inline unsigned vmodfodReserve(struct ws *ws, char **p)
{
	unsigned u = WS_ReserveAll(ws);
#if VRT_MAJOR_VERSION >= 12U
	*p = WS_Reservation(ws);
#else
	*p = ws->f;
#endif
	return u;
}

/**
 * Remove a header.
 * @param hp the headers to remove it from
 * @param name the name of the header including a trailing colon, prefixed with
 * a byte holding the length of the name
 */
static inline void vmodfodUnsetHeader(struct http *hp, const char *name)
{
#if VRT_MAJOR_VERSION >= 22U
	hdr_t hdr;
	CAST_HDR(hdr, name);
	http_Unset(hp, hdr);
#else
	http_Unset(hp, name);
#endif
}

/**
 * Find the length of the values the 51Degrees library wrote to a buffer. The
 * count it returns includes the null terminator when there is room for one, and
 * is the length of all the values when they do not fit, so the string is measured
 * and the count only shows whether the values were cut short.
 * @param p the buffer, with the last byte cleared before the library was called
 * @param u the size of the buffer
 * @param count the count the library returned
 * @return the length excluding the null terminator, or u if the values do not fit
 */
static inline unsigned vmodfodValueLength(char *p, unsigned u, size_t count)
{
	const char *end = (const char *)memchr(p, '\0', u);
	if (end == NULL || count > (size_t)(end - p) + 1) {
		p[u - 1] = '\0';
		return u;
	}
	return (unsigned)(end - p);
}

#endif
