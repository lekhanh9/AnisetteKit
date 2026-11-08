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

int32_t start_provision_macauth(
    const char *authkit_path,
    const uint8_t *spim,
    uint32_t spim_len,
    char **out_json
) {
    if (!out_json) return ANISETTE_ERR_INVALID_ARGUMENT;

    int r = macauth_init(authkit_path, NULL);
    if (r != 0) {
        *out_json = strdup("{\"error\":\"Failed to initialize MacAuth emulator\"}");
        return ANISETTE_ERR_LOADER_FAILED;
    }

    uint8_t *cpim = NULL;
    uint32_t cpim_len = 0;
    uint32_t session = 0;

    r = macauth_start_provision(spim, spim_len, &cpim, &cpim_len, &session);
    if (r != 0 && r != -45061) {
        if (cpim) free(cpim);
        *out_json = strdup("{\"error\":\"ADIProvisioningStart failed\"}");
        return r;
    }

    char *cpim_b64 = cpim ? macauth_base64_encode(cpim, cpim_len) : strdup("");
    if (cpim) free(cpim);

    size_t cap = (cpim_b64 ? strlen(cpim_b64) : 0) + 128;
    char *json = (char *)malloc(cap);
    if (!json) {
        if (cpim_b64) free(cpim_b64);
        return ANISETTE_ERR_LOADER_FAILED;
    }

    snprintf(json, cap, "{\"cpim\":\"%s\",\"session\":%u}", cpim_b64 ? cpim_b64 : "", session);
    if (cpim_b64) free(cpim_b64);

    *out_json = json;
    return ANISETTE_OK;
}

int32_t end_provision_macauth(
    const char *authkit_path,
    uint32_t session,
    const uint8_t *ptm,
    uint32_t ptm_len,
    char **out_json
) {
    if (!out_json) return ANISETTE_ERR_INVALID_ARGUMENT;

    int r = macauth_init(authkit_path, NULL);
    if (r != 0) {
        *out_json = strdup("{\"error\":\"Failed to initialize MacAuth emulator\"}");
        return ANISETTE_ERR_LOADER_FAILED;
    }

    uint8_t *routing = NULL;
    uint32_t routing_len = 0;

    r = macauth_end_provision(session, ptm, ptm_len, &routing, &routing_len);
    if (r != 0) {
        if (routing) free(routing);
        *out_json = strdup("{\"error\":\"ADIProvisioningEnd failed\"}");
        return r;
    }

    char *routing_b64 = routing ? macauth_base64_encode(routing, routing_len) : strdup("");
    if (routing) free(routing);

    size_t cap = (routing_b64 ? strlen(routing_b64) : 0) + 128;
    char *json = (char *)malloc(cap);
    if (!json) {
        if (routing_b64) free(routing_b64);
        return ANISETTE_ERR_LOADER_FAILED;
    }

    snprintf(json, cap, "{\"adi_pb_base64\":\"%s\"}", routing_b64 ? routing_b64 : "");
    if (routing_b64) free(routing_b64);

    *out_json = json;
    return ANISETTE_OK;
}
