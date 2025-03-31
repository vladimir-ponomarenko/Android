//
// Created by Vladimir Ponomarenko on 01.04.2025.
//

#ifndef LOGIN_LTE_PDSCH_STAT_INDICATION_H
#define LOGIN_LTE_PDSCH_STAT_INDICATION_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

const Fmt LtePdschStatIndication_Fmt[] = {
        {UINT, "Version", 1},
};

// --- Version 36 ---
const Fmt LtePdschStatIndication_Payload_v36[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v36_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {PLACEHOLDER, "HSIC Enabled", 0},
        {SKIP, NULL, 6},
};
const Fmt LtePdschStatIndication_Record_TB_v36[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {SKIP, NULL, 2},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {UINT, "QED2 Interim Status", 1},
        {PLACEHOLDER, "QED Iteration", 0},
        {SKIP, NULL, 2},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v36_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
        {SKIP, NULL, 2},
};

// --- Version 32 ---
const Fmt LtePdschStatIndication_Payload_v32[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v32_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {PLACEHOLDER, "HSIC Enabled", 0},
};
const Fmt LtePdschStatIndication_Record_TB_v32[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {SKIP, NULL, 1},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v32_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
};

// --- Version 24 ---
const Fmt LtePdschStatIndication_Payload_v24[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v24_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {PLACEHOLDER, "HSIC Enabled", 0},
};
const Fmt LtePdschStatIndication_Record_TB_v24[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {SKIP, NULL, 1},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v24_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
};

// --- Version 16 ---
const Fmt LtePdschStatIndication_Payload_v16[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v16_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {PLACEHOLDER, "HSIC Enabled", 0},
};
const Fmt LtePdschStatIndication_Record_TB_v16[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v16_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
};

// --- Version 5 ---
const Fmt LtePdschStatIndication_Payload_v5[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v5_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
};
const Fmt LtePdschStatIndication_Record_TB_v5[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {PLACEHOLDER, "Modulation Type", 0},
        {UINT, "Num RBs", 1},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v5_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
};

// --- Version 37 ---
const Fmt LtePdschStatIndication_Payload_v37[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v37_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {SKIP, NULL, 1},
        {UINT, "Alt TBS Enabled", 1},
        {SKIP, NULL, 4},
};
const Fmt LtePdschStatIndication_Record_TB_v37[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Discarded ReTx", 0},
        {UINT, "Did Recombining", 1},
        {SKIP, NULL, 1},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {UINT, "QED2 Interim Status", 1},
        {PLACEHOLDER, "QED Iteration", 0},
        {SKIP, NULL, 2},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v37_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
        {SKIP, NULL, 2},
};

// --- Version 40 ---
const Fmt LtePdschStatIndication_Payload_v40[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v40_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
        {SKIP, NULL, 1},
        {SKIP, NULL, 5},
};
const Fmt LtePdschStatIndication_Record_TB_v40[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 2},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded reTx Present", 0},
        {PLACEHOLDER, "Discarded ReTx", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {SKIP, NULL, 1},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {UINT, "QED2 Interim Status", 1},
        {PLACEHOLDER, "QED Iteration", 0},
        {SKIP, NULL, 2},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v40_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
        {SKIP, NULL, 2},
};

// --- Version 34 ---
const Fmt LtePdschStatIndication_Payload_v34[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};
const Fmt LtePdschStatIndication_Record_v34_P1[] = {
        {UINT, "Subframe Num", 2},
        {PLACEHOLDER, "Frame Num", 0},
        {UINT, "Num RBs", 1},
        {UINT, "Num Layers", 1},
        {UINT, "Num Transport Blocks Present", 1},
        {UINT, "Serving Cell Index", 1},
};
const Fmt LtePdschStatIndication_Record_TB_v34[] = {
        {UINT, "HARQ ID", 1},
        {PLACEHOLDER, "RV", 0},
        {PLACEHOLDER, "NDI", 0},
        {PLACEHOLDER, "CRC Result", 0},
        {UINT, "RNTI Type", 1},
        {PLACEHOLDER, "TB Index", 0},
        {PLACEHOLDER, "Discarded ReTx", 0},
        {PLACEHOLDER, "Did Recombining", 0},
        {UINT, "TB Size", 2},
        {UINT, "MCS", 1},
        {UINT, "Num RBs", 1},
        {UINT, "Modulation Type", 1},
        {UINT, "QED2 Interim Status", 1},
        {PLACEHOLDER, "QED Iteration", 0},
        {PLACEHOLDER, "ACK/NACK Decision", 0},
};
const Fmt LtePdschStatIndication_Record_v34_P2[] = {
        {UINT, "PMCH ID", 1},
        {UINT, "Area ID", 1},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation[] = {
        {0, "QPSK"},
        {1, "16QAM"},
        {2, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v16[] = {
        {2, "QPSK"},
        {4, "16QAM"},
        {6, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v24[] = {
        {2, "QPSK"},
        {4, "16QAM"},
        {6, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v32[] = {
        {2, "QPSK"},
        {4, "16QAM"},
        {6, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v36[] = {
        {2, "QPSK"},
        {4, "16QAM"},
        {6, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v34[] = {
        {2, "QPSK"},
        {4, "16QAM"},
        {6, "64QAM"},
        {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_CrcResult[] = {
        {0, "Fail"},
        {1, "Pass"},
};

const ValueName LtePdschStatIndication_Record_TB_DiscardedReTxPresent[] = {
        {0, "None"},
        {1, "Present"},
};

const ValueName LtePdschStatIndication_Record_TB_DidRecombining[] = {
        {0, "No"},
        {1, "Yes"},
};

const ValueName LtePdschStatIndication_Record_TB_Discarded_ReTx[] = {
        {0, "NO_DISCARD"},
        {1, "DISCARD_AND_ACK"},
};

const ValueName LtePdschStatIndication_Record_TB_AckNackDecision[] = {
        {0, "NACK"},
        {1, "ACK"},
};

const ValueName LtePdschStatIndication_Record_TB_v34_QED_Iteration[] = {
        {0, "OFF"},
};


static int _decode_lte_pdsch_stat_indication_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_PDSCH_Stat_Indication with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 36: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v36,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v36, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v36_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v36_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                int ihsic_enabled = (non_decode_p1_2 >> 3) & 15;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");

                record_item_j["HSIC Enabled"] = ihsic_enabled;
                const char* hsic_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), ihsic_enabled);
                record_item_j["HSIC Enabled"] = (hsic_name ? hsic_name : "(MI)Unknown");

                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v36,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v36, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");

                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;
                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");

                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");

                    // Modulation mapping
                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v36, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result; // Based on CRC result
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    // QED Status/Iteration
                    int qed_interim_status_raw = tb_item_j["QED2 Interim Status"];
                    int qed_iterations = (qed_interim_status_raw >> 2) & 63; // 6 bits
                    int qed_interim_status = qed_interim_status_raw & 3; // 2 bits
                    tb_item_j["QED2 Interim Status"] = qed_interim_status;
                    tb_item_j["QED Iteration"] = qed_iterations;

                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 12; // Size of one TB struct v36 is 12 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v36_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v36_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 32: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v32,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v32, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v32_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v32_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                int ihsic_enabled = (non_decode_p1_2 >> 3) & 15;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");

                record_item_j["HSIC Enabled"] = ihsic_enabled;
                const char* hsic_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), ihsic_enabled);
                record_item_j["HSIC Enabled"] = (hsic_name ? hsic_name : "(MI)Unknown");

                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v32,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v32, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");

                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;
                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");

                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");

                    // Modulation mapping
                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v32, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v32, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");

                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 8; // Size of one TB struct v32 is 8 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v32_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v32_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 24: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v24,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v24, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v24_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v24_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                int ihsic_enabled = (non_decode_p1_2 >> 3) & 15;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");

                record_item_j["HSIC Enabled"] = ihsic_enabled;
                const char* hsic_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), ihsic_enabled);
                record_item_j["HSIC Enabled"] = (hsic_name ? hsic_name : "(MI)Unknown");

                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v24,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v24, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;
                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");

                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");

                    // Modulation mapping
                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v24, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v24, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 8; // Size of one TB struct v24 is 8 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v24_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v24_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 16: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v16,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v16, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v16_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v16_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                int ihsic_enabled = (non_decode_p1_2 >> 3) & 15;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");

                record_item_j["HSIC Enabled"] = ihsic_enabled;
                const char* hsic_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), ihsic_enabled);
                record_item_j["HSIC Enabled"] = (hsic_name ? hsic_name : "(MI)Unknown");


                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v16,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v16, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;
                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");

                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");


                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 6; // Size of one TB struct v16 is 6 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v16_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v16_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 5: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v5,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v5, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v5_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v5_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                // v5 only has Serving Cell Index in this field
                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");


                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v5,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v5, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;
                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");


                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");


                    // Modulation derived from MCS for v5
                    int imcs = tb_item_j["MCS"];
                    int modulation_type_val = -1;
                    if (imcs < 10) {
                        modulation_type_val = 0;
                    } else if (imcs < 17) {
                        modulation_type_val = 1;
                    } else if (imcs < 29) {
                        modulation_type_val = 2;
                    }
                    tb_item_j["Modulation Type"] = modulation_type_val;
                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation, ValueName), modulation_type_val);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");


                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 6; // Size of one TB struct v5 is 6 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v5_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v5_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 37: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v37,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v37, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v37_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v37_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");


                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v37,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v37, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idiscarded_retx = (non_decode_p2_2 >> 6) & 3; // 2 bits for v37
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;

                    tb_item_j["Discarded ReTx"] = idiscarded_retx;
                    const char* discarded_retx_name = search_name(LtePdschStatIndication_Record_TB_Discarded_ReTx, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Discarded_ReTx, ValueName), idiscarded_retx);
                    tb_item_j["Discarded ReTx"] = (discarded_retx_name ? discarded_retx_name : "(MI)Unknown");


                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");

                    int idid_recombining = tb_item_j["Did Recombining"];
                    idid_recombining = idid_recombining & 1;
                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");


                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v36, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    // QED Status/Iteration
                    int qed_interim_status_raw = tb_item_j["QED2 Interim Status"];
                    int qed_iterations = (qed_interim_status_raw >> 2) & 63; // 6 bits
                    int qed_interim_status = qed_interim_status_raw & 3; // 2 bits
                    tb_item_j["QED2 Interim Status"] = qed_interim_status;
                    tb_item_j["QED Iteration"] = qed_iterations;


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 12; // Size of one TB struct v37 is 12 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v37_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v37_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 40: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v40,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v40, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v40_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v40_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");


                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v40,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v40, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_re_tx_present = (non_decode_p2_2 >> 5) & 1;
                    int idiscarded_retx = (non_decode_p2_2 >> 6) & 0xF;
                    int idid_recombining = (non_decode_p2_2 >> 10) & 1;

                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;

                    tb_item_j["Discarded ReTx"] = idiscarded_retx;

                    tb_item_j["Discarded reTx Present"] = idiscarded_re_tx_present;
                    const char* discarded_present_name = search_name(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent, ValueName), idiscarded_re_tx_present);
                    tb_item_j["Discarded reTx Present"] = (discarded_present_name ? discarded_present_name : "(MI)Unknown");


                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");

                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v36, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    int qed_interim_status_raw = tb_item_j["QED2 Interim Status"];
                    int qed_iterations = (qed_interim_status_raw >> 2) & 63; // 6 bits
                    int qed_interim_status = qed_interim_status_raw & 3; // 2 bits
                    tb_item_j["QED2 Interim Status"] = qed_interim_status;
                    tb_item_j["QED Iteration"] = qed_iterations;


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 12; // Size of one TB struct v40 is 12 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v40_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v40_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 34: {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v34,
                                     ARRAY_SIZE(LtePdschStatIndication_Payload_v34, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v34_P1,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v34_P1, Fmt),
                                         b, offset, length, record_item_j);

                // --- Decode P1 ---
                int non_decode_p1_1 = record_item_j["Subframe Num"];
                int isubfn = non_decode_p1_1 & 15;
                int ifn = (non_decode_p1_1 >> 4) & 4095;
                record_item_j["Subframe Num"] = isubfn;
                record_item_j["Frame Num"] = ifn;

                int non_decode_p1_2 = record_item_j["Serving Cell Index"];
                int iserv_cell_idx = non_decode_p1_2 & 7;
                record_item_j["Serving Cell Index"] = iserv_cell_idx;
                const char* serv_cell_name = search_name(ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName), iserv_cell_idx);
                record_item_j["Serving Cell Index"] = (serv_cell_name ? serv_cell_name : "(MI)Unknown");


                // --- Decode Transport Blocks ---
                int num_tb = record_item_j["Num Transport Blocks Present"];
                json tb_list_j = json::array();
                for (int tb_idx = 0; tb_idx < num_tb; ++tb_idx) {
                    json tb_item_j;
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v34,
                                             ARRAY_SIZE(LtePdschStatIndication_Record_TB_v34, Fmt),
                                             b, offset, length, tb_item_j);

                    int non_decode_p2_1 = tb_item_j["HARQ ID"];
                    int iharq_id = non_decode_p2_1 & 15;
                    int irv = (non_decode_p2_1 >> 4) & 3;
                    int indi = (non_decode_p2_1 >> 6) & 1;
                    int icrc_result = (non_decode_p2_1 >> 7) & 1;
                    tb_item_j["HARQ ID"] = iharq_id;
                    tb_item_j["RV"] = irv;
                    tb_item_j["NDI"] = indi;
                    tb_item_j["CRC Result"] = icrc_result;
                    const char* crc_name = search_name(LtePdschStatIndication_Record_TB_CrcResult, ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult, ValueName), icrc_result);
                    tb_item_j["CRC Result"] = (crc_name ? crc_name : "(MI)Unknown");


                    int non_decode_p2_2 = tb_item_j["RNTI Type"];
                    int irnti = non_decode_p2_2 & 15;
                    int itb_idx_field = (non_decode_p2_2 >> 4) & 1;
                    int idiscarded_retx = (non_decode_p2_2 >> 5) & 1; // 1 bit for v34
                    int idid_recombining = (non_decode_p2_2 >> 6) & 1;
                    tb_item_j["RNTI Type"] = irnti;
                    const char* rnti_name = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), irnti);
                    tb_item_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

                    tb_item_j["TB Index"] = itb_idx_field;

                    tb_item_j["Discarded ReTx"] = idiscarded_retx; // Store the 1-bit value
                    const char* discarded_retx_name = search_name(LtePdschStatIndication_Record_TB_Discarded_ReTx, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Discarded_ReTx, ValueName), idiscarded_retx);
                    tb_item_j["Discarded ReTx"] = (discarded_retx_name ? discarded_retx_name : "(MI)Unknown");


                    tb_item_j["Did Recombining"] = idid_recombining;
                    const char* recombining_name = search_name(LtePdschStatIndication_Record_TB_DidRecombining, ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining, ValueName), idid_recombining);
                    tb_item_j["Did Recombining"] = (recombining_name ? recombining_name : "(MI)Unknown");


                    // Modulation mapping
                    const char* mod_name = search_name(LtePdschStatIndication_Record_TB_Modulation_v34, ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v34, ValueName), tb_item_j["Modulation Type"]);
                    tb_item_j["Modulation Type"] = (mod_name ? mod_name : "(MI)Unknown");

                    // ACK/NACK Decision
                    tb_item_j["ACK/NACK Decision"] = icrc_result;
                    const char* ack_nack_name = search_name(LtePdschStatIndication_Record_TB_AckNackDecision, ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision, ValueName), icrc_result);
                    tb_item_j["ACK/NACK Decision"] = (ack_nack_name ? ack_nack_name : "(MI)Unknown");


                    // QED Status/Iteration
                    int qed_interim_status_raw = tb_item_j["QED2 Interim Status"];
                    int qed_iterations = (qed_interim_status_raw >> 2) & 63; // 6 bits
                    int qed_interim_status = qed_interim_status_raw & 3; // 2 bits
                    tb_item_j["QED2 Interim Status"] = qed_interim_status;
                    tb_item_j["QED Iteration"] = qed_iterations;
                    const char* qed_iter_name = search_name(LtePdschStatIndication_Record_TB_v34_QED_Iteration, ARRAY_SIZE(LtePdschStatIndication_Record_TB_v34_QED_Iteration, ValueName), qed_iterations);
                    tb_item_j["QED Iteration"] = (qed_iter_name ? qed_iter_name : std::to_string(qed_iterations)); // Map or show number


                    tb_list_j.push_back(tb_item_j);
                }
                record_item_j["Transport Blocks"] = tb_list_j;

                // --- Padding ---
                if (num_tb == 1) {
                    offset += 8; // Size of one TB struct v34 is 8 bytes
                }

                // --- Decode P2 ---
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v34_P2,
                                         ARRAY_SIZE(LtePdschStatIndication_Record_v34_P2, Fmt),
                                         b, offset, length, record_item_j);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PDSCH Stat Indication version: 0x%x", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PDSCH_STAT_INDICATION_H
