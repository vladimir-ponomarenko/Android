//
// Created by Vladimir Ponomarenko on 09.10.2025.
//

//TODO: add dissector for hex payload

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

static int _decode_gsm_dsds_rr_signaling_msg_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;

    json& subpkt_j = j["payload"]["GsmDsdsRrSignalingMessage"];

    if (subpkt_j.find("Channel Type") != subpkt_j.end() && subpkt_j["Channel Type"].is_number()) {
        int temp = subpkt_j["Channel Type"];
        int iChannelType = temp & 127;
        int iDirection = (temp >> 7) & 1;

        subpkt_j["Channel Type"] = iChannelType;
        const char* channel_name = search_name(ValueNameGSMChannelType, ARRAY_SIZE(ValueNameGSMChannelType, ValueName), iChannelType);
        subpkt_j["Channel Type"] = (channel_name ? channel_name : "(MI)Unknown");

        subpkt_j["Direction"] = iDirection;
        const char* direction_name = search_name(ValueNameDirection, ARRAY_SIZE(ValueNameDirection, ValueName), iDirection);
        subpkt_j["Direction"] = (direction_name ? direction_name : "(MI)Unknown");
    }

    if (subpkt_j.find("Message Type") != subpkt_j.end() && subpkt_j["Message Type"].is_number()) {
        int iMessageType = subpkt_j["Message Type"];
        const char* msg_type_name = search_name(ValueNameGSMSignalingMessageType, ARRAY_SIZE(ValueNameGSMSignalingMessageType, ValueName), iMessageType);
        subpkt_j["Message Type"] = (msg_type_name ? msg_type_name : "(MI)Unknown");
    }

    if (subpkt_j.find("Message Length in bytes") != subpkt_j.end() && subpkt_j["Message Length in bytes"].is_number()) {
        int msg_len = subpkt_j["Message Length in bytes"].get<int>();
        if (msg_len > 0) {
            if (offset + msg_len <= length) {
                subpkt_j["Message Payload Hex"] = bytes_to_hex_string(b + offset, msg_len);
                offset += msg_len;
            } else {
                LOGD("GSM_DSDS_RR_Signaling_Message: Message Length (%d) exceeds available buffer (%d)", msg_len, (int)(length - offset));
                subpkt_j["error"] = "Invalid Message Length";
            }
        }
    }

    return offset - start;
}