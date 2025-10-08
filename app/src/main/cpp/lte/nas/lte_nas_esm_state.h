//
// Created by Vladimir Ponomarenko on 09.10.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

static int _decode_lte_nas_esm_state(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_NAS_ESM_State: Missing or invalid 'Version'");
        j["error"] = "Missing Version for NAS ESM State payload";
        return 0;
    }

//    LOGD("Decoding LTE NAS ESM State payload with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            int consumed = _decode_by_fmt(LteNasEsmStateFmt_v1,
                                          ARRAY_SIZE(LteNasEsmStateFmt_v1, Fmt),
                                          b, offset, length, j);
            offset += consumed;
            break;
        }
        default:
            LOGD("(MI)Unknown LTE NAS ESM State version: 0x%x", pkt_ver);
            j["error"] = "Unknown NAS ESM State Version " + std::to_string(pkt_ver);
            return offset - start;
    }

    return offset - start;
}