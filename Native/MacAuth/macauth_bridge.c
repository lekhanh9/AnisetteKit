#include "anisette.h"
#include "../include/anisette_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t get_anisette_headers_macauth(
    const char *authkit_path,
    const char *aidauth_path,
    char **out_json
) {
    if (!out_json) return ANISETTE_ERR_INVALID_ARGUMENT;

    int r = macauth_init(authkit_path, aidauth_path);
    if (r != 0) {
        *out_json = strdup("{\"error\":\"Failed to initialize MacAuth emulator\"}");
        return ANISETTE_ERR_LOADER_FAILED;
    }

    anisette_headers_t h;
    r = macauth_fetch_anisette_headers(&h);
    if (r != 0) {
        *out_json = strdup("{\"error\":\"Failed to fetch Anisette headers from MacAuth\"}");
        return ANISETTE_ERR_LOADER_FAILED;
    }

    /* Build JSON matching AnisetteDataResponse */
    size_t cap = 2048;
    char *json = (char *)malloc(cap);
    if (!json) {
        macauth_free_anisette_headers(&h);
        return ANISETTE_ERR_LOADER_FAILED;
    }

    snprintf(json, cap,
             "{\"X-Apple-I-MD-M\":\"%s\","
             "\"X-Apple-I-MD\":\"%s\","
             "\"X-Apple-I-MD-RINFO\":\"%s\","
             "\"X-Apple-I-MD-LU\":\"%s\","
             "\"X-Apple-I-TimeZone\":\"%s\","
             "\"X-Apple-I-Locale\":\"%s\","
             "\"X-Apple-I-Client-Time\":\"%s\","
             "\"X-MMe-Client-Info\":\"%s\","
             "\"User-Agent\":\"%s\"}",
             h.x_apple_i_md_m ? h.x_apple_i_md_m : "",
             h.x_apple_i_md ? h.x_apple_i_md : "",
             h.x_apple_i_md_rinfo ? h.x_apple_i_md_rinfo : "17106176",
             h.x_apple_i_md_lu ? h.x_apple_i_md_lu : "",
             h.x_apple_i_timezone ? h.x_apple_i_timezone : "UTC",
             h.x_apple_i_locale ? h.x_apple_i_locale : "en_US",
             h.x_apple_i_client_time ? h.x_apple_i_client_time : "",
             h.x_mme_client_info ? h.x_mme_client_info : "",
             h.user_agent ? h.user_agent : "");

    macauth_free_anisette_headers(&h);
    *out_json = json;
    return ANISETTE_OK;
}
