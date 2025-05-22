//
// Created by Vladimir Ponomarenko on 02.04.2025.
//

#ifndef LOGIN_LTE_PHY_PUSCH_TX_REPORT_H
#define LOGIN_LTE_PHY_PUSCH_TX_REPORT_H

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

static std::string format_hex(unsigned int value, int width) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(width) << std::setfill('0') << value;
    return ss.str();
}

const Fmt LtePhyPuschTxReport_Fmt[] = {
        {UINT, "Version", 1},
};

// --- Version 23 ---
const Fmt LtePhyPuschTxReport_Payload_v23[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v23[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "Coding Rate Data", 2},
        {UINT, "ACK", 4},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {UINT, "DMRS Root Slot 1", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Num ACK Bits", 2},
        {PLACEHOLDER, "ACK Payload", 0},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "PUSCH Mod Order", 0},
        {PLACEHOLDER, "RI Payload2", 0},
        {UINT, "PUSCH Digital Gain (dB)", 1},
        {UINT, "SRS Occasion", 1},
        {PLACEHOLDER, "Re-tx Index", 0},
        {SKIP, NULL, 2},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {BYTE_STREAM, "CQI Payload", 16},
        {UINT, "Tx Resampler", 4},
};

// --- Version 24 ---
const Fmt LtePhyPuschTxReport_Payload_v24[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v24[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "Coding Rate Data", 2},
        {UINT, "ACK", 4},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {UINT, "DMRS Root Slot 1", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Num ACK Bits", 2},
        {PLACEHOLDER, "ACK Payload", 0},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "PUSCH Mod Order", 0},
        {PLACEHOLDER, "RI Payload2", 0},
        {UINT, "PUSCH Digital Gain (dB)", 1},
        {UINT, "SRS Occasion", 1},
        {PLACEHOLDER, "Re-tx Index", 0},
        {SKIP, NULL, 2},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {BYTE_STREAM, "CQI Payload", 16},
        {UINT, "Tx Resampler", 4},
};

// --- Version 26 ---
const Fmt LtePhyPuschTxReport_Payload_v26[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v26[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "Coding Rate Data", 2},
        {UINT, "ACK", 4},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {UINT, "DMRS Root Slot 1", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Num ACK Bits", 2},
        {PLACEHOLDER, "ACK Payload", 0},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "PUSCH Mod Order", 0},
        {PLACEHOLDER, "RI Payload2", 0},
        {UINT, "PUSCH Digital Gain (dB)", 1},
        {UINT, "SRS Occasion", 1},
        {PLACEHOLDER, "Re-tx Index", 0},
        {SKIP, NULL, 2},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {BYTE_STREAM, "CQI Payload", 16},
        {UINT, "Tx Resampler", 4},
        {UINT, "Num Repetition", 4},
        {PLACEHOLDER,"RB NB Start Index",0},
};

// --- Version 43 ---
const Fmt LtePhyPuschTxReport_Payload_v43[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v43[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "Coding Rate Data", 2},
        {UINT, "ACK", 4},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {UINT, "DMRS Root Slot 1", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Rate Matched ACK Bits", 2},
        {UINT, "ACK Payload", 4},
        {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},
        {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {UINT, "RI Payload", 4},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "PUSCH Mod Order", 0},
        {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},
        {PLACEHOLDER, "SRS Occasion", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {SKIP, NULL, 4},
        {BYTE_STREAM, "CQI Payload", 28},
        {UINT, "Tx Resampler", 4},
};

// --- Version 102 ---
const Fmt LtePhyPuschTxReport_Payload_v102[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v102[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "Carrier Index", 2},
        {PLACEHOLDER, "ACK", 0},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {UINT, "Resource Allocation Type", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Coding Rate", 2},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {PLACEHOLDER, "SRS Occasion", 0},
        {UINT, "ACK Payload", 4},
        {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},
        {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {UINT, "PUSCH Mod Order", 4},
        {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},
        {PLACEHOLDER, "Start RB Cluster1", 0},
        {PLACEHOLDER, "Num RB Cluster1", 0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {UINT, "Num DL Carriers", 4},
        {PLACEHOLDER, "Ack Nack Index", 0},
        {PLACEHOLDER, "Ack Nack Late", 0},
        {PLACEHOLDER, "CSF Late", 0},
        {PLACEHOLDER, "Drop PUSCH", 0},
        {BYTE_STREAM, "CQI Payload", 28},
        {UINT, "Tx Resampler", 4},
        {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "DMRS Root Slot 1", 0},
};

// --- Version 122 ---
const Fmt LtePhyPuschTxReport_Payload_v122[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v122[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "UL Carrier Index", 2},
        {PLACEHOLDER, "ACK", 0},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {UINT, "Start RB Slot 0", 4},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {PLACEHOLDER, "DL Carrier Index",0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Coding Rate", 2},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {PLACEHOLDER, "SRS Occasion", 0},
        {UINT, "ACK Payload", 4},
        {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},
        {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {UINT, "PUSCH Mod Order", 4},
        {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {UINT, "Num DL Carriers", 4},
        {PLACEHOLDER, "Ack Nack Index", 0},
        {PLACEHOLDER, "Ack Nack Late", 0},
        {PLACEHOLDER, "CSF Late", 0},
        {PLACEHOLDER, "Drop PUSCH", 0},
        {BYTE_STREAM, "CQI Payload", 28},
        {UINT, "Tx Resampler", 4},
        {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "DMRS Root Slot 1", 0},
};

// --- Version 124 ---
const Fmt LtePhyPuschTxReport_Payload_v124[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v124[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "UL Carrier Index", 2},
        {PLACEHOLDER, "ACK", 0},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {UINT, "Resource Allocation Type", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {PLACEHOLDER, "DL Carrier Index",0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Coding Rate", 2},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Rate Matched RI Bits", 0},
        {PLACEHOLDER, "UE SRS", 0},
        {PLACEHOLDER, "SRS Occasion", 0},
        {UINT, "ACK Payload", 4},
        {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},
        {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},
        {PLACEHOLDER, "Num RI Bits NRI (bits)", 0},
        {UINT, "PUSCH Mod Order", 4},
        {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},
        {PLACEHOLDER, "Start RB Cluster1", 0},
        {PLACEHOLDER, "Num RB Cluster1", 0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {UINT, "Num DL Carriers", 4},
        {PLACEHOLDER, "Ack Nack Index", 0},
        {PLACEHOLDER, "Ack Nack Late", 0},
        {PLACEHOLDER, "CSF Late", 0},
        {PLACEHOLDER, "Drop PUSCH", 0},
        {BYTE_STREAM, "CQI Payload", 44},
        {UINT, "Tx Resampler", 4},
        {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
        {PLACEHOLDER, "DMRS Root Slot 0", 0},
        {PLACEHOLDER, "DMRS Root Slot 1", 0},
};

// --- Version 144 ---
const Fmt LtePhyPuschTxReport_Payload_v144[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};
const Fmt LtePhyPuschTxReport_Record_v144[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "UL Carrier Index", 2},
        {PLACEHOLDER, "ACK", 0},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {UINT, "Resource Allocation Type", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {PLACEHOLDER, "DL Carrier Index",0},
        {PLACEHOLDER,"Enable UL DMRS OCC",0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Coding Rate", 2},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Num RI Bits NRI", 0},
        {UINT, "ACK Payload", 16},
        {UINT,"ACK/NAK Inp Length 0",4},
        {PLACEHOLDER,"ACK/NAK Inp Length 1",0},
        {PLACEHOLDER,"Rate Matched RI Bits",0},
        {PLACEHOLDER,"Reserved",0},
        {UINT,"UE SRS",4},
        {PLACEHOLDER,"SRS Occasion",0},
        {PLACEHOLDER,"PUSCH Mod Order",0},
        {PLACEHOLDER,"PUSCH Digital Gain",0},
        {PLACEHOLDER,"Start RB Cluster1",0},
        {PLACEHOLDER,"Num RB Cluster1",0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {UINT, "Num DL Carriers", 4},
        {PLACEHOLDER, "Ack Nack Index", 0},
        {PLACEHOLDER, "Ack Nack Late", 0},
        {PLACEHOLDER, "CSF Late", 0},
        {PLACEHOLDER, "Drop PUSCH", 0},
        {BYTE_STREAM, "CQI Payload", 44},
        {UINT, "Tx Resampler", 4},
        {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
};


// --- Version 145 ---
const Fmt LtePhyPuschTxReport_Payload_v145[] = {
        {UINT, "Serving Cell ID", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v145[] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "UL Carrier Index", 2},
        {PLACEHOLDER, "ACK", 0},
        {PLACEHOLDER, "CQI", 0},
        {PLACEHOLDER, "RI", 0},
        {PLACEHOLDER, "Frequency Hopping", 0},
        {PLACEHOLDER, "Re-tx Index", 0},
        {PLACEHOLDER, "Redund Ver", 0},
        {PLACEHOLDER, "Mirror Hopping", 0},
        {UINT, "Resource Allocation Type", 4},
        {PLACEHOLDER, "Start RB Slot 0", 0},
        {PLACEHOLDER, "Start RB Slot 1", 0},
        {PLACEHOLDER, "Num of RB", 0},
        {PLACEHOLDER, "DL Carrier Index",0},
        {PLACEHOLDER,"Enable UL DMRS OCC",0},
        {UINT, "PUSCH TB Size", 2},
        {UINT, "Coding Rate", 2},
        {UINT, "Rate Matched ACK Bits", 4},
        {PLACEHOLDER, "RI Payload", 0},
        {PLACEHOLDER, "Num RI Bits NRI", 0},
        {UINT, "ACK Payload", 16},
        {UINT,"ACK/NAK Inp Length 0",4},
        {PLACEHOLDER,"ACK/NAK Inp Length 1",0},
        {PLACEHOLDER,"Rate Matched RI Bits",0},
        {PLACEHOLDER,"Reserved",0},
        {UINT,"UE SRS",4},
        {PLACEHOLDER,"SRS Occasion",0},
        {PLACEHOLDER,"PUSCH Mod Order",0},
        {PLACEHOLDER,"PUSCH Digital Gain",0},
        {PLACEHOLDER,"Start RB Cluster1",0},
        {PLACEHOLDER,"Num RB Cluster1",0},
        {UINT, "PUSCH Tx Power (dBm)", 4},
        {PLACEHOLDER, "Num CQI Bits", 0},
        {PLACEHOLDER, "Rate Matched CQI Bits", 0},
        {UINT, "Num DL Carriers", 4},
        {PLACEHOLDER, "Ack Nack Index", 0},
        {PLACEHOLDER, "Ack Nack Late", 0},
        {PLACEHOLDER, "CSF Late", 0},
        {PLACEHOLDER, "Drop PUSCH", 0},
        {BYTE_STREAM, "CQI Payload", 44},
        {UINT, "Tx Resampler", 4},
        {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},
        {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},
};


const ValueName ValueNameExistsOrNone[] = {
        {0, "None"},
        {1, "Exists"}
};


const ValueName ValueNameOnOrOff[] = {
        {0, "Off"},
        {1, "On"}
};

const ValueName ValueNameModulation[] = {
        {0, "BPSK"},
        {1, "QPSK"},
        {2, "16QAM"},
        {3, "64QAM"},
        {4, "256QAM"}
};


static int _decode_lte_phy_pusch_tx_report_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    int temp = 0;
    unsigned int u_temp = 0;

    LOGD("Decoding LTE_PHY_PUSCH_Tx_Report with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 23:
//        case 25:
        case 24:
        {
            if (pkt_ver == 23) {
                offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v23,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Payload_v23, Fmt),
                                         b, offset, length, j);
            } else {
                offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v24,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Payload_v24, Fmt),
                                         b, offset, length, j);
            }

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;    // 9 bits
            int num_records = (temp >> 9) & 31; // 5 bits
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                if (pkt_ver == 23) {
                    offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v23,
                                             ARRAY_SIZE(LtePhyPuschTxReport_Record_v23, Fmt),
                                             b, offset, length, record_item_j);
                } else { // v24
                    offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v24,
                                             ARRAY_SIZE(LtePhyPuschTxReport_Record_v24, Fmt),
                                             b, offset, length, record_item_j);
                }

                // Coding Rate
                temp = record_item_j["Coding Rate Data"];
                record_item_j["Coding Rate Data"] = (double)temp / 1024.0;

                // ACK/CQI/RI/etc. from 'ACK' field
                u_temp = record_item_j["ACK"];
                int iAck = u_temp & 1;
                int iCQI = (u_temp >> 1) & 1;
                int iRI = (u_temp >> 2) & 1;
                int iFrequencyHopping = (u_temp >> 3) & 3;
                int iRedundVer = (u_temp >> 5) & 3;
                int iMirrorHopping = (u_temp >> 7) & 3;
                int iCSDSS0 = (u_temp >> 9) & 15;
                int iCSDSS1 = (u_temp >> 13) & 15;
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047;
                int iUESRS = (u_temp >> 28) & 1;

                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");

                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");

                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");

                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");

                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;

                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");

                u_temp = record_item_j["DMRS Root Slot 1"];
                int iDMRSRootSlot1 = u_temp & 2047;
                int iStartRBSlot0 = (u_temp >> 11) & 127;
                int iStartRBSlot1 = (u_temp >> 18) & 127;
                int iNumOfRB = (u_temp >> 25) & 127;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;

                // ACK Payload bits from 'Num ACK Bits' field
                u_temp = record_item_j["Num ACK Bits"];
                int iNumACKBits = u_temp & 0x7; // 3 bits
                int iACKPayload_part1 = (u_temp >> 3) & 0xF;
                record_item_j["Num ACK Bits"] = iNumACKBits;
                record_item_j["ACK Payload"] = iACKPayload_part1;

                // RI Payload / Mod Order etc. from 'Rate Matched ACK Bits' field
                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedACKBits = u_temp & 0x7FF; // 11 bits
                int iNumRIBit = (u_temp >> 11) & 0x3;   // 2 bits
                int iRIPayload_part1 = (u_temp >> 13) & 0x3; // 2 bits
                int iRateMatchedRIBits = (u_temp >> 15) & 0x7FF; // 11 bits
                int iPUSCHModOrder = (u_temp >> 26) & 0x3; // 2 bits
                int iRIPayload2_part1 = (u_temp >> 28) & 0xF; // 4 bits

                record_item_j["Rate Matched ACK Bits"] = iRateMatchedACKBits;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRIBit;
                record_item_j["RI Payload"] = iRIPayload_part1;
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRIBits;

                record_item_j["PUSCH Mod Order"] = iPUSCHModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPUSCHModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");

                record_item_j["RI Payload2"] = iRIPayload2_part1;

                // SRS/Re-tx from 'SRS Occasion' field
                u_temp = record_item_j["SRS Occasion"];
                int iSRSOccasion = u_temp & 0x1;
                int iRetxIndex = (u_temp >> 1) & 0x1F; // 5 bits
                record_item_j["SRS Occasion"] = iSRSOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSRSOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");

                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));


                // TxPower/CQI
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPUSCHTxPower = u_temp & 0x3FF; // 10 bits
                int iNumCqiBits = (u_temp >> 10) & 0xFF; // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 18) & 0x3FFF; // 14 bits
                record_item_j["PUSCH Tx Power (dBm)"] = iPUSCHTxPower;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }

        case 26: {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v26,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v26, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;    // 9 bits
            int num_records = (temp >> 9) & 31; // 5 bits
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v26,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Record_v26, Fmt),
                                         b, offset, length, record_item_j);

                temp = record_item_j["Coding Rate Data"];
                record_item_j["Coding Rate Data"] = (double)temp / 1024.0;

                u_temp = record_item_j["ACK"];
                int iAck = u_temp & 1;
                int iCQI = (u_temp >> 1) & 1;
                int iRI = (u_temp >> 2) & 1;
                int iFrequencyHopping = (u_temp >> 3) & 3;
                int iRedundVer = (u_temp >> 5) & 3;
                int iMirrorHopping = (u_temp >> 7) & 3;
                int iCSDSS0 = (u_temp >> 9) & 15;
                int iCSDSS1 = (u_temp >> 13) & 15;
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047;
                int iUESRS = (u_temp >> 28) & 1;
                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");


                u_temp = record_item_j["DMRS Root Slot 1"];
                int iDMRSRootSlot1 = u_temp & 2047;
                int iStartRBSlot0 = (u_temp >> 11) & 127;
                int iStartRBSlot1 = (u_temp >> 18) & 127;
                int iNumOfRB = (u_temp >> 25) & 127;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;

                u_temp = record_item_j["Num ACK Bits"];
                int iNumACKBits = u_temp & 0x7;
                int iACKPayload_part1 = (u_temp >> 3) & 0xF;
                record_item_j["Num ACK Bits"] = iNumACKBits;
                record_item_j["ACK Payload"] = iACKPayload_part1;

                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedACKBits = u_temp & 0x7FF;
                int iNumRIBit = (u_temp >> 11) & 0x3;
                int iRIPayload_part1 = (u_temp >> 13) & 0x3;
                int iRateMatchedRIBits = (u_temp >> 15) & 0x7FF;
                int iPUSCHModOrder = (u_temp >> 26) & 0x3;
                int iRIPayload2_part1 = (u_temp >> 28) & 0xF;
                record_item_j["Rate Matched ACK Bits"] = iRateMatchedACKBits;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRIBit;
                record_item_j["RI Payload"] = iRIPayload_part1;
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRIBits;
                record_item_j["PUSCH Mod Order"] = iPUSCHModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPUSCHModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");
                record_item_j["RI Payload2"] = iRIPayload2_part1;

                u_temp = record_item_j["SRS Occasion"];
                int iSRSOccasion = u_temp & 0x1;
                int iRetxIndex = (u_temp >> 1) & 0x1F;
                record_item_j["SRS Occasion"] = iSRSOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSRSOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));

                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPUSCHTxPower = u_temp & 0x3FF;
                int iNumCqiBits = (u_temp >> 10) & 0xFF;
                int iRateMatchedCqiBits = (u_temp >> 18) & 0x3FFF;
                record_item_j["PUSCH Tx Power (dBm)"] = iPUSCHTxPower;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                // New fields for v26
                u_temp = record_item_j["Num Repetition"];
                int iNumRepetition = u_temp & 0xFFF;   // 12 bits
                int iRBNBStartIndex = (u_temp >> 12) & 0xFF; // 8 bits
                record_item_j["Num Repetition"] = iNumRepetition;
                record_item_j["RB NB Start Index"] = iRBNBStartIndex;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 43: {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v43,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v43, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;
            int num_records = (temp >> 9) & 31;
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v43,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Record_v43, Fmt),
                                         b, offset, length, record_item_j);

                // Coding Rate
                temp = record_item_j["Coding Rate Data"];
                record_item_j["Coding Rate Data"] = (double)temp / 1024.0;

                // ACK/CQI/RI/etc. from 'ACK' field
                u_temp = record_item_j["ACK"];
                int iAck = u_temp & 1;
                int iCQI = (u_temp >> 1) & 1;
                int iRI = (u_temp >> 2) & 1;
                int iFrequencyHopping = (u_temp >> 3) & 3;
                int iRedundVer = (u_temp >> 5) & 3;
                int iMirrorHopping = (u_temp >> 7) & 3;
                int iCSDSS0 = (u_temp >> 9) & 15;
                int iCSDSS1 = (u_temp >> 13) & 15;
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047;
                int iUESRS = (u_temp >> 28) & 1;

                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");

                // DMRS Root 1/RB info from 'DMRS Root Slot 1' field
                u_temp = record_item_j["DMRS Root Slot 1"];
                int iDMRSRootSlot1 = u_temp & 2047;
                int iStartRBSlot0 = (u_temp >> 11) & 127;
                int iStartRBSlot1 = (u_temp >> 18) & 127;
                int iNumOfRB = (u_temp >> 25) & 127;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;

                // ACK Payload / Lengths / NumRI from 'ACK Payload' field
                u_temp = record_item_j["ACK Payload"];
                unsigned int ack_payload_val = u_temp & 0xFFFFF; // 20 bits
                int iAckNakInpLength0 = (u_temp >> 20) & 15; // 4 bits
                int iAckNakInpLength1 = (u_temp >> 24) & 15; // 4 bits
                int iNumRiBitsNri = (u_temp >> 28) & 7;      // 3 bits

                // Format ACK Payload as hex string (5 hex digits needed for 20 bits)
                record_item_j["ACK Payload"] = format_hex(ack_payload_val, 5);
                record_item_j["ACK/NAK Inp Length 0"] = iAckNakInpLength0;
                record_item_j["ACK/NAK Inp Length 1"] = iAckNakInpLength1;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRiBitsNri;

                // RI Payload / RM RI / Mod Order etc. from 'RI Payload' field
                u_temp = record_item_j["RI Payload"];
                int iRiPayload_val = u_temp & 15;              // 4 bits
                int iRateMatchedRiBits = (u_temp >> 4) & 2047; // 11 bits
                int iPuschModOrder = (u_temp >> 15) & 3;     // 2 bits
                int iPuschDigitalGain = (u_temp >> 17) & 255;  // 8 bits
                int iSrsOccasion = (u_temp >> 25) & 1;     // 1 bit
                int iRetxIndex = (u_temp >> 26) & 7;       // 3 bits

                record_item_j["RI Payload"] = format_hex(iRiPayload_val, 1); // 1 hex digit for 4 bits
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRiBits;
                record_item_j["PUSCH Mod Order"] = iPuschModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPuschModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");
                record_item_j["PUSCH Digital Gain (dB)"] = iPuschDigitalGain;
                record_item_j["SRS Occasion"] = iSrsOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSrsOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));


                // TxPower/CQI bits from 'PUSCH Tx Power (dBm)' field
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPuschTxPower = u_temp & 127; // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535; // 16 bits
                record_item_j["PUSCH Tx Power (dBm)"] = iPuschTxPower;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 102: {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;
            int num_records = (temp >> 9) & 31;
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v102,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Record_v102, Fmt),
                                         b, offset, length, record_item_j);

                // Carrier Idx / ACK / CQI / RI etc from 'Carrier Index' field
                u_temp = record_item_j["Carrier Index"];
                int iCarrierIndex = u_temp & 3;
                int iAck = (u_temp >> 2) & 1;
                int iCQI = (u_temp >> 3) & 1;
                int iRI = (u_temp >> 4) & 1;
                int iFrequencyHopping = (u_temp >> 5) & 3;
                int iRetxIndex = (u_temp >> 7) & 31; // 5 bits
                int iRedundVer = (u_temp >> 12) & 3;
                int iMirrorHopping = (u_temp >> 14) & 3;

                record_item_j["Carrier Index"] = iCarrierIndex;
                const char* carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iCarrierIndex);
                record_item_j["Carrier Index"] = (carr_idx_name ? carr_idx_name : "(MI)Unknown");
                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;

                // RA Type / RB Info from 'Resource Allocation Type' field
                u_temp = record_item_j["Resource Allocation Type"];
                int iResourceAllocationType = u_temp & 1;
                int iStartRBSlot0 = (u_temp >> 1) & 127;
                int iStartRBSlot1 = (u_temp >> 8) & 127;
                int iNumOfRB = (u_temp >> 15) & 127;
                record_item_j["Resource Allocation Type"] = iResourceAllocationType;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;

                // Coding Rate
                temp = record_item_j["Coding Rate"];
                record_item_j["Coding Rate"] = (double)temp / 1024.0;

                // RM ACK / RI Payload / RM RI etc from 'Rate Matched ACK Bits' field
                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedAckBits = u_temp & 16383; // 14 bits
                int iRiPayload_val = (u_temp >> 14) & 15;    // 4 bits
                int iRateMatchedRiBits = (u_temp >> 18) & 2047; // 11 bits
                int iUESRS = (u_temp >> 29) & 1;
                int iSrsOccasion = (u_temp >> 30) & 1;
                record_item_j["Rate Matched ACK Bits"] = iRateMatchedAckBits;
                record_item_j["RI Payload"] = format_hex(iRiPayload_val, 1);
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRiBits;
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");
                record_item_j["SRS Occasion"] = iSrsOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSrsOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");


                // ACK Payload / Lengths / NumRI from 'ACK Payload' field
                u_temp = record_item_j["ACK Payload"];
                unsigned int ack_payload_val = u_temp & 0xFFFFF;
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                record_item_j["ACK Payload"] = format_hex(ack_payload_val, 5);
                record_item_j["ACK/NAK Inp Length 0"] = iAckNakInpLength0;
                record_item_j["ACK/NAK Inp Length 1"] = iAckNakInpLength1;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRiBitsNri;

                // Mod Order / Gain / Cluster info from 'PUSCH Mod Order' field
                u_temp = record_item_j["PUSCH Mod Order"];
                int iPuschModOrder = u_temp & 3;
                int iPuschDigitalGain = (u_temp >> 2) & 255;
                int iStartRBCluster1 = (u_temp >> 10) & 255;
                int iNumRBCluster1 = (u_temp >> 18) & 63;
                record_item_j["PUSCH Mod Order"] = iPuschModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPuschModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");
                record_item_j["PUSCH Digital Gain (dB)"] = iPuschDigitalGain;
                record_item_j["Start RB Cluster1"] = iStartRBCluster1;
                record_item_j["Num RB Cluster1"] = iNumRBCluster1;

                // Tx Power / CQI Bits from 'PUSCH Tx Power (dBm)' field
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPuschTxPower = (u_temp & 127); // 7 bits raw
                int iNumCqiBits = (u_temp >> 7) & 255;
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;
                record_item_j["PUSCH Tx Power (dBm)"] = iPuschTxPower - 128;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                // DL Carriers / Ack Nack info from 'Num DL Carriers' field
                u_temp = record_item_j["Num DL Carriers"];
                int iNumDLCarriers = u_temp & 3;
                int iAckNackIndex = (u_temp >> 2) & 0xFFF; // 12 bits
                int iAckNackLate = (u_temp >> 14) & 1;
                int iCSFLate = (u_temp >> 15) & 1;
                int iDropPusch = (u_temp >> 16) & 1;
                record_item_j["Num DL Carriers"] = iNumDLCarriers;
                record_item_j["Ack Nack Index"] = iAckNackIndex;
                record_item_j["Ack Nack Late"] = iAckNackLate;
                record_item_j["CSF Late"] = iCSFLate;
                record_item_j["Drop PUSCH"] = iDropPusch;

                // CQI Payload handled by BYTE_STREAM

                // DMRS info from 'Cyclic Shift...' field
                u_temp = record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"];
                int iCSDSS0 = u_temp & 15;
                int iCSDSS1 = (u_temp >> 4) & 15;
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047;
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;


                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 122: {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;
            int num_records = (temp >> 9) & 31;
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v122,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Record_v122, Fmt),
                                         b, offset, length, record_item_j);

                // UL Carrier Idx / ACK / CQI / RI etc from 'UL Carrier Index' field
                u_temp = record_item_j["UL Carrier Index"];
                int iULCarrierIndex = u_temp & 3;
                int iAck = (u_temp >> 2) & 1;
                int iCQI = (u_temp >> 3) & 1;
                int iRI = (u_temp >> 4) & 1;
                int iFrequencyHopping = (u_temp >> 5) & 3;
                int iRetxIndex = (u_temp >> 7) & 31;
                int iRedundVer = (u_temp >> 12) & 3;
                int iMirrorHopping = (u_temp >> 14) & 3;

                record_item_j["UL Carrier Index"] = iULCarrierIndex;
                const char* ul_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iULCarrierIndex);
                record_item_j["UL Carrier Index"] = (ul_carr_idx_name ? ul_carr_idx_name : "(MI)Unknown");
                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;

                // RB Info / DL Carrier Idx from 'Start RB Slot 0' field
                u_temp = record_item_j["Start RB Slot 0"];
                int iStartRBSlot0 = u_temp & 127;
                int iStartRBSlot1 = (u_temp >> 7) & 127;
                int iNumOfRB = (u_temp >> 14) & 127;
                int iDLCarrierIndex = (u_temp >> 21) & 7; // 3 bits
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;
                record_item_j["DL Carrier Index"] = iDLCarrierIndex;
                const char* dl_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iDLCarrierIndex);
                record_item_j["DL Carrier Index"] = (dl_carr_idx_name ? dl_carr_idx_name : "(MI)Unknown");

                // Coding Rate
                temp = record_item_j["Coding Rate"];
                record_item_j["Coding Rate"] = (double)temp / 1024.0;

                // RM ACK / RI Payload etc. from 'Rate Matched ACK Bits'
                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedAckBits = u_temp & 16383;
                int iRiPayload_val = (u_temp >> 14) & 15;
                int iRateMatchedRiBits = (u_temp >> 18) & 2047;
                int iUESRS = (u_temp >> 29) & 1;
                int iSrsOccasion = (u_temp >> 30) & 1;
                record_item_j["Rate Matched ACK Bits"] = iRateMatchedAckBits;
                record_item_j["RI Payload"] = format_hex(iRiPayload_val, 1);
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRiBits;
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");
                record_item_j["SRS Occasion"] = iSrsOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSrsOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");

                // ACK Payload / Lengths / NumRI from 'ACK Payload' field
                u_temp = record_item_j["ACK Payload"];
                unsigned int ack_payload_val = u_temp & 0xFFFFF;
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                record_item_j["ACK Payload"] = format_hex(ack_payload_val, 5);
                record_item_j["ACK/NAK Inp Length 0"] = iAckNakInpLength0;
                record_item_j["ACK/NAK Inp Length 1"] = iAckNakInpLength1;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRiBitsNri;

                // Mod Order / Gain from 'PUSCH Mod Order' field
                u_temp = record_item_j["PUSCH Mod Order"];
                int iPuschModOrder = u_temp & 3;
                int iPuschDigitalGain = (u_temp >> 2) & 255;
                record_item_j["PUSCH Mod Order"] = iPuschModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPuschModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");
                record_item_j["PUSCH Digital Gain (dB)"] = iPuschDigitalGain;

                // Tx Power / CQI Bits from 'PUSCH Tx Power (dBm)' field
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPuschTxPower = (u_temp & 127);
                int iNumCqiBits = (u_temp >> 7) & 255;
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;
                record_item_j["PUSCH Tx Power (dBm)"] = iPuschTxPower - 128;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                // DL Carriers / Ack Nack info from 'Num DL Carriers' field
                u_temp = record_item_j["Num DL Carriers"];
                int iNumDLCarriers = u_temp & 3;
                int iAckNackIndex = (u_temp >> 2) & 0xFFF;
                int iAckNackLate = (u_temp >> 14) & 1;
                int iCSFLate = (u_temp >> 15) & 1;
                int iDropPusch = (u_temp >> 16) & 1;
                record_item_j["Num DL Carriers"] = iNumDLCarriers;
                record_item_j["Ack Nack Index"] = iAckNackIndex;
                record_item_j["Ack Nack Late"] = iAckNackLate;
                record_item_j["CSF Late"] = iCSFLate;
                record_item_j["Drop PUSCH"] = iDropPusch;

                // CQI Payload handled by BYTE_STREAM

                // DMRS info from 'Cyclic Shift...' field
                u_temp = record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"];
                int iCSDSS0 = u_temp & 15;
                int iCSDSS1 = (u_temp >> 4) & 15;
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047;
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 124: {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;
            int num_records = (temp >> 9) & 31;
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v124,
                                         ARRAY_SIZE(LtePhyPuschTxReport_Record_v124, Fmt),
                                         b, offset, length, record_item_j);

                // UL Carrier Idx / ACK / CQI / RI etc from 'UL Carrier Index' field
                u_temp = record_item_j["UL Carrier Index"];
                int iULCarrierIndex = u_temp & 3;
                int iAck = (u_temp >> 2) & 1;
                int iCQI = (u_temp >> 3) & 1;
                int iRI = (u_temp >> 4) & 1;
                int iFrequencyHopping = (u_temp >> 5) & 3;
                int iRetxIndex = (u_temp >> 7) & 31;
                int iRedundVer = (u_temp >> 12) & 3;
                int iMirrorHopping = (u_temp >> 14) & 3;

                record_item_j["UL Carrier Index"] = iULCarrierIndex;
                const char* ul_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iULCarrierIndex);
                record_item_j["UL Carrier Index"] = (ul_carr_idx_name ? ul_carr_idx_name : "(MI)Unknown");
                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;

                // RA Type / RB Info / DL Idx from 'Resource Allocation Type' field
                u_temp = record_item_j["Resource Allocation Type"];
                int iResourceAllocationType = u_temp & 1;
                int iStartRBSlot0 = (u_temp >> 1) & 127;
                int iStartRBSlot1 = (u_temp >> 8) & 127;
                int iNumOfRB = (u_temp >> 15) & 127;
                int iDLCarrierIndex = (u_temp >> 25) & 3;
                record_item_j["Resource Allocation Type"] = iResourceAllocationType;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;
                record_item_j["DL Carrier Index"] = iDLCarrierIndex;
                const char* dl_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iDLCarrierIndex);
                record_item_j["DL Carrier Index"] = (dl_carr_idx_name ? dl_carr_idx_name : "(MI)Unknown");


                // Coding Rate
                temp = record_item_j["Coding Rate"];
                record_item_j["Coding Rate"] = (double)temp / 1024.0;

                // RM ACK / RI Payload etc. from 'Rate Matched ACK Bits'
                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedAckBits = u_temp & 16383;
                int iRiPayload_val = (u_temp >> 14) & 15;
                int iRateMatchedRiBits = (u_temp >> 18) & 2047;
                int iUESRS = (u_temp >> 29) & 1;
                int iSrsOccasion = (u_temp >> 30) & 1;
                record_item_j["Rate Matched ACK Bits"] = iRateMatchedAckBits;
                record_item_j["RI Payload"] = format_hex(iRiPayload_val, 1);
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRiBits;
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");
                record_item_j["SRS Occasion"] = iSrsOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSrsOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");

                // ACK Payload / Lengths / NumRI from 'ACK Payload' field
                u_temp = record_item_j["ACK Payload"];
                unsigned int ack_payload_val = u_temp & 0xFFFFF;
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                record_item_j["ACK Payload"] = format_hex(ack_payload_val, 5);
                record_item_j["ACK/NAK Inp Length 0"] = iAckNakInpLength0;
                record_item_j["ACK/NAK Inp Length 1"] = iAckNakInpLength1;
                record_item_j["Num RI Bits NRI (bits)"] = iNumRiBitsNri;

                // Mod Order / Gain / Cluster info from 'PUSCH Mod Order' field
                u_temp = record_item_j["PUSCH Mod Order"];
                int iPuschModOrder = u_temp & 3;
                int iPuschDigitalGain = (u_temp >> 2) & 255;
                int iStartRBCluster1 = (u_temp >> 10) & 255;
                int iNumRBCluster1 = (u_temp >> 18) & 63;
                record_item_j["PUSCH Mod Order"] = iPuschModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPuschModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : "(MI)Unknown");
                record_item_j["PUSCH Digital Gain (dB)"] = iPuschDigitalGain;
                record_item_j["Start RB Cluster1"] = iStartRBCluster1;
                record_item_j["Num RB Cluster1"] = iNumRBCluster1;

                // Tx Power / CQI Bits from 'PUSCH Tx Power (dBm)' field
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPuschTxPower = (u_temp & 127);
                int iNumCqiBits = (u_temp >> 7) & 255;
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;
                record_item_j["PUSCH Tx Power (dBm)"] = iPuschTxPower - 128;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                // DL Carriers / Ack Nack info from 'Num DL Carriers' field
                u_temp = record_item_j["Num DL Carriers"];
                int iNumDLCarriers = u_temp & 3;
                int iAckNackIndex = (u_temp >> 2) & 0xFFF;
                int iAckNackLate = (u_temp >> 14) & 1;
                int iCSFLate = (u_temp >> 15) & 1;
                int iDropPusch = (u_temp >> 16) & 1;
                record_item_j["Num DL Carriers"] = iNumDLCarriers;
                record_item_j["Ack Nack Index"] = iAckNackIndex;
                record_item_j["Ack Nack Late"] = iAckNackLate;
                record_item_j["CSF Late"] = iCSFLate;
                record_item_j["Drop PUSCH"] = iDropPusch;

                // CQI Payload handled by BYTE_STREAM (44 bytes in v124)

                // DMRS info from 'Cyclic Shift...' field
                u_temp = record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"];
                int iCSDSS0 = u_temp & 15;
                int iCSDSS1 = (u_temp >> 4) & 15;
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047;
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;
                record_item_j["DMRS Root Slot 0"] = iDMRSRootSlot0;
                record_item_j["DMRS Root Slot 1"] = iDMRSRootSlot1;


                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 144:
        case 145: // v145 record seems to be v144 minus DMRS roots at the end, handled by Fmt length difference
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v144,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Payload_v144, Fmt),
                                     b, offset, length, j);

            temp = j["Serving Cell ID"];
            int iServingCellId = temp & 511;
            int num_records = (temp >> 9) & 31;
            j["Number of Records"] = num_records;
            j["Serving Cell ID"] = iServingCellId;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                if (pkt_ver == 144) {
                    offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v144,
                                             ARRAY_SIZE(LtePhyPuschTxReport_Record_v144, Fmt),
                                             b, offset, length, record_item_j);
                } else { // v145
                    offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v145,
                                             ARRAY_SIZE(LtePhyPuschTxReport_Record_v145, Fmt),
                                             b, offset, length, record_item_j);
                }


                // UL Carrier Idx / ACK / CQI / RI etc from 'UL Carrier Index' field
                u_temp = record_item_j["UL Carrier Index"];
                int iULCarrierIndex = u_temp & 3;
                int iAck = (u_temp >> 2) & 1;
                int iCQI = (u_temp >> 3) & 1;
                int iRI = (u_temp >> 4) & 1;
                int iFrequencyHopping = (u_temp >> 5) & 3;
                int iRetxIndex = (u_temp >> 7) & 31;
                int iRedundVer = (u_temp >> 12) & 3;
                int iMirrorHopping = (u_temp >> 14) & 3;

                record_item_j["UL Carrier Index"] = iULCarrierIndex;
                const char* ul_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iULCarrierIndex);
                record_item_j["UL Carrier Index"] = (ul_carr_idx_name ? ul_carr_idx_name : "(MI)Unknown");
                record_item_j["ACK"] = iAck;
                const char* ack_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iAck);
                record_item_j["ACK"] = (ack_name ? ack_name : "(MI)Unknown");
                record_item_j["CQI"] = iCQI;
                const char* cqi_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iCQI);
                record_item_j["CQI"] = (cqi_name ? cqi_name : "(MI)Unknown");
                record_item_j["RI"] = iRI;
                const char* ri_name = search_name(ValueNameExistsOrNone, ARRAY_SIZE(ValueNameExistsOrNone, ValueName), iRI);
                record_item_j["RI"] = (ri_name ? ri_name : "(MI)Unknown");
                record_item_j["Frequency Hopping"] = iFrequencyHopping;
                const char* freqhop_name = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), iFrequencyHopping);
                record_item_j["Frequency Hopping"] = (freqhop_name ? freqhop_name : "(MI)Unknown");
                record_item_j["Re-tx Index"] = iRetxIndex;
                const char* retx_name = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), iRetxIndex);
                record_item_j["Re-tx Index"] = (retx_name ? retx_name : std::to_string(iRetxIndex));
                record_item_j["Redund Ver"] = iRedundVer;
                record_item_j["Mirror Hopping"] = iMirrorHopping;

                // RA Type / RB Info / DL Idx / DMRS OCC from 'Resource Allocation Type' field
                u_temp = record_item_j["Resource Allocation Type"];
                int iResourceAllocationType = u_temp & 1;
                int iStartRBSlot0 = (u_temp >> 1) & 127;
                int iStartRBSlot1 = (u_temp >> 8) & 127;
                int iNumOfRB = (u_temp >> 15) & 127;
                int iDLCarrierIndex = (u_temp >> 22) & 7; // 3 bits
                int iEnableULDMRSOCC = (u_temp >> 25) & 1; // 1 bit
                record_item_j["Resource Allocation Type"] = iResourceAllocationType;
                record_item_j["Start RB Slot 0"] = iStartRBSlot0;
                record_item_j["Start RB Slot 1"] = iStartRBSlot1;
                record_item_j["Num of RB"] = iNumOfRB;
                record_item_j["DL Carrier Index"] = iDLCarrierIndex;
                const char* dl_carr_idx_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), iDLCarrierIndex);
                record_item_j["DL Carrier Index"] = (dl_carr_idx_name ? dl_carr_idx_name : "(MI)Unknown");
                record_item_j["Enable UL DMRS OCC"] = iEnableULDMRSOCC;

                // Coding Rate
                temp = record_item_j["Coding Rate"];
                record_item_j["Coding Rate"] = (double)temp / 1024.0;

                // RM ACK / RI Payload / NumRI from 'Rate Matched ACK Bits' field
                u_temp = record_item_j["Rate Matched ACK Bits"];
                int iRateMatchedAckBits = u_temp & 16383;   // 14 bits
                int iRiPayload_val = (u_temp >> 14) & 16383; // 14 bits in v144
                int iNumRiBitsNri = (u_temp >> 28) & 0xF;    // 4 bits in v144
                record_item_j["Rate Matched ACK Bits"] = iRateMatchedAckBits;
                record_item_j["RI Payload"] = format_hex(iRiPayload_val, 4); // Approx 4 hex digits
                record_item_j["Num RI Bits NRI"] = iNumRiBitsNri;

                // ACK Lengths / RM RI / Reserved from 'ACK/NAK Inp Length 0' field
                u_temp = record_item_j["ACK/NAK Inp Length 0"];
                int iAckNakInpLength0 = u_temp & 0x7F;         // 7 bits
                int iAckNakInpLength1 = (u_temp >> 7) & 0x7F;  // 7 bits
                int iRateMatchedRiBits = (u_temp >> 14) & 0x7FF; // 11 bits
                int iReserved = (u_temp >> 25) & 0x7F;         // 7 bits
                record_item_j["ACK/NAK Inp Length 0"] = iAckNakInpLength0;
                record_item_j["ACK/NAK Inp Length 1"] = iAckNakInpLength1;
                record_item_j["Rate Matched RI Bits"] = iRateMatchedRiBits;
                record_item_j["Reserved"] = iReserved;

                // UE SRS / SRS Occ / Mod Order etc. from 'UE SRS' field
                u_temp = record_item_j["UE SRS"];
                int iUESRS = u_temp & 1;
                int iSrsOccasion = (u_temp >> 1) & 1;
                int iPuschModOrder = (u_temp >> 2) & 7;      // 3 bits
                int iPuschDigitalGain = (u_temp >> 5) & 255; // 8 bits
                int iStartRBCluster1 = (u_temp >> 13) & 0x7F; // 7 bits
                int iNumRBCluster1 = (u_temp >> 20) & 0x7F;   // 7 bits
                record_item_j["UE SRS"] = iUESRS;
                const char* uesrs_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iUESRS);
                record_item_j["UE SRS"] = (uesrs_name ? uesrs_name : "(MI)Unknown");
                record_item_j["SRS Occasion"] = iSrsOccasion;
                const char* srs_occ_name = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), iSrsOccasion);
                record_item_j["SRS Occasion"] = (srs_occ_name ? srs_occ_name : "(MI)Unknown");
                record_item_j["PUSCH Mod Order"] = iPuschModOrder;
                const char* mod_name = search_name(ValueNameModulation, ARRAY_SIZE(ValueNameModulation, ValueName), iPuschModOrder);
                record_item_j["PUSCH Mod Order"] = (mod_name ? mod_name : std::to_string(iPuschModOrder));
                record_item_j["PUSCH Digital Gain"] = iPuschDigitalGain;
                record_item_j["Start RB Cluster1"] = iStartRBCluster1;
                record_item_j["Num RB Cluster1"] = iNumRBCluster1;

                // Tx Power / CQI Bits from 'PUSCH Tx Power (dBm)' field
                u_temp = record_item_j["PUSCH Tx Power (dBm)"];
                int iPuschTxPower = (u_temp & 127);
                int iNumCqiBits = (u_temp >> 7) & 255;
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;
                record_item_j["PUSCH Tx Power (dBm)"] = iPuschTxPower - 128;
                record_item_j["Num CQI Bits"] = iNumCqiBits;
                record_item_j["Rate Matched CQI Bits"] = iRateMatchedCqiBits;

                // DL Carriers / Ack Nack info from 'Num DL Carriers' field
                u_temp = record_item_j["Num DL Carriers"];
                int iNumDLCarriers = u_temp & 3;
                int iAckNackIndex = (u_temp >> 2) & 0xFFF;
                int iAckNackLate = (u_temp >> 14) & 1;
                int iCSFLate = (u_temp >> 15) & 1;
                int iDropPusch = (u_temp >> 16) & 1;
                record_item_j["Num DL Carriers"] = iNumDLCarriers;
                record_item_j["Ack Nack Index"] = iAckNackIndex;
                record_item_j["Ack Nack Late"] = iAckNackLate;
                record_item_j["CSF Late"] = iCSFLate;
                record_item_j["Drop PUSCH"] = iDropPusch;

                // DMRS info from 'Cyclic Shift...' field
                u_temp = record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"];
                int iCSDSS0 = u_temp & 15;
                int iCSDSS1 = (u_temp >> 4) & 15;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 0 (Samples)"] = iCSDSS0;
                record_item_j["Cyclic Shift of DMRS Symbols Slot 1 (Samples)"] = iCSDSS1;


                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PUSCH Tx Report version: %d", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_PUSCH_TX_REPORT_H
