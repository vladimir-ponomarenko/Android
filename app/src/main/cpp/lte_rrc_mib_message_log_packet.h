//
// Created by User on 23.03.2025.
//

#pragma once

#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include <android/log.h>
#include "utils.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const Fmt LteRrcMibMessageLogPacketFmt[] = {
        {UINT, "Version", 1}
};

const ValueName LteRrcMibMessageLogPacketFmt_OpModeType[] = {
        {0,  "inband-DifferentPCI"},
        {1,  "inband-SamePCI"},
        {2,  "GUARDBAND"},
        {3,  "STANDALONE"}
};

const ValueName LteRrcMibMessageLogPacketFmt_RasterOffset[] = {
        {0,  "KHZ-7DOT5"},
        {1,  "KHZ-2DOT5"},
        {2,  "KHZ2DOT5"},
        {3,  "KHZ7DOT5"}
};

const Fmt LteRrcMibMessageLogPacketFmt_v1[] = {
        {UINT,      "Physical Cell ID",  2},    // cell ID
        {UINT,      "Freq",              2},    // frequency
        {UINT,      "SFN",               2},
        {UINT,      "Number of Antenna", 1},
        {BANDWIDTH, "DL BW",             1}     // downlink bandwidth
};

const Fmt LteRrcMibMessageLogPacketFmt_v2[] = {
        {UINT,      "Physical Cell ID",  2},    // cell ID
        {UINT,      "Freq",              4},    // frequency
        {UINT,      "SFN",               2},
        {UINT,      "Number of Antenna", 1},
        {BANDWIDTH, "DL BW",             1}     // downlink bandwidth
};

const Fmt LteRrcMibMessageLogPacketFmt_v3[] = {
        {UINT,      "Physical Cell ID",  2},    // cell ID
        {UINT,      "Freq",              4},    // frequency
        {UINT,      "SFN",               2},
        {UINT,      "Number of Antenna", 1},
        {BANDWIDTH, "DL BW",             1},    // downlink bandwidth
        {UINT,      "Sib1 BR Sch Info",  1}
};

const Fmt LteRrcMibMessageLogPacketFmt_v17[] = {
        {UINT,      "Physical Cell ID",  2},    // cell ID
        {UINT,      "Freq",              4},    // frequency
        {UINT,      "SFN",               2},
        {UINT,      "SFN MSB4",          1},
        {UINT,      "HSFN LSB2",         1},
        {UINT,      "Sib1 Sch Info",     1},
        {UINT,      "Sys Info Value Tag", 1},
        {UINT,      "Access Barring Enabled", 1},
        {UINT,      "Op Mode Type",      1},
        {UINT,      "Raster Offset",     2},
        {UINT,      "Number of Antenna", 1}
};

static int _decode_lte_rrc_mib(const char *b, int offset, size_t length, json &j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("(MI)Decoding LTE RRC MIB with version: 0x%x", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            LOGD("(MI)Decoding LTE RRC MIB version 1");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v1,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v1, Fmt),
                                     b, offset, length, j);

            // Обработка полей для версии 1
            int phy_cell_id = j["Physical Cell ID"];
            LOGD("(MI)Raw Physical Cell ID: %d", phy_cell_id);
            j["Physical Cell ID"] = phy_cell_id & 511; // 9 bits

            int freq = j["Freq"];
            LOGD("(MI)Raw Frequency: %d", freq);
            j["Freq"] = freq & 4095; // 12 bits

            int sfn = j["SFN"];
            LOGD("(MI)Raw SFN: %d", sfn);
            j["SFN"] = sfn & 1023; // 10 bits

            break;
        }
        case 2: {
            LOGD("(MI)Decoding LTE RRC MIB version 2");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v2,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v2, Fmt),
                                     b, offset, length, j);

            // Обработка полей для версии 2
            int phy_cell_id = j["Physical Cell ID"];
            LOGD("(MI)Raw Physical Cell ID: %d", phy_cell_id);
            j["Physical Cell ID"] = phy_cell_id & 511; // 9 bits

            int freq = j["Freq"];
            LOGD("(MI)Raw Frequency: %d", freq);
            j["Freq"] = freq; // 32 bits

            int sfn = j["SFN"];
            LOGD("(MI)Raw SFN: %d", sfn);
            j["SFN"] = sfn & 1023; // 10 bits

            break;
        }
        case 3: {
            LOGD("(MI)Decoding LTE RRC MIB version 3");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v3,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v3, Fmt),
                                     b, offset, length, j);

            // Обработка полей для версии 3
            int phy_cell_id = j["Physical Cell ID"];
            LOGD("(MI)Raw Physical Cell ID: %d", phy_cell_id);
            j["Physical Cell ID"] = phy_cell_id & 511; // 9 bits

            int freq = j["Freq"];
            LOGD("(MI)Raw Frequency: %d", freq);
            j["Freq"] = freq; // 32 bits

            int sfn = j["SFN"];
            LOGD("(MI)Raw SFN: %d", sfn);
            j["SFN"] = sfn & 1023; // 10 bits

            int sib1_br_sch_info = j["Sib1 BR Sch Info"];
            LOGD("(MI)Raw Sib1 BR Sch Info: %d", sib1_br_sch_info);
            j["Sib1 BR Sch Info"] = sib1_br_sch_info & 1; // 1 bit

            break;
        }
        case 17: {
            LOGD("(MI)Decoding LTE RRC MIB version 17");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v17,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v17, Fmt),
                                     b, offset, length, j);

            int phy_cell_id = j["Physical Cell ID"];
            LOGD("(MI)Raw Physical Cell ID: %d", phy_cell_id);
            j["Physical Cell ID"] = phy_cell_id & 511; // 9 bits

            int freq = j["Freq"];
            LOGD("(MI)Raw Frequency: %d", freq);
            j["Freq"] = freq; // 32 bits

            int sfn = j["SFN"];
            LOGD("(MI)Raw SFN: %d", sfn);
            j["SFN"] = sfn & 1023; // 10 bits

            int sfn_msb4 = j["SFN MSB4"];
            LOGD("(MI)Raw SFN MSB4: %d", sfn_msb4);
            j["SFN MSB4"] = sfn_msb4 & 15; // 4 bits

            int hsfm_lsb2 = j["HSFN LSB2"];
            LOGD("(MI)Raw HSFN LSB2: %d", hsfm_lsb2);
            j["HSFN LSB2"] = hsfm_lsb2 & 3; // 2 bits

            const char* op_mode_name = search_name(
                    LteRrcMibMessageLogPacketFmt_OpModeType,
                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_OpModeType, ValueName),
                    j["Op Mode Type"]
            );
            LOGD("(MI)Op Mode Type: %s", op_mode_name ? op_mode_name : "(MI)Unknown");
            j["Op Mode Type"] = (op_mode_name != NULL) ? op_mode_name : "(MI)Unknown";

            const char* raster_offset_name = search_name(
                    LteRrcMibMessageLogPacketFmt_RasterOffset,
                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_RasterOffset, ValueName),
                    j["Raster Offset"]
            );
            LOGD("(MI)Raster Offset: %s", raster_offset_name ? raster_offset_name : "(MI)Unknown");
            j["Raster Offset"] = (raster_offset_name != NULL) ? raster_offset_name : "(MI)Unknown";

            break;
        }
        default:
            LOGD("(MI)Unknown LTE RRC MIB version: 0x%x", pkt_ver);
            return 0;
    }

    LOGD("(MI)Decoded LTE RRC MIB successfully");
    return offset - start;
}

