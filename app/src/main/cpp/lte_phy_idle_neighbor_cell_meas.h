//
// Created by User on 25.03.2025.
//

#ifndef LOGIN_LTE_PHY_IDLE_NEIGHBOR_CELL_MEAS_H
#define LOGIN_LTE_PHY_IDLE_NEIGHBOR_CELL_MEAS_H

#include "consts.h"
#include "log_packet_utils.h"
#include "log_packet.h"
#include "nlohmann/json.hpp"
#include <android/log.h>

using json = nlohmann::json;
using namespace std;


const Fmt LtePhyIncm_Fmt [] = {
        {UINT, "Version", 1},
        {UINT, "Number of SubPackets", 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyIncm_Subpacket_Header_v1 [] = {
        {UINT, "SubPacket ID", 1},
        {UINT, "Version", 1},
        {UINT, "SubPacket Size", 2},
};

const Fmt LtePhyIncm_Subpacket_Payload_26v1 [] = {
        {UINT, "E-ARFCN", 2},
        {UINT, "Num Cells", 1}, // 4 bits
        {PLACEHOLDER, "Num Rx Ant", 0}, // 2 bits
        {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
        {SKIP, NULL, 1},
};

const Fmt LtePhyIncm_Subpacket_Payload_26v2 [] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num Cells", 1}, // 4 bits
        {PLACEHOLDER, "Num Rx Ant", 0}, // 2 bits
        {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
        {SKIP, NULL, 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyIncm_Subpacket_26v1_cell [] = {
        {UINT, "Cell ID", 2},   // 10 bits
        {PLACEHOLDER, "CP Type", 0},    // 1 bit
        {PLACEHOLDER, "Enabled Tx Antennas", 0},    // 2 bits
        {PLACEHOLDER, "TTL Enabled", 0},    // 1 bit
        {PLACEHOLDER, "FTL Enabled", 0},    // 1 bit
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyIncm_Subpacket_Payload_27v2 [] = {
        {UINT, "E-ARFCN", 2},
        {UINT, "Num Cells", 2}, // 6 bits
        // skip 1 bit
        {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
};

const Fmt LtePhyIncm_Subpacket_Payload_27v4 [] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num Cells", 2}, // 6 bits
        // skip 1 bit
        {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
        {PLACEHOLDER, "Serving Cell Index", 0}, // 4 bits
        {SKIP, NULL, 2},
};

const Fmt LtePhyIncm_Subpacket_27v2_cell [] = {
        {UINT, "Physical Cell ID", 4},   // 10 bits
        {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},    // 16 bit
        {SKIP, NULL, 4},    // RSRP[0]
        {SKIP, NULL, 4},    // RSRP[1]
        {UINT, "Inst Measured RSRP", 2},    // 12 bits (0.0625 * x - 180) dBm
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},    // RSRQ[0] and RSRQ[1]
        {UINT, "Inst Measured RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dBm
        {SKIP, NULL, 4},    // RSRI[0]
        {UINT, "Inst Measured RSSI", 4},    // skip 11 bits, then 11 bits, (0.0625 * x - 110) dBm
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
        {SKIP, NULL, 4},
};

const ValueName ValueNameDuplexingMode[] = {
        {0, "FDD"},
        {1, "TDD"},
        {2, "HD-FDD"},
        {3, "Reserved"}
};

const ValueName ValueNameCPType[] = {
        {0, "Normal"},
        {1, "Extended"}
};

const ValueName ValueNameCellIndexx[] = {
        {0, "Cell 0"},
        {1, "Cell 1"},
        {2, "Cell 2"},
        {3, "Cell 3"},
        {4, "Cell 4"},
        {5, "Cell 5"},
        {6, "Cell 6"},
        {7, "Cell 7"},
};

static int _decode_lte_phy_idle_neighbor_cell_meas_payload(const char* b, int offset, size_t length, json& result) {
    int start = offset;
    int pkt_ver = result["Version"];
    int n_subpkts = result["Number of SubPackets"];

    LOGD("Decoding LTE_PHY_Idle_Neighbor_Cell_Meas with version: 0x%x", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            json all_subpkts_j = json::array();
            for (int i = 0; i < n_subpkts; i++) {
                json subpkt_j;
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePhyIncm_Subpacket_Header_v1,
                                         ARRAY_SIZE(LtePhyIncm_Subpacket_Header_v1, Fmt),
                                         b, offset, length, subpkt_j);

                int subpkt_id = subpkt_j["SubPacket ID"];
                int subpkt_ver = subpkt_j["Version"];
                int subpkt_size = subpkt_j["SubPacket Size"];

                if (subpkt_id == 26 && (subpkt_ver == 1 || subpkt_ver == 2)) {
                    // Idle mode neighbor cell measurement request v1 or v2
                    const Fmt* payload_fmt = (subpkt_ver == 1) ? LtePhyIncm_Subpacket_Payload_26v1 : LtePhyIncm_Subpacket_Payload_26v2;
                    size_t payload_fmt_size = (subpkt_ver == 1) ? ARRAY_SIZE(LtePhyIncm_Subpacket_Payload_26v1, Fmt) : ARRAY_SIZE(LtePhyIncm_Subpacket_Payload_26v2, Fmt);

                    offset += _decode_by_fmt(payload_fmt, payload_fmt_size, b, offset, length, subpkt_j);

                    int temp = subpkt_j["Num Cells"];
                    int num_cells = temp & 15;  // 4 bits
                    int num_rx_ant = (temp >> 4) & 3;    // 2 bits
                    int duplexing_mode = (temp >> 6) & 3;    // 2 bits

                    subpkt_j["Num Cells"] = num_cells;
                    subpkt_j["Num Rx Ant"] = num_rx_ant;
                    subpkt_j["Deplexing Mode"] = duplexing_mode;

                    const char* duplexing_mode_name = search_name(ValueNameDuplexingMode,
                                                                  ARRAY_SIZE(ValueNameDuplexingMode, ValueName), subpkt_j["Deplexing Mode"]);
                    subpkt_j["Deplexing Mode"] = (duplexing_mode_name != NULL) ? duplexing_mode_name : "(MI)Unknown";


                    json cells_j = json::array();
                    for (int j = 0; j < num_cells; j++) {
                        json cell_item_j;
                        offset += _decode_by_fmt(LtePhyIncm_Subpacket_26v1_cell,
                                                 ARRAY_SIZE(LtePhyIncm_Subpacket_26v1_cell, Fmt),
                                                 b, offset, length, cell_item_j);

                        int temp_val = cell_item_j["Cell ID"];
                        int i_cell_id = temp_val & 1023;  // 10 bits
                        int i_cp_type = (temp_val >> 10) & 1; // 1 bit
                        int i_enabled_tx_antennas = (temp_val >> 11) & 3;  // 2 bits
                        int i_ttl_enabled = (temp_val >> 13) & 1; // 1 bit
                        int i_ftl_enabled = (temp_val >> 14) & 1; // 1 bit

                        cell_item_j["Cell ID"] = i_cell_id;
                        cell_item_j["CP Type"] = i_cp_type;
                        cell_item_j["Enabled Tx Antennas"] = i_enabled_tx_antennas;
                        cell_item_j["TTL Enabled"] = i_ttl_enabled;
                        cell_item_j["FTL Enabled"] = i_ftl_enabled;

                        const char* cp_type_name = search_name(ValueNameCPType,
                                                               ARRAY_SIZE(ValueNameCPType, ValueName), cell_item_j["CP Type"]);
                        cell_item_j["CP Type"] = (cp_type_name != NULL) ? cp_type_name : "(MI)Unknown";


                        const char* ttl_enabled_name = search_name(ValueNameTrueOrFalse,
                                                                   ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), cell_item_j["TTL Enabled"]);
                        cell_item_j["TTL Enabled"] = (ttl_enabled_name != NULL) ? ttl_enabled_name : "(MI)Unknown";

                        const char* ftl_enabled_name = search_name(ValueNameTrueOrFalse,
                                                                   ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), cell_item_j["FTL Enabled"]);
                        cell_item_j["FTL Enabled"] = (ftl_enabled_name != NULL) ? ftl_enabled_name : "(MI)Unknown";

                        cells_j.push_back(cell_item_j);
                    }
                    subpkt_j["Neighbor Cells"] = cells_j;


                }
                else if (subpkt_id == 27 && (subpkt_ver == 2 || subpkt_ver == 4)) {
                    // Idle mode neighbor cell measurement result v2 or v4

                    const Fmt* payload_fmt = (subpkt_ver == 2) ? LtePhyIncm_Subpacket_Payload_27v2 : LtePhyIncm_Subpacket_Payload_27v4;
                    size_t payload_fmt_size = (subpkt_ver == 2) ? ARRAY_SIZE(LtePhyIncm_Subpacket_Payload_27v2, Fmt) : ARRAY_SIZE(LtePhyIncm_Subpacket_Payload_27v4, Fmt);


                    offset += _decode_by_fmt(payload_fmt, payload_fmt_size, b, offset, length, subpkt_j);

                    int temp = subpkt_j["Num Cells"];
                    int num_cells = temp & 63;  // 6 bits
                    int duplexing_mode = (temp >> 7) & 3;    // 2 bits, shift by 7, not 6
                    subpkt_j["Num Cells"] = num_cells;
                    subpkt_j["Deplexing Mode"] = duplexing_mode;

                    const char* duplexing_mode_name = search_name(ValueNameDuplexingMode,
                                                                  ARRAY_SIZE(ValueNameDuplexingMode, ValueName), subpkt_j["Deplexing Mode"]);
                    subpkt_j["Deplexing Mode"] = (duplexing_mode_name != NULL) ? duplexing_mode_name : "(MI)Unknown";


                    if (subpkt_ver == 4) {
                        int serving_cell_index = (temp >> 9) & 15;
                        subpkt_j["Serving Cell Index"] = serving_cell_index;

                        const char* serving_cell_index_name = search_name(ValueNameCellIndexx,
                                                                          ARRAY_SIZE(ValueNameCellIndexx, ValueName), subpkt_j["Serving Cell Index"]);
                        subpkt_j["Serving Cell Index"] = (serving_cell_index_name != NULL) ? serving_cell_index_name : "(MI)Unknown";
                    }


                    json cells_j = json::array();
                    for (int j = 0; j < num_cells; j++) {
                        json cell_item_j;
                        offset += _decode_by_fmt(LtePhyIncm_Subpacket_27v2_cell,
                                                 ARRAY_SIZE(LtePhyIncm_Subpacket_27v2_cell, Fmt),
                                                 b, offset, length, cell_item_j);

                        unsigned int utemp = cell_item_j["Physical Cell ID"];
                        int i_physical_cell_id = utemp & 1023;  // 10 bits
                        int i_ftl_cfo = (utemp >> 10) & 0xFFFF; // 16 bits

                        cell_item_j["Physical Cell ID"] = i_physical_cell_id;
                        cell_item_j["FTL Cumulative Freq Offset"] = i_ftl_cfo;

                        int inst_measured_rsrp = cell_item_j["Inst Measured RSRP"];
                        float rsrp = float(inst_measured_rsrp & 4095);
                        rsrp = rsrp * 0.0625 - 180.0;
                        cell_item_j["Inst Measured RSRP"] = rsrp;


                        utemp = cell_item_j["Inst Measured RSRQ"];
                        float rsrq = float((utemp >> 10) & 1023);
                        rsrq = rsrq * 0.0625 - 30.0;
                        cell_item_j["Inst Measured RSRQ"] = rsrq;


                        utemp = cell_item_j["Inst Measured RSSI"];
                        float rssi = float((utemp >> 11) & 2047);
                        rssi = rssi * 0.0625 - 110.0;
                        cell_item_j["Inst Measured RSSI"] = rssi;

                        cells_j.push_back(cell_item_j);
                    }
                    subpkt_j["Neighbor Cells"] = cells_j;

                }
                else {
                    LOGD("(MI)Unknown LTE PHY Idle Neighbor Cell Meas subpkt id and version: %d - %d\n", subpkt_id, subpkt_ver);
                }

                all_subpkts_j.push_back(subpkt_j);
                offset += subpkt_size - (offset - start_subpkt);
            }
            result["SubPackets"] = all_subpkts_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY Idle Neighbor Cell Meas version: 0x%x\n", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_IDLE_NEIGHBOR_CELL_MEAS_H
