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
#include <GeoIPCity.h>

#include "vrt.h"
#include "vrt_obj.h"
#include "cache/cache.h"

#include "vcc_if.h"


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

static const char *_mk_Unknown(const char *p)
{
    return p ? p : "Unknown";
}

GeoIPRecord *
vmod_geoip_record(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	if (!pp->priv) {
		init_priv(pp);
	}

	return GeoIP_record_by_addr((GeoIP *)pp->priv, ip);
}

VCL_STRING
vmod_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->country_code), strlen (_mk_Unknown(gir->country_code)));
}

VCL_STRING
vmod_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->country_name), strlen (_mk_Unknown(gir->country_name)));
}

VCL_STRING
vmod_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->region), strlen (_mk_Unknown(gir->region)));
}

VCL_STRING
vmod_region_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(GeoIP_region_name_by_code(gir->country_code, gir->region)), strlen (_mk_Unknown(gir->country_code)));
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
vmod_client_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp) {
	return vmod_country_name(ctx, pp, VRT_IP_string(ctx, VRT_r_client_ip(ctx)));
}

VCL_STRING
vmod_ip_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_IP ip) {
	return vmod_country_name(ctx, pp, VRT_IP_string(ctx, ip));
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
