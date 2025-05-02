//
// Created by Vladimir Ponomarenko on 23.04.2025.
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


static int _decode_lte_nas_emm_state(const char* b, int offset, size_t length, json& j) {
    int start = offset;

    if (j.find("Version") == j.end() || !j["Version"].is_number()) {
        LOGD("(MI)LTE NAS EMM State missing or invalid Version");
        j["error"] = "Internal Error: Missing Version";
        return 0;
    }
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_NAS_EMM_State with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 2: {

            int consumed = _decode_by_fmt(LteNasEmmStateFmt_v2,
                                          ARRAY_SIZE(LteNasEmmStateFmt_v2, Fmt),
                                          b, offset, length, j);
            offset += consumed;


            if (j.find("EMM State") == j.end() || !j["EMM State"].is_number() ||
                j.find("EMM Substate") == j.end() || !j["EMM Substate"].is_number()) {
                LOGD("(MI)LTE NAS EMM State v2 missing EMM State or Substate field after decoding.");
                j["error"] = "Missing EMM State or Substate";
                return offset - start;
            }


            int emm_state_id = j["EMM State"];
            const char* emm_state_name = search_name(LteNasEmmState_v2_EmmState,
                                                     ARRAY_SIZE(LteNasEmmState_v2_EmmState, ValueName),
                                                     emm_state_id);
            j["EMM State"] = (emm_state_name ? emm_state_name : "(MI)Unknown");


            int emm_substate_id = j["EMM Substate"];
            const ValueName* substate_table = nullptr;
            size_t substate_table_size = 0;

            switch (emm_state_id) {
                case 1: // EMM_DEREGISTERED
                    substate_table = LteNasEmmState_v2_EmmSubstate_Deregistered;
                    substate_table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Deregistered, ValueName);
                    break;
                case 2: // EMM_REGISTERED_INITIATED
                    substate_table = LteNasEmmState_v2_EmmSubstate_Registered_Initiated;
                    substate_table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered_Initiated, ValueName);
                    break;
                case 3:
                case 4: // EMM_TRACKING_AREA_UPDATING_INITIATED
                case 5: // EMM_SERVICE_REQUEST_INITIATED
                    substate_table = LteNasEmmState_v2_EmmSubstate_Registered;
                    substate_table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered, ValueName);
                    break;
                case 0: // EMM_NULL
                case 6: // EMM_DEREGISTERED_INITIATED
                default:

                    break;
            }

            if (substate_table != nullptr && substate_table_size > 0) {
                const char* substate_name = search_name(substate_table, substate_table_size, emm_substate_id);
                j["EMM Substate"] = (substate_name ? substate_name : "(MI)Unknown");
            } else {

                j["EMM Substate"] = "Undefined";
            }


            if (j.find("GUTI Valid") != j.end() && j["GUTI Valid"].is_number()) {
                j["GUTI Valid"] = (j["GUTI Valid"].get<int>() != 0);
            }



            break;
        }
        default:
            LOGD("(MI)Unknown LTE NAS EMM State version: 0x%x", pkt_ver);
            j["error"] = "Unknown Version";

            return 0;
    }

    return offset - start;
}