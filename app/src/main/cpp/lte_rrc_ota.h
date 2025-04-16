//
// Created by Vladimir Ponomarenko on 13.04.2025.
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

using json = nlohmann::json;
using namespace std;

static int _decode_lte_rrc_ota(const char* b, int offset, size_t length, json& j) {
    int start_payload = offset;
    int pkt_ver = j["Pkt Version"];

    LOGD("Decoding LTE_RRC_OTA_Packet with version: %d", pkt_ver);


    switch (pkt_ver) {
        case 2:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v2, ARRAY_SIZE(LteRrcOtaPacketFmt_v2, Fmt), b, offset, length, j);
            break;
        case 4:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v4, ARRAY_SIZE(LteRrcOtaPacketFmt_v4, Fmt), b, offset, length, j);
            break;
        case 7:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v7, ARRAY_SIZE(LteRrcOtaPacketFmt_v7, Fmt), b, offset, length, j);
            break;
        case 8:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v8, ARRAY_SIZE(LteRrcOtaPacketFmt_v8, Fmt), b, offset, length, j);
            break;
        case 9:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v9, ARRAY_SIZE(LteRrcOtaPacketFmt_v9, Fmt), b, offset, length, j);
            break;
        case 12:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v12, ARRAY_SIZE(LteRrcOtaPacketFmt_v12, Fmt), b, offset, length, j);
            break;
        case 13:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v13, ARRAY_SIZE(LteRrcOtaPacketFmt_v13, Fmt), b, offset, length, j);
            break;
        case 15:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v15, ARRAY_SIZE(LteRrcOtaPacketFmt_v15, Fmt), b, offset, length, j);
            break;
        case 19:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v19, ARRAY_SIZE(LteRrcOtaPacketFmt_v19, Fmt), b, offset, length, j);
            break;
        case 20:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v20, ARRAY_SIZE(LteRrcOtaPacketFmt_v20, Fmt), b, offset, length, j);
            break;
        case 24:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v24, ARRAY_SIZE(LteRrcOtaPacketFmt_v24, Fmt), b, offset, length, j);
            break;
        case 26:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt_v26, ARRAY_SIZE(LteRrcOtaPacketFmt_v26, Fmt), b, offset, length, j);
            break;
        default:
            LOGD("(MI)Unknown LTE RRC OTA packet version: %d", pkt_ver);
            j["error"] = "Unknown Version";
            return offset - start_payload;
    }



    if (j.count("PDU Number") == 0 || j.count("Msg Length") == 0) {
        LOGD("(MI)LTE RRC OTA missing PDU Number or Msg Length for version %d", pkt_ver);
        j["error"] = "Missing PDU Number or Msg Length";
        return offset - start_payload;
    }


    int pdu_number = j["PDU Number"];
    int pdu_length = j["Msg Length"];

    const char* type_name = nullptr;
    string type_str_prefix = "raw_msg/";


    if (pkt_ver == 19 || pkt_ver == 26) {
        type_name = search_name(LteRrcOtaPduType_v19, ARRAY_SIZE(LteRrcOtaPduType_v19, ValueName), pdu_number);
    } else if (pkt_ver >= 15) {
        type_name = search_name(LteRrcOtaPduType_v15, ARRAY_SIZE(LteRrcOtaPduType_v15, ValueName), pdu_number);
    } else {

        if (pdu_number > 8) {
            pdu_number -= 7;
        }
        type_name = search_name(LteRrcOtaPduType, ARRAY_SIZE(LteRrcOtaPduType, ValueName), pdu_number);
    }

    if (type_name == nullptr) {
        LOGD("(MI)Unknown LTE RRC PDU Type: 0x%x for version %d", pdu_number, pkt_ver);
        j["error"] = "Unknown PDU Type";




        type_str_prefix += "UNKNOWN_PDU_" + std::to_string(pdu_number);
        j["Msg_Type"] = type_str_prefix;
        j["Msg"] = bytes_to_hex_string(b + offset, pdu_length);
        offset += pdu_length;
        return offset - start_payload;

    } else {
        j["Msg_Type"] = type_str_prefix + type_name;

        j["Msg"] = bytes_to_hex_string(b + offset, pdu_length);
        offset += pdu_length;
        return offset - start_payload;
    }
}