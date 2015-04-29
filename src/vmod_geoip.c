/**
 * libvmod-geoip - varnish interface to MaxMind's GeoIP library
 * GeoIP API: http://www.maxmind.com/app/c
 *
 * See file README.rst for usage instructions
 * 
 * This code is licensed under a MIT-style License, see file LICENSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <GeoIP.h>

#include "vrt.h"
#include "vrt_obj.h"
#include "cache/cache.h"

#include "vcc_if.h"


// The default string in case the GeoIP lookup fails
#define GI_UNKNOWN_STRING "Unknown"


int
init_function(struct vmod_priv *pp, const struct VCL_conf *conf)
{
	// first call to lookup functions initializes pp
	return (0);
}

static void
init_priv(struct vmod_priv *pp) {
	// The README says:
	// If GEOIP_MMAP_CACHE doesn't work on a 64bit machine, try adding
	// the flag "MAP_32BIT" to the mmap call. MMAP is not avail for WIN32.
	pp->priv = GeoIP_new(GEOIP_MMAP_CACHE);
	if (pp->priv != NULL) {
		pp->free = (vmod_priv_free_f *)GeoIP_delete;
		GeoIP_set_charset((GeoIP *)pp->priv, GEOIP_CHARSET_UTF8);
	}
}

VCL_STRING
vmod_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	const char* data = NULL;

	if (!pp->priv) {
		init_priv(pp);
	}

	if (ip) {
		data = GeoIP_country_code_by_addr((GeoIP *)pp->priv, ip);
	}
	data = data ? data : GI_UNKNOWN_STRING;

	return WS_Copy(ctx->ws, data, strlen (data));
}

VCL_STRING
vmod_client_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp) {
	return vmod_country_code(ctx, pp, VRT_IP_string(ctx, VRT_r_client_ip(ctx)));
}

VCL_STRING
vmod_ip_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_IP ip) {
	return vmod_country_code(ctx, pp, VRT_IP_string(ctx, ip));
}


VCL_STRING
vmod_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	const char* data = NULL;

	if (!pp->priv) {
		init_priv(pp);
	}

	if (ip) {
		data = GeoIP_country_name_by_addr((GeoIP *)pp->priv, ip);
	}

	data = data ? data : GI_UNKNOWN_STRING;

	return WS_Copy(ctx->ws, data, strlen (data));
}

VCL_STRING
vmod_client_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp) {
	return vmod_country_name(ctx, pp, VRT_IP_string(ctx, VRT_r_client_ip(ctx)));
}

VCL_STRING
vmod_ip_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_IP ip) {
	return vmod_country_name(ctx, pp, VRT_IP_string(ctx, ip));
}


VCL_STRING
vmod_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRegion *gir;
	const char* data = NULL;

	if (!pp->priv) {
		init_priv(pp);
	}

	if (ip) {
		if ( (gir = GeoIP_region_by_addr((GeoIP *)pp->priv, ip)) ) {
			data = GeoIP_region_name_by_code(gir->country_code, gir->region);
			// TODO: is gir* a local copy or the actual record?
			GeoIPRegion_delete(gir);
		}
	}

	data = data ? data : GI_UNKNOWN_STRING;

	return WS_Copy(ctx->ws, data, strlen (data));
}

VCL_STRING
vmod_region_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip) {
   GeoIPRegion *gir;
	const char* data = NULL;

	if (!pp->priv) {
		init_priv(pp);
	}

	if (ip) {
		if ( (gir = GeoIP_region_by_addr((GeoIP *)pp->priv, ip)) ) {
			data = gir->region;
			// TODO: is gir* a local copy or the actual record?
			GeoIPRegion_delete(gir);
		}
	}

	data = data ? data : GI_UNKNOWN_STRING;

	return WS_Copy(ctx->ws, data, strlen (data));
}

VCL_STRING
vmod_client_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp) {
	return vmod_region_name(ctx, pp, VRT_IP_string(ctx, VRT_r_client_ip(ctx)));
}

VCL_STRING
vmod_ip_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_IP ip) {
	return vmod_region_name(ctx, pp, VRT_IP_string(ctx, ip));
}

VCL_STRING
vmod_client_region_code(const struct vrt_ctx *ctx, struct vmod_priv *pp) {
	return vmod_region_code(ctx, pp, VRT_IP_string(ctx, VRT_r_client_ip(ctx)));
}
