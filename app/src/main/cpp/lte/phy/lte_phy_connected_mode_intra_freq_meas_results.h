//
// Created by Vladimir Ponomarenko on 13.04.2025.
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



const Fmt LtePhyCmlifmrFmt[] = {
        {UINT, "Version", 1},
        {SKIP, NULL, 3},
        {UINT, "Serving Cell Index", 1},
        {SKIP, NULL, 3},
};


const Fmt LtePhyCmlifmrFmt_v3_Header[] = {
        {UINT, "E-ARFCN", 2},
        {UINT, "Serving Physical Cell ID", 2},
        {UINT, "Sub-frame Number", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 2},
        {UINT, "Number of Neighbor Cells", 1},
        {UINT, "Number of Detected Cells", 1},
};

const Fmt LtePhyCmlifmrFmt_v3_Neighbor_Cell[] = {
        {UINT, "Physical Cell ID", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 4},
};

const Fmt LtePhyCmlifmrFmt_v3_Detected_Cell[] = {
        {UINT, "Physical Cell ID", 4},
        {UINT, "SSS Corr Value", 4},
        {UINT, "Reference Time", 8},
};


const Fmt LtePhyCmlifmrFmt_v4_Header[] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Serving Physical Cell ID", 2},
        {UINT, "Sub-frame Number", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 2},
        {UINT, "Number of Neighbor Cells", 1},
        {UINT, "Number of Detected Cells", 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyCmlifmrFmt_v4_Neighbor_Cell[] = {
        {UINT, "Physical Cell ID", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 4},
};

const Fmt LtePhyCmlifmrFmt_v4_Detected_Cell[] = {
        {UINT, "Physical Cell ID", 2},
        {SKIP, NULL, 2},
        {UINT, "SSS Corr Value", 4},
        {UINT, "Reference Time", 8},
};







static int _decode_lte_phy_cmlifmr(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];


    j["Serving Cell Index"] = j["Serving Cell Index"].get<unsigned int>() & 7;
    const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), j["Serving Cell Index"]);
    j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");

   // LOGD("Decoding LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 3: {
            offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Header,
                                     ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Header, Fmt),
                                     b, offset, length, j);

            int n_neighbor_cells = j["Number of Neighbor Cells"];
            int n_detected_cells = j["Number of Detected Cells"];

            json neighbor_cells_j = json::array();
            for (int i = 0; i < n_neighbor_cells; ++i) {
                json cell_j;
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Neighbor_Cell,
                                         ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Neighbor_Cell, Fmt),
                                         b, offset, length, cell_j);
                neighbor_cells_j.push_back(cell_j);
            }
            j["Neighbor Cells"] = neighbor_cells_j;

            json detected_cells_j = json::array();
            for (int i = 0; i < n_detected_cells; ++i) {
                json cell_j;
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Detected_Cell,
                                         ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Detected_Cell, Fmt),
                                         b, offset, length, cell_j);
                detected_cells_j.push_back(cell_j);
            }
            j["Detected Cells"] = detected_cells_j;

            return offset - start;
        }
        case 4: {
            offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Header,
                                     ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Header, Fmt),
                                     b, offset, length, j);

            int n_neighbor_cells = j["Number of Neighbor Cells"];
            int n_detected_cells = j["Number of Detected Cells"];

            json neighbor_cells_j = json::array();
            for (int i = 0; i < n_neighbor_cells; ++i) {
                json cell_j;
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Neighbor_Cell,
                                         ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Neighbor_Cell, Fmt),
                                         b, offset, length, cell_j);
                neighbor_cells_j.push_back(cell_j);
            }
            j["Neighbor Cells"] = neighbor_cells_j;

            json detected_cells_j = json::array();
            for (int i = 0; i < n_detected_cells; ++i) {
                json cell_j;
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Detected_Cell,
                                         ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Detected_Cell, Fmt),
                                         b, offset, length, cell_j);
                detected_cells_j.push_back(cell_j);
            }
            j["Detected Cells"] = detected_cells_j;

            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY CMLIFMR version: 0x%x", pkt_ver);
            return 0;
    }
}