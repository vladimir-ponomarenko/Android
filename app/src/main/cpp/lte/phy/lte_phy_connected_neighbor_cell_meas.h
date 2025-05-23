//
// Created by Vladimir Ponomarenko on 06.04.2025.
//

#ifndef LOGIN_LTE_PHY_CONNECTED_NEIGHBOR_CELL_MEAS_H
#define LOGIN_LTE_PHY_CONNECTED_NEIGHBOR_CELL_MEAS_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;



const Fmt LtePhyCncm_Fmt[] = {
        {UINT, "Version", 1},
        {UINT, "Number of SubPackets", 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyCncm_Subpacket_Header_v1[] = {
        {UINT, "SubPacket ID", 1},
        {UINT, "Version", 1},
        {UINT, "SubPacket Size", 2},
};


const Fmt LtePhyCncm_Subpacket_Payload_31v3[] = {
        {UINT, "E-ARFCN", 2},
        {UINT, "Num Cells", 2},
        {PLACEHOLDER, "Deplexing Mode", 0},
        {PLACEHOLDER, "Serving Cell Index", 0},
};
const Fmt LtePhyCncm_Subpacket_31v3_cell[] = {
        {UINT, "Physical Cell ID", 4},
        {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRP", 2},
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRQ", 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSSI", 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
};


const Fmt LtePhyCncm_Subpacket_Payload_31v4[] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num Cells", 2},
        {PLACEHOLDER, "Deplexing Mode", 0},
        {PLACEHOLDER, "Serving Cell Index", 0},
        {SKIP, NULL, 2},
};
const Fmt LtePhyCncm_Subpacket_31v4_cell[] = {
        {UINT, "Physical Cell ID", 4},
        {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRP", 2},
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRQ", 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSSI", 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
};



const Fmt LtePhyCncm_Subpacket_Payload_31v24[] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num Cells", 2},
        {PLACEHOLDER, "Deplexing Mode", 0},
        {PLACEHOLDER, "Serving Cell Index", 0},
        {SKIP, NULL, 2},
};
const Fmt LtePhyCncm_Subpacket_31v24_cell[] = {
        {UINT, "Physical Cell ID", 4},
        {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRP", 2},
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSRQ", 4},
        {SKIP, NULL, 4},
        {UINT, "Inst Measured RSSI", 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
};




static int _decode_lte_phy_connected_neighbor_cell_meas_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];
    int n_subpkts = j["Number of SubPackets"];

   // LOGD("Decoding LTE_PHY_Connected_Mode_Neighbor_Measurement with version: %d, num_subpkts: %d", pkt_ver, n_subpkts);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkts; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                offset += _decode_by_fmt(LtePhyCncm_Subpacket_Header_v1,
                                         ARRAY_SIZE(LtePhyCncm_Subpacket_Header_v1, Fmt),
                                         b, offset, length, subpkt_j);

                int subpkt_id = subpkt_j["SubPacket ID"];
                int subpkt_ver = subpkt_j["Version"];
                int subpkt_size = subpkt_j["SubPacket Size"];

                if (subpkt_id == 30) {
                   // LOGD("LTE PHY CNCM: Skipping Subpacket ID 30 (Request)");

                } else if (subpkt_id == 31) {
                    if (subpkt_ver == 4) {
                       // LOGD("LTE PHY CNCM: Decoding Subpacket ID 31, Version 4");
                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_Payload_31v4,
                                                 ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v4, Fmt),
                                                 b, offset, length, subpkt_j);

                        int temp_nc = subpkt_j["Num Cells"];
                        int num_cells = temp_nc & 63;
                        int duplexingMode = (temp_nc >> 7) & 3;
                        int servingCellIndx = (temp_nc >> 9) & 15;

                        subpkt_j["Num Cells"] = num_cells;
                        subpkt_j["Deplexing Mode"] = duplexingMode;
                        const char* duplex_name = search_name(ValueNameDuplexingMode, ARRAY_SIZE(ValueNameDuplexingMode, ValueName), duplexingMode);
                        subpkt_j["Deplexing Mode"] = (duplex_name ? duplex_name : "(MI)Unknown");

                        subpkt_j["Serving Cell Index"] = servingCellIndx;
                        const char* cellidx_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), servingCellIndx);
                        subpkt_j["Serving Cell Index"] = (cellidx_name ? cellidx_name : "(MI)Unknown");


                        json cells_j = json::array();
                        for (int cell_idx = 0; cell_idx < num_cells; ++cell_idx) {
                            json cell_item_j;
                            offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v4_cell,
                                                     ARRAY_SIZE(LtePhyCncm_Subpacket_31v4_cell, Fmt),
                                                     b, offset, length, cell_item_j);

                            unsigned int utemp_pci = cell_item_j["Physical Cell ID"];
                            cell_item_j["Physical Cell ID"] = utemp_pci & 1023;
                            cell_item_j["FTL Cumulative Freq Offset"] = (utemp_pci >> 10) & 65535;

                            int temp_rsrp = cell_item_j["Inst Measured RSRP"];
                            cell_item_j["Inst Measured RSRP"] = (static_cast<double>(temp_rsrp & 4095) * 0.0625) - 180.0;

                            unsigned int utemp_rsrq = cell_item_j["Inst Measured RSRQ"];
                            cell_item_j["Inst Measured RSRQ"] = (static_cast<double>((utemp_rsrq >> 10) & 1023) * 0.0625) - 30.0;

                            unsigned int utemp_rssi = cell_item_j["Inst Measured RSSI"];
                            cell_item_j["Inst Measured RSSI"] = (static_cast<double>((utemp_rssi >> 11) & 2047) * 0.0625) - 110.0;

                            cells_j.push_back(cell_item_j);
                        }
                        subpkt_j["Neighbor Cells"] = cells_j;

                    } else if (subpkt_ver == 3) {
                        //LOGD("LTE PHY CNCM: Decoding Subpacket ID 31, Version 3");
                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_Payload_31v3,
                                                 ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v3, Fmt),
                                                 b, offset, length, subpkt_j);

                        int temp_nc = subpkt_j["Num Cells"];
                        int num_cells = temp_nc & 63;
                        int duplexingMode = (temp_nc >> 7) & 3;
                        int servingCellIndx = (temp_nc >> 9) & 15;

                        subpkt_j["Num Cells"] = num_cells;
                        subpkt_j["Deplexing Mode"] = duplexingMode;
                        const char* duplex_name = search_name(ValueNameDuplexingMode, ARRAY_SIZE(ValueNameDuplexingMode, ValueName), duplexingMode);
                        subpkt_j["Deplexing Mode"] = (duplex_name ? duplex_name : "(MI)Unknown");

                        subpkt_j["Serving Cell Index"] = servingCellIndx;
                        const char* cellidx_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), servingCellIndx);
                        subpkt_j["Serving Cell Index"] = (cellidx_name ? cellidx_name : "(MI)Unknown");


                        json cells_j = json::array();
                        for (int cell_idx = 0; cell_idx < num_cells; ++cell_idx) {
                            json cell_item_j;
                            offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v3_cell,
                                                     ARRAY_SIZE(LtePhyCncm_Subpacket_31v3_cell, Fmt),
                                                     b, offset, length, cell_item_j);

                            unsigned int utemp_pci = cell_item_j["Physical Cell ID"];
                            cell_item_j["Physical Cell ID"] = utemp_pci & 1023;
                            cell_item_j["FTL Cumulative Freq Offset"] = (utemp_pci >> 10) & 65535;

                            int temp_rsrp = cell_item_j["Inst Measured RSRP"];
                            cell_item_j["Inst Measured RSRP"] = (static_cast<double>(temp_rsrp & 4095) * 0.0625) - 180.0;

                            unsigned int utemp_rsrq = cell_item_j["Inst Measured RSRQ"];
                            cell_item_j["Inst Measured RSRQ"] = (static_cast<double>((utemp_rsrq >> 10) & 1023) * 0.0625) - 30.0;

                            unsigned int utemp_rssi = cell_item_j["Inst Measured RSSI"];
                            cell_item_j["Inst Measured RSSI"] = (static_cast<double>((utemp_rssi >> 11) & 2047) * 0.0625) - 110.0;

                            cells_j.push_back(cell_item_j);
                        }
                        subpkt_j["Neighbor Cells"] = cells_j;

                    } else if (subpkt_ver == 24) {
                      //  LOGD("LTE PHY CNCM: Decoding Subpacket ID 31, Version 24 (using v4 logic)");

                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_Payload_31v24,
                                                 ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v24, Fmt),
                                                 b, offset, length, subpkt_j);

                        int temp_nc = subpkt_j["Num Cells"];
                        int num_cells = temp_nc & 63;
                        int duplexingMode = (temp_nc >> 7) & 3;
                        int servingCellIndx = (temp_nc >> 9) & 15;

                        subpkt_j["Num Cells"] = num_cells;
                        subpkt_j["Deplexing Mode"] = duplexingMode;
                        const char* duplex_name = search_name(ValueNameDuplexingMode, ARRAY_SIZE(ValueNameDuplexingMode, ValueName), duplexingMode);
                        subpkt_j["Deplexing Mode"] = (duplex_name ? duplex_name : "(MI)Unknown");

                        subpkt_j["Serving Cell Index"] = servingCellIndx;
                        const char* cellidx_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), servingCellIndx);
                        subpkt_j["Serving Cell Index"] = (cellidx_name ? cellidx_name : "(MI)Unknown");


                        json cells_j = json::array();
                        for (int cell_idx = 0; cell_idx < num_cells; ++cell_idx) {
                            json cell_item_j;
                            offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v24_cell,
                                                     ARRAY_SIZE(LtePhyCncm_Subpacket_31v24_cell, Fmt),
                                                     b, offset, length, cell_item_j);

                            unsigned int utemp_pci = cell_item_j["Physical Cell ID"];
                            cell_item_j["Physical Cell ID"] = utemp_pci & 1023;
                            cell_item_j["FTL Cumulative Freq Offset"] = (utemp_pci >> 10) & 65535;

                            int temp_rsrp = cell_item_j["Inst Measured RSRP"];
                            cell_item_j["Inst Measured RSRP"] = (static_cast<double>(temp_rsrp & 4095) * 0.0625) - 180.0;

                            unsigned int utemp_rsrq = cell_item_j["Inst Measured RSRQ"];
                            cell_item_j["Inst Measured RSRQ"] = (static_cast<double>((utemp_rsrq >> 10) & 1023) * 0.0625) - 30.0;

                            unsigned int utemp_rssi = cell_item_j["Inst Measured RSSI"];
                            cell_item_j["Inst Measured RSSI"] = (static_cast<double>((utemp_rssi >> 11) & 2047) * 0.0625) - 110.0;

                            cells_j.push_back(cell_item_j);
                        }
                        subpkt_j["Neighbor Cells"] = cells_j;

                    } else {
                        LOGD("(MI)Unknown LTE PHY Connected Neighbor Cell Meas subpkt id 31 version: %d", subpkt_ver);

                        offset = start_subpkt + subpkt_size;
                        subpkt_j["error"] = "Unknown Subpacket 31 Version";
                    }
                } else {
                    LOGD("(MI)Unknown LTE PHY Connected Neighbor Cell Meas subpkt id: %d", subpkt_id);

                    offset = start_subpkt + subpkt_size;
                    subpkt_j["error"] = "Unknown Subpacket ID";
                }


                if (offset < start_subpkt + subpkt_size) {
                   // LOGD("LTE PHY CNCM: Advancing offset for subpacket padding/remaining data. Current: %d, Expected end: %d", offset, start_subpkt + subpkt_size);
                    offset = start_subpkt + subpkt_size;
                } else if (offset > start_subpkt + subpkt_size) {
                    //LOGD("LTE PHY CNCM: WARNING - Offset (%d) exceeded subpacket size (%d) for subpacket ID %d, Version %d.", offset, subpkt_size, subpkt_id, subpkt_ver);

                }

                subpackets_j.push_back(subpkt_j);
            }
            j["SubPackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY Connected Neighbor Cell Meas version: %d", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_CONNECTED_NEIGHBOR_CELL_MEAS_H
