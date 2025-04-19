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
#include "ws_wrap.h"

using json = nlohmann::json;
using namespace std;

static bool epan_initialized = false;


static int _decode_lte_rrc_ota(const char* b, int offset, size_t length, json& j) {
    int start_payload = offset;
    if (j.find("Pkt Version") == j.end() || !j["Pkt Version"].is_number()) {
        LOGD("(MI)LTE RRC OTA missing or invalid Pkt Version in _decode_lte_rrc_ota");
        j["error"] = "Internal Error: Missing Pkt Version";
        return 0;
    }
    int pkt_ver = j["Pkt Version"];
    LOGD("Decoding LTE_RRC_OTA_Packet payload with version: %d", pkt_ver);

    int version_specific_consumed = 0;
    const Fmt* version_fmt_array = nullptr;
    size_t version_fmt_size = 0;

    switch (pkt_ver) {
        case 2:  version_fmt_array = LteRrcOtaPacketFmt_v2; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v2, Fmt); break;
        case 4:  version_fmt_array = LteRrcOtaPacketFmt_v4; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v4, Fmt); break;
        case 7:  version_fmt_array = LteRrcOtaPacketFmt_v7; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v7, Fmt); break;
        case 8:  version_fmt_array = LteRrcOtaPacketFmt_v8; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v8, Fmt); break;
        case 9:  version_fmt_array = LteRrcOtaPacketFmt_v9; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v9, Fmt); break;
        case 12: version_fmt_array = LteRrcOtaPacketFmt_v12; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v12, Fmt); break;
        case 13: version_fmt_array = LteRrcOtaPacketFmt_v13; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v13, Fmt); break;
        case 15: version_fmt_array = LteRrcOtaPacketFmt_v15; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v15, Fmt); break;
        case 19: version_fmt_array = LteRrcOtaPacketFmt_v19; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v19, Fmt); break;
        case 20: version_fmt_array = LteRrcOtaPacketFmt_v20; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v20, Fmt); break;
        case 24: version_fmt_array = LteRrcOtaPacketFmt_v24; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v24, Fmt); break;
        case 26: version_fmt_array = LteRrcOtaPacketFmt_v26; version_fmt_size = ARRAY_SIZE(LteRrcOtaPacketFmt_v26, Fmt); break;
        default:
            LOGD("(MI)Unknown LTE RRC OTA packet version: %d", pkt_ver);
            j["error"] = "Unknown Version";
            return 0;
    }

    version_specific_consumed = _decode_by_fmt(version_fmt_array, version_fmt_size, b, offset, length, j);
    offset += version_specific_consumed;

    if (j.find("RRC Release Number") != j.end() && j["RRC Release Number"].is_number()) {
        uint8_t rrc_rel_raw = j["RRC Release Number"];
        j["RRC Release"] = (rrc_rel_raw >> 4) & 0x0F;
        j["RRC Major Version"] = rrc_rel_raw & 0x0F;
    }
    if (j.count("Major/minor")) {
        // j.erase("Major/minor");
    }

    if (j.find("PDU Number") == j.end() || !j["PDU Number"].is_number() ||
        j.find("Msg Length") == j.end() || !j["Msg Length"].is_number()) {
        LOGD("(MI)LTE RRC OTA missing PDU Number or Msg Length for version %d after decoding", pkt_ver);
        j["error"] = "Missing PDU Number or Msg Length";
        return offset - start_payload;
    }

    int pdu_number_raw = j["PDU Number"];
    int pdu_length = j["Msg Length"];
    size_t remaining_length = start_payload + length - offset;

    if (pdu_length < 0 || pdu_length > remaining_length) {
        LOGD("(MI)LTE RRC OTA invalid PDU Length (%d) or insufficient data (remaining %zu). Offset: %d", pdu_length, remaining_length, offset);
        j["error"] = "Invalid PDU Length or insufficient data";
        j["Msg_RAW_Hex_Incomplete"] = bytes_to_hex_string(b + offset, remaining_length);
        return offset - start_payload;
    }

    const char* type_name = nullptr;
    int pdu_number_to_search = pdu_number_raw;

    if (pkt_ver < 15) {
        if (pdu_number_raw > 8) {
            pdu_number_to_search = pdu_number_raw - 7;
            LOGD("Applied PDU mapping for ver %d: %d -> %d", pkt_ver, pdu_number_raw, pdu_number_to_search);
        }
        type_name = search_name(LteRrcOtaPduType, ARRAY_SIZE(LteRrcOtaPduType, ValueName), pdu_number_to_search);
    } else if (pkt_ver == 19 || pkt_ver == 26) {
        type_name = search_name(LteRrcOtaPduType_v19, ARRAY_SIZE(LteRrcOtaPduType_v19, ValueName), pdu_number_to_search);
    } else {
        type_name = search_name(LteRrcOtaPduType_v15, ARRAY_SIZE(LteRrcOtaPduType_v15, ValueName), pdu_number_to_search);
    }

    std::string msg_hex = bytes_to_hex_string(b + offset, pdu_length);
    j["Msg_RAW_Hex"] = msg_hex;

    if (type_name == nullptr) {
        LOGD("(MI)Unknown LTE RRC PDU Type: 0x%x (search key: %d) for version %d", pdu_number_raw, pdu_number_to_search, pkt_ver);
        j["Msg_Type"] = "raw_msg/UNKNOWN_PDU_" + std::to_string(pdu_number_raw);
        j["Msg_Dissected"] = "(Cannot dissect unknown PDU type)";
    } else {
        j["Msg_Type"] = type_name;
        if (!epan_initialized) {
            LOGD("Initializing epan for RRC OTA dissection");
            initialize_epan();
            epan_initialized = true;
            LOGD("epan initialized");
        }
        std::string ws_result = decode_msg(type_name, msg_hex);
        if (ws_result.rfind("Error", 0) == 0 || ws_result.rfind("Unknown", 0) == 0 || ws_result.rfind("character", 0) != std::string::npos) {
            LOGD("Wireshark dissection failed for %s: %s", type_name, ws_result.c_str());
            j["Msg_Dissected"] = "(Dissection failed: " + ws_result + ")";
        } else {
            try {
                j["Msg_Dissected"] = json::parse(ws_result);
            } catch (const json::parse_error& e) {
                j["Msg_Dissected"] = ws_result;
            }
        }
    }

    offset += pdu_length;
    return offset - start_payload;
}