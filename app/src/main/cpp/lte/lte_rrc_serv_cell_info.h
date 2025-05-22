//
// Created by Vladimir Ponomarenko on 06.04.2025.
//

#ifndef LOGIN_LTE_RRC_SERV_CELL_INFO_H
#define LOGIN_LTE_RRC_SERV_CELL_INFO_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_rrc_serv_cell_info(const char* b, int offset, size_t length, json& j) {
    int start = offset;


    if (j.find("Version") == j.end()) {
        LOGD("(MI)LTE RRC Serving Cell Info: Version field missing in JSON object.");
        return 0;
    }

    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_RRC_Serv_Cell_Info with version: %d", pkt_ver);

    int consumed = 0;
    switch (pkt_ver) {
        case 2:
            consumed = _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v2,
                                      ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v2, Fmt),
                                      b, offset, length, j);
            if (consumed < 0) {
                LOGD("(MI)LTE RRC Serving Cell Info v2: _decode_by_fmt failed.");
                return 0;
            }
            offset += consumed;
            break;
        case 3:
            consumed = _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v3,
                                      ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v3, Fmt),
                                      b, offset, length, j);
            if (consumed < 0) {
                LOGD("(MI)LTE RRC Serving Cell Info v3: _decode_by_fmt failed.");
                return 0;
            }
            offset += consumed;
            break;
        default:
            LOGD("(MI)Unknown LTE RRC Serving Cell Info packet version: %d", pkt_ver);
            j["error"] = "Unknown Version";
            return 0;
    }

    return offset - start;
}

#endif //LOGIN_LTE_RRC_SERV_CELL_INFO_H