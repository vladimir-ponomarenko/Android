//
// Created by User on 23.03.2025.
//
#pragma once

#include "nlohmann/json.hpp"
#include "log_packet_utils.h"
#include "consts.h"
#include <android/log.h>


using json = nlohmann::json;
using namespace std;


const Fmt LteNb1Ml1GmTxReport[] = {
        {UINT, "Version", 1},
        {SKIP, NULL, 1},
};

const Fmt LteNb1Ml1GmTxReport_v2[] = {
        {UINT, "Subcarrier Space", 1},    //left 3 bits
        {UINT, "Num of Records", 1},
};

const Fmt LteNb1Ml1GmTxReport_Record_v2[] = {
        {UINT, "NPDCCH Timing HSFN", 4},         // 10 bits
        {PLACEHOLDER, "NPDCCH Timing SFN",0},    // 10 bits
        {PLACEHOLDER, "NPDCCH Timing Sub FN",0}, // 4 bits
        {PLACEHOLDER, "NPUSCH Format",0}, // 1 bit
        {PLACEHOLDER, "Is MSG3",0}, // 1 bit
        {PLACEHOLDER, "ITBS",0}, // 6 bits
        {UINT, "Repetition Number", 4},         // 8 bits
        {PLACEHOLDER, "Num RU",0}, // 4 bits
        {PLACEHOLDER, "RV Index",0}, // 1 bit
        {PLACEHOLDER, "Num Tone",0}, // 4 bits
        {PLACEHOLDER, "Start Tone",0}, // 7 bits
        {PLACEHOLDER, "TX Power",0}, // 8 bits
        {UINT, "NPUSCH format 1 TX Type", 4},         // 1 bit
        {PLACEHOLDER, "ACK NACK",0}, // 1 bit
        {PLACEHOLDER, "RFLM",0}, // 8 bits
        {PLACEHOLDER, "PRACH Collision Valid",0}, // 1 bits
        {PLACEHOLDER, "PRACH Collision 7680ms",0}, // 3 bits
        {PLACEHOLDER, "Reserved",0}, // 18 bits
        {UINT, "Scrambling Mask",4},
};

const ValueName ValueNameNB1_GM_TX_Report_Subcarrier_Space_Type[] = {
        {0, "15 kHz"},
        {1, "3.75 kHz"}
};

const ValueName ValueNameNPUSCHFormat[] = {
        {0, "Format 1"},
        {1, "Format 2"}
};

const ValueName ValueNameTrueOrFalse[] = {
        {0, "False"},
        {1, "True"}
};

const ValueName ValueNameNPUSCHFormat_1_TX_Type[] = {
        {0, "Data"},
        {1, "UCI"}
};

const ValueName ValueNameAckOrNack[] = {
        {0, "NACK"},
        {1, "ACK"}
};


static int _decode_lte_nb1_ml1_gm_tx_report_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_NB1_ML1_GM_TX_Report with version: 0x%x", pkt_ver);

    switch (pkt_ver) {
        case 2: {
            offset += _decode_by_fmt(LteNb1Ml1GmTxReport_v2,
                                     ARRAY_SIZE(LteNb1Ml1GmTxReport_v2, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num of Records"];

            unsigned int non_decode_subcarrier_space = j["Subcarrier Space"];
            int subcarrier_space = (non_decode_subcarrier_space >> 5) & 7;
            j["Subcarrier Space"] = subcarrier_space;

            const char* subcarrier_space_name = search_name(
                    ValueNameNB1_GM_TX_Report_Subcarrier_Space_Type,
                    ARRAY_SIZE(ValueNameNB1_GM_TX_Report_Subcarrier_Space_Type, ValueName),
                    j["Subcarrier Space"]
            );
            j["Subcarrier Space"] = (subcarrier_space_name != NULL) ? subcarrier_space_name : "(MI)Unknown";


            json records_j = json::array();
            for (int i = 0; i < num_records; i++) {
                json record_item_j;
                offset += _decode_by_fmt(LteNb1Ml1GmTxReport_Record_v2,
                                         ARRAY_SIZE(LteNb1Ml1GmTxReport_Record_v2, Fmt),
                                         b, offset, length, record_item_j);

                unsigned int non_decode_hsfn = record_item_j["NPDCCH Timing HSFN"];
                int ihsfn = non_decode_hsfn & 1023;          // 10 bits
                int isfn = (non_decode_hsfn >> 10) & 1023;   // 10 bits
                int isubfn = (non_decode_hsfn >> 20) & 15;   // 4 bits
                int inpusch_fmt = (non_decode_hsfn >> 24) & 1;              // 1 bit
                int ismsg3 = (non_decode_hsfn >> 25) & 1;         // 1 bit
                int iitbs = (non_decode_hsfn >> 26) & 63;           // 6 bits

                record_item_j["NPDCCH Timing HSFN"] = ihsfn;
                record_item_j["NPDCCH Timing SFN"] = isfn;
                record_item_j["NPDCCH Timing Sub FN"] = isubfn;
                record_item_j["NPUSCH Format"] = inpusch_fmt;

                const char* npusch_format_name = search_name(
                        ValueNameNPUSCHFormat,
                        ARRAY_SIZE(ValueNameNPUSCHFormat, ValueName),
                        record_item_j["NPUSCH Format"]
                );
                record_item_j["NPUSCH Format"] = (npusch_format_name != NULL) ? npusch_format_name : "(MI)Unknown";

                record_item_j["Is MSG3"] = ismsg3;
                const char* is_msg3_name = search_name(
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        record_item_j["Is MSG3"]
                );
                record_item_j["Is MSG3"] = (is_msg3_name != NULL) ? is_msg3_name : "(MI)Unknown";

                record_item_j["ITBS"] = iitbs;

                int non_decode_rep_num = record_item_j["Repetition Number"];
                int irep_num = non_decode_rep_num & 255;          // 8 bits
                int iru_num = (non_decode_rep_num >> 8) & 15;   // 4 bits
                int irv_ind = (non_decode_rep_num >> 12) & 1;   // 1 bits
                int inum_tone = (non_decode_rep_num >> 13) & 15;              // 4 bit
                int istart_tone = (non_decode_rep_num >> 17) & 127;         // 7 bit
                int itx_power = non_decode_rep_num >> 24;           // 8 bits

                record_item_j["Repetition Number"] = irep_num;
                record_item_j["Num RU"] = iru_num;
                record_item_j["RV Index"] = irv_ind;
                record_item_j["Num Tone"] = inum_tone;
                record_item_j["Start Tone"] = istart_tone;
                record_item_j["TX Power"] = itx_power;


                unsigned int non_decode_npusch_fmt_tx = record_item_j["NPUSCH format 1 TX Type"];
                int inpusch_fmt_tx = non_decode_npusch_fmt_tx & 1;          // 1 bits
                int iack = (non_decode_npusch_fmt_tx >> 1) & 1;   // 1 bits
                int irflm = (non_decode_npusch_fmt_tx >> 2) & 255;   // 8 bits
                int iprach_col_val = (non_decode_npusch_fmt_tx >> 10) & 1;              // 1 bit
                int iprach_col7680 = (non_decode_npusch_fmt_tx >> 11) & 7;         // 3 bit
                int ireserved = (non_decode_npusch_fmt_tx >> 14) & 0x3ffff;           // 18 bits

                record_item_j["NPUSCH format 1 TX Type"] = inpusch_fmt_tx;
                const char* npusch_format_1_tx_type_name = search_name(
                        ValueNameNPUSCHFormat_1_TX_Type,
                        ARRAY_SIZE(ValueNameNPUSCHFormat_1_TX_Type, ValueName),
                        record_item_j["NPUSCH format 1 TX Type"]
                );
                record_item_j["NPUSCH format 1 TX Type"] = (npusch_format_1_tx_type_name != NULL) ? npusch_format_1_tx_type_name : "(MI)Unknown";

                record_item_j["ACK NACK"] = iack;
                const char* ack_nack_name = search_name(
                        ValueNameAckOrNack,
                        ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                        record_item_j["ACK NACK"]
                );
                record_item_j["ACK NACK"] = (ack_nack_name != NULL) ? ack_nack_name : "(MI)Unknown";

                record_item_j["RFLM"] = irflm;
                record_item_j["PRACH Collision Valid"] = iprach_col_val;
                record_item_j["PRACH Collision 7680ms"] = iprach_col7680;
                record_item_j["Reserved"] = ireserved;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE NB1 ML1 GM TX Report version: 0x%x\n", pkt_ver);
            return 0;
    }
}

