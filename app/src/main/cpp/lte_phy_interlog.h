//
// Created by Vladimir Ponomarenko on 02.04.2025.
//

#pragma once

#include <vector>
#include <string>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

static int _decode_lte_phy_interlog(const char *b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    switch (pkt_ver) {
        case 2: {
            offset += _decode_by_fmt(LtePhyInterlogFmt_v2_Header,
                                     ARRAY_SIZE(LtePhyInterlogFmt_v2_Header, Fmt),
                                     b, offset, length, j);

            int n_neighbor_cells = 0;
            if (j.find("Number of Neighbor Cells") != j.end() && j["Number of Neighbor Cells"].is_number()) {
                n_neighbor_cells = j["Number of Neighbor Cells"].get<int>();
            } else {
                LOGD("LTE PHY InterLog v2: 'Number of Neighbor Cells' not found or not a number.");
            }


            json neighbor_cells_j = json::array();
            for (int i = 0; i < n_neighbor_cells; i++) {
                json cell_j;
                offset += _decode_by_fmt(LtePhyInterlogFmt_v2_Neighbor_Cell,
                                         ARRAY_SIZE(LtePhyInterlogFmt_v2_Neighbor_Cell, Fmt),
                                         b, offset, length, cell_j);
                neighbor_cells_j.push_back(cell_j);
            }
            j["Neighbor Cells"] = neighbor_cells_j;

            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY InterLog version: 0x%x", pkt_ver);
            return 0;
    }
}

