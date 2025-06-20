//
// Created by Vladimir Ponomarenko on 22.06.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"
#include "ws_wrap.h"
#include "pdml_parser.h"

using json = nlohmann::json;
using namespace std;

extern bool epan_initialized;


static int _decode_lte_nas_plain(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Pkt Version") == j.end() || !j["Pkt Version"].is_number()) {
        LOGD("LTE_NAS_Plain: Missing or invalid 'Pkt Version'");
        j["error"] = "Missing Pkt Version for NAS PLAIN payload";
        return 0;
    }
    pkt_ver = j["Pkt Version"].get<int>();
    //LOGD("Decoding LTE NAS Plain payload with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            int consumed_v1_header = _decode_by_fmt(LteNasPlainFmt_v1,
                                                    ARRAY_SIZE(LteNasPlainFmt_v1, Fmt),
                                                    b, offset, length, j);
            offset += consumed_v1_header;
            break;
        }
        default:
            LOGD("(MI)Unknown LTE NAS version: 0x%x", pkt_ver);
            j["error"] = "Unknown NAS Version " + std::to_string(pkt_ver);
            return offset - start;
    }

    size_t pdu_length = length - offset;

    if (pdu_length > 0) {
        std::string msg_hex = bytes_to_hex_string(b + offset, pdu_length);
        j["Msg_RAW_Hex"] = msg_hex;

        const char* nas_dissector_name = "LTE-NAS_EPS_PLAIN";
        j["Msg_Type"] = nas_dissector_name;

        if (!epan_initialized) {
            LOGD("Initializing epan for NAS PLAIN dissection");
            initialize_epan();
            epan_initialized = true;
            LOGD("epan initialized for NAS");
        }

        std::string ws_result = decode_msg(nas_dissector_name, msg_hex);

        if (ws_result.rfind("Error", 0) == 0 || ws_result.rfind("Unknown", 0) == 0 || ws_result.rfind("character", 0) != std::string::npos || ws_result.empty()) {
            LOGD("Wireshark dissection failed for %s: %s", nas_dissector_name, ws_result.c_str());
            j["PDU_Decoded"] = "(Dissection failed: " + ws_result + ")";
        } else {
            json dissected_json_result = parse_pdml(ws_result, "nas-eps");

            if (dissected_json_result.count("parsing_error") > 0) {
                j["PDU_Decoded"] = "(PDML Parsing failed: " + dissected_json_result["parsing_error"].get<std::string>() + ")";
                j["PDU_Decoded_RawPDML"] = ws_result;
            } else if (dissected_json_result.empty()) {
                j["PDU_Decoded"] = "(PDML Parsing yielded empty result)";
                j["PDU_Decoded_RawPDML"] = ws_result;
            } else {

                bool data_found_in_dissection = false;
                if (dissected_json_result.find("nas-eps") != dissected_json_result.end() && !dissected_json_result["nas-eps"].empty()) {
                    j["PDU_Decoded"] = dissected_json_result["nas-eps"];
                    data_found_in_dissection = true;
                } else {

                    for (auto it = dissected_json_result.begin(); it != dissected_json_result.end(); ++it) {
                        if (it.value().is_object() && !it.value().empty()) {
                            j["PDU_Decoded"] = it.value();
                            data_found_in_dissection = true;
                            break;
                        } else if (!it.value().is_object() && !it.value().is_null()){
                            j["PDU_Decoded"] = dissected_json_result;
                            data_found_in_dissection = true;
                            break;
                        }
                    }
                }
                if (!data_found_in_dissection) {
                    j["PDU_Decoded"] = "(PDML Parsing yielded no main data)";
                    j["PDU_Decoded_RawPDML"] = ws_result;
                }
            }
        }
        offset += pdu_length;
    } else {
        j["Msg_RAW_Hex"] = "";
        j["Msg_Type"] = "LTE-NAS_EPS_PLAIN (empty)";
        j["PDU_Decoded"] = "(No PDU data)";
    }
    return offset - start;
}