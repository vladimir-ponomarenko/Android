#ifndef SIGNAL_LOG_PACKET_H
#define SIGNAL_LOG_PACKET_H

#include <string>
#include <sstream>
#include "consts.h"

// Field types
enum FmtType {
    UINT,                       // Little endian. len = 1, 2, 4, 8
    BYTE_STREAM,                // A stream of bytes.
    BYTE_STREAM_LITTLE_ENDIAN,  // a stream of bytes in little endian
    QCDM_TIMESTAMP,             // Timestamp in all messages. len = 8
    PLMN_MK1,                   // in WCDMA Cell ID
    PLMN_MK2,                   // in LTE NAS EMM State
    BANDWIDTH,                  // in LTE RRC Serving Cell Info, LTE RRC MIB Message
    RSRP,
    RSRQ,
    SKIP,                       // This field is ignored (but bytes are consumed)
    PLACEHOLDER,                // This field is created with a dummy value (no byte is consumed)
    WCDMA_MEAS,                 // Used for RSCP/RSSI/ECN0 in LTE_PHY_IRAT_MDB
};

struct Fmt {
    FmtType type;
    const char *field_name;
    int len;                    // Some FmtType has restrictions on this field.
};

const Fmt LogPacketHeaderFmt [] = {
        {SKIP, NULL, 2},
        {UINT, "log_msg_len", 2},
        {UINT, "type_id", 2},
        {QCDM_TIMESTAMP, "timestamp", 8}
};

const ValueName ValueNameTrueOrFalse[] = {
        {0, "False"},
        {1, "True"}
};

const ValueName ValueNameCellIndex [] = {
        // 4 bits
        {0, "PCell"},
        {1, "SCell"},
};

const ValueName ValueNameBandClassGSM [] = {
        // 4 bits
        {0, "Current 900/1800 Setting"},
        {10, "1900 PCS"},
        {11, "GSM 850"},
};

const ValueName ValueNameTimerState [] = {
        // 8 bits
        {0, "Stopped"},
        {1, "Running"},
        {2, "Expired"},
};

const ValueName ValueNameEnableOrDisable [] = {
        {0, "Disabled"},
        {1, "Enabled"},
};

const ValueName ValueNameYesOrNo [] = {
        {0, "No"},
        {1, "Yes"},
};

const ValueName RNTIType[] = {
{0, "C-RNTI"},
{2, "P-RNTI"},
{3, "RA-RNTI"},
{4, "Temporary-C-RNTI"},
{5, "SI-RNTI"}
};

const ValueName ValueNameNumber[] = {
        {0, "First"},
        {1, "Second"},
        {2, "Third"},
        {3, "Fourth"},
        {4, "Fifth"},
        {5, "Sixth"},
        {6, "Seventh"},
        {7, "Eighth"},
};

const ValueName ValueNameCarrierIndex[] = {
        // 4 bits
        {0, "PCC"},
        {1, "SCC"},
        {2, "SCC-2"},
};

const ValueName ValueNameRNTIType[] = {
        // 4 bits
        {0, "C-RNTI"},
        {1, "SPS-RNTI"},
        {2, "P-RNTI"},
        {3, "RA-RNTI"},
        {4, "Temporary-C-RNTI"},
        {5, "SI-RNTI"},
        {6, "TPC-PUSCH-RNTI"},
        {7, "TPC-PUCCH-RNTI"},
        {8, "MBMS RNTI"},
};

const ValueName ValueNameHARQLogStatus[] = {
        {0, "Normal"},
};

const ValueName ValueNamePassOrFail[] = {
        {0, "Fail"},
        {1, "Pass"},
};

const ValueName ValueNameCompandingStats[] = {
        {0, "3 bit LLR"},
        {1, "4 bit LLR"},
        {2, "6 bit LLR"},
};


// LTE RLC UL Statistics

const Fmt LteRlcUlStats_Fmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num Subpkt", 1},
        {SKIP, NULL,         2},
};

const Fmt LteRlcUlStats_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcUlStats_SubpktPayload[] = {
        {UINT, "Num RBs",           1},
        {UINT, "RLCUL Error Count", 4},
};

const Fmt LteRlcUlStats_Subpkt_RB_Fmt[] = {
        {UINT, "Rb Cfg Idx",                  1},
        {UINT, "Mode",                        1},
        {UINT, "Num RST",                     4},
        {UINT, "Num New Data PDU",            4},
        {UINT, "Num New Data PDU Bytes",      4},
        {UINT, "Num SDU",                     4},
        {UINT, "Num SDU Bytes",               4},
        {UINT, "Num Ctrl PDU Tx",             4},
        {UINT, "Num Ctrl PDU Bytes Tx",       4},
        {UINT, "Num Retx PDU",                4},
        {UINT, "Num Retx PDU Bytes",          4},
        {UINT, "Num Ctrl PDU Rx",             4},
        {UINT, "Num Complete NACK",           4},
        {UINT, "Num Segm NACK",               4},
        {UINT, "Num Invalid Ctrl PDU Rx",     4},
        {UINT, "Num Poll",                    4},
        {UINT, "Num T Poll Retx Expiry",      4},
        {SKIP, NULL,                          4},
        {UINT, "Num New Data PDU Rst",        4},
        {UINT, "Num New Data PDU Bytes Rst",  4},
        {UINT, "Num SDU Rst",                 4},
        {UINT, "Num SDU Bytes Rst",           4},
        {UINT, "Num Ctrl PDU Tx Rst",         4},
        {UINT, "Num Ctrl PDU Bytes Tx Rst",   4},
        {UINT, "Num Retx PDU Rst",            4},
        {UINT, "Num Retx PDU Bytes Rst",      4},
        {UINT, "Num Ctrl PDU Rx Rst",         4},
        {UINT, "Num Complete NACK Rst",       4},
        {UINT, "Num Segm NACK Rst",           4},
        {UINT, "Num Invalid Ctrl PDU Rx Rst", 4},
        {UINT, "Num Poll Rst",                4},
        {UINT, "Num T Poll Retx Expiry Rst",  4},
        {SKIP, NULL,                          4},
};
const ValueName LteRlcUlStats_Subpkt_RB_Mode[] = {
        {1, "AM"},
};


// LTE RLC DL Statistics

const Fmt LteRlcDlStats_Fmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num Subpkt", 1},
        {SKIP, NULL,         2},
};

const Fmt LteRlcDlStats_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcDlStats_SubpktPayload_v2[] = {
        {UINT, "Num RBs",               1},
        {UINT, "RLC PDCP Q Full Count", 4},
        {UINT, "RLCDL Error Count",     4},
};

const Fmt LteRlcDlStats_SubpktPayload_v3[] = {
        {UINT, "Num RBs",               1},
        {UINT, "RLC PDCP Q Full Count", 4},
        {UINT, "RLCDL Error Count",     4},
};

const Fmt LteRlcDlStats_Subpkt_RB_Fmt_v2[] = {
        {UINT, "Rb Cfg Idx",                1},
        {UINT, "Mode",                      1},
        {UINT, "Num RST",                   4},
        {UINT, "Num Data PDU",              4},
        {UINT, "Data PDU Bytes",            4},
        {UINT, "Num Status Rxed",           4},
        {UINT, "Status Rxed Bytes",         4},
        {UINT, "Num Invalid PDU",           4},
        {UINT, "Invalid PDU Bytes",         4},
        {UINT, "Num Retx PDU",              4},
        {UINT, "Retx PDU Bytes",            4},
        {UINT, "Num Dup PDU",               4},
        {UINT, "Dup PDU Bytes",             4},
        {UINT, "Num Dropped PDU",           4},
        {UINT, "Dropped PDU Bytes",         4},
        {UINT, "Num Dropped PDU FC",        4},
        {UINT, "Dropped PDU Bytes FC",      4},
        {UINT, "Num SDU",                   4},
        {UINT, "Num SDU Bytes",             4},
        {UINT, "Num NonSeq SDU",            4},
        {UINT, "Num Ctrl PDU",              4},
        {UINT, "Num Complete NACK",         4},
        {UINT, "Num Segments NACK",         4},
        {UINT, "Num t_reorder Expired",     4},
        {SKIP, NULL,                        4},
        {UINT, "Num Data PDU Rst",          4},
        {UINT, "Data PDU Bytes Rst",        4},
        {UINT, "Num Status Rxed Rst",       4},
        {UINT, "Status Rxed Bytes Rst",     4},
        {UINT, "Num Invalid PDU Rst",       4},
        {UINT, "Invalid PDU Bytes Rst",     4},
        {UINT, "Num Retx PDU Rst",          4},
        {UINT, "Retx PDU Bytes Rst",        4},
        {UINT, "Num Dup PDU Rst",           4},
        {UINT, "Dup PDU Bytes Rst",         4},
        {UINT, "Num Dropped PDU Rst",       4},
        {UINT, "Dropped PDU Bytes Rst",     4},
        {UINT, "Num Dropped PDU FC Rst",    4},
        {UINT, "Dropped PDU Bytes FC Rst",  4},
        {UINT, "Num SDU Rst",               4},
        {UINT, "Num SDU Bytes Rst",         4},
        {UINT, "Num NonSeq SDU Rst",        4},
        {UINT, "Num Ctrl PDU Rst",          4},
        {UINT, "Num Complete NACK Rst",     4},
        {UINT, "Num Segments NACK Rst",     4},
        {UINT, "Num t_reorder Expired Rst", 4},
        {SKIP, NULL,                        4},
};

const Fmt LteRlcDlStats_Subpkt_RB_Fmt_v3[] = {
        {UINT, "Rb Cfg Idx",                1},
        {UINT, "Mode",                      1},
        {UINT, "Num RST",                   4},
        {UINT, "Num Data PDU",              4},
        {UINT, "Data PDU Bytes",            8},
        {UINT, "Num Status Rxed",           4},
        {UINT, "Status Rxed Bytes",         8},
        {UINT, "Num Invalid PDU",           4},
        {UINT, "Invalid PDU Bytes",         8},
        {UINT, "Num Retx PDU",              4},
        {UINT, "Retx PDU Bytes",            8},
        {UINT, "Num Dup PDU",               4},
        {UINT, "Dup PDU Bytes",             8},
        {UINT, "Num Dropped PDU",           4},
        {UINT, "Dropped PDU Bytes",         8},
        {UINT, "Num Dropped PDU FC",        4},
        {UINT, "Dropped PDU Bytes FC",      8},
        {UINT, "Num SDU",                   4},
        {UINT, "Num SDU Bytes",             8},
        {UINT, "Num NonSeq SDU",            4},
        {UINT, "Num Ctrl PDU",              4},
        {UINT, "Num Complete NACK",         4},
        {UINT, "Num Segments NACK",         4},
        {UINT, "Num t_reorder Expired",     4},
        {UINT, "Num t_reorder Start",       4},
        {UINT, "Num Missed UM PDU",         4},
        {SKIP, NULL,                        4},
        {UINT, "Num Data PDU Rst",          4},
        {UINT, "Data PDU Bytes Rst",        8},
        {UINT, "Num Status Rxed Rst",       4},
        {UINT, "Status Rxed Bytes Rst",     8},
        {UINT, "Num Invalid PDU Rst",       4},
        {UINT, "Invalid PDU Bytes Rst",     8},
        {UINT, "Num Retx PDU Rst",          4},
        {UINT, "Retx PDU Bytes Rst",        8},
        {UINT, "Num Dup PDU Rst",           4},
        {UINT, "Dup PDU Bytes Rst",         8},
        {UINT, "Num Dropped PDU Rst",       4},
        {UINT, "Dropped PDU Bytes Rst",     8},
        {UINT, "Num Dropped PDU FC Rst",    4},
        {UINT, "Dropped PDU Bytes FC Rst",  8},
        {UINT, "Num SDU Rst",               4},
        {UINT, "Num SDU Bytes Rst",         8},
        {UINT, "Num NonSeq SDU Rst",        4},
        {UINT, "Num Ctrl PDU Rst",          4},
        {UINT, "Num Complete NACK Rst",     4},
        {UINT, "Num Segments NACK Rst",     4},
        {UINT, "Num t_reorder Expired Rst", 4},
        {UINT, "Num t_reorder Start Rst",   4},
        {UINT, "Num Missed UM PDU Rst",     4},
        {SKIP, NULL,                        4},
};
const ValueName LteRlcDlStats_Subpkt_RB_Mode[] = {
        {1, "AM"},
};


const Fmt LtePuschPowerControl_Fmt[] = {
        {UINT, "Version", 1},
};

const Fmt LtePuschPowerControl_Fmt_v4[] = {
        {SKIP, NULL,                2},
        {UINT, "Number of Records", 1},
};

const Fmt LtePuschPowerControl_Fmt_v5[] = {
        {SKIP, NULL,                2},
        {UINT, "Number of Records", 1},
};

const Fmt LtePuschPowerControl_Fmt_v25[] = {
        {SKIP, NULL,                2},
        {UINT, "Number of Records", 1},
};

const Fmt LtePuschPowerControl_Record_Fmt_v4[] = {
        {UINT,        "SFN",                   4},
        {PLACEHOLDER, "Sub-FN",                0},
        {PLACEHOLDER, "PUSCH Tx Power (dBm)",  0},
        {PLACEHOLDER, "DCI Format",            0},
        {PLACEHOLDER, "Tx Type",               0},
        {UINT,        "Transport Block Size",  4},
        {PLACEHOLDER, "DL Path Loss",          0},
        {PLACEHOLDER, "F(i)",                  0},
        {UINT,        "TPC",                   4},
        {PLACEHOLDER, "PUSCH Actual Tx Power", 0},
};
const Fmt LtePuschPowerControl_Record_Fmt_v5[] = {
        {UINT,        "SFN",                   4},
        {PLACEHOLDER, "Sub-FN",                0},
        {PLACEHOLDER, "PUSCH Tx Power (dBm)",  0},
        {PLACEHOLDER, "DCI Format",            0},
        {PLACEHOLDER, "Tx Type",               0},
        {UINT,        "Num RBs",               4},
        {PLACEHOLDER, "Transport Block Size",  0},
        {PLACEHOLDER, "DL Path Loss",          0},
        {UINT,        "F(i)",                  4},
        {PLACEHOLDER, "TPC",                   0},
        {PLACEHOLDER, "PUSCH Actual Tx Power", 0},
};

const Fmt LtePuschPowerControl_Record_Fmt_v25[] = {
        {UINT,        "SFN",                   4},
        {PLACEHOLDER, "Cell Index",            0},
        {PLACEHOLDER, "Sub-FN",                0},
        {PLACEHOLDER, "PUSCH Tx Power (dBm)",  0},
        {PLACEHOLDER, "DCI Format",            0},
        {PLACEHOLDER, "Tx Type",               0},
        {PLACEHOLDER, "TPC Frozen",            0},
        {UINT,        "Num RBs",               4},
        {PLACEHOLDER, "Transport Block Size",  0},
        {PLACEHOLDER, "DL Path Loss",          0},
        {UINT,        "F(i)",                  4},
        {PLACEHOLDER, "TPC",                   0},
        {PLACEHOLDER, "PUSCH Actual Tx Power", 0},
        {PLACEHOLDER, "Max Power",             0},
};

const ValueName LtePuschPowerControl_Record_v5_TxType[] = {
        {1, "Dynamic"},
        {2, "RACH MSG3"},
};
const ValueName LtePuschPowerControl_Record_v5_DCI_Format[] = {
        // Release 8
        // http:    //www.sharetechnote.com/html/LTE_Advanced_DCI.html
        {0,  "Format 0"},
        {1,  "Format 1"},
        {2,  "Format 1A"},
        {3,  "Format 1B"},
        {4,  "Format 1C"},
        {5,  "Format 1D"},
        {6,  "Format 2"},
        {7,  "Format 2A"},
        // {8, "Format 2B"},
        // {9, "Format 2C"},
        {10, "Format 3"},
        {11, "Format 3A"},
        // {12, "Format 4"},
};
const ValueName LtePuschPowerControl_Record_v5_TPC[] = {
        {15, "N/A"},
        {31, "-1"},
};


const Fmt LtePhyPdschDemapperConfigFmt[] = {
        {UINT, "Version", 1}
};

const Fmt LtePhyPdschDemapperConfigFmt_v23[] = {
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "Frequency Selective PMI",  1},    // right shift 1 bit, 2 bits
        {PLACEHOLDER,               "PMI Index",                0},    // 4 bits
        {UINT,                      "Transmission Scheme",      1},    // 4 bits
        {SKIP,                      NULL,                       2},
        // {UINT, "Transport Block Size Stream 0", 2},
        {UINT,                      "TBS 0",                    2},
        // {UINT, "Modulation Stream 0", 2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        // {UINT, "Transport Block Size Stream 1", 2},
        {UINT,                      "TBS 1",                    2},
        // {UINT, "Modulation Stream 1", 2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "Carrier Index",            0}
};

const Fmt LtePhyPdschDemapperConfigFmt_v28[] = {
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "Frequency Selective PMI",  1},    // right shift 1 bit, 2 bits
        {PLACEHOLDER,               "PMI Index",                0},    // 4 bits
        {UINT,                      "Transmission Scheme",      1},    // 4 bits

        {UINT,                      "Repetition Index Data",    2},

        // {UINT, "Transport Block Size Stream 0", 2},
        {UINT,                      "TBS 0",                    2},
        // {UINT, "Modulation Stream 0", 2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        // {UINT, "Transport Block Size Stream 1", 2},
        {UINT,                      "TBS 1",                    2},
        // {UINT, "Modulation Stream 1", 2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "Carrier Index",            0},

        //skip PB,CSI_RS,ZP CSI-RS,CSI-RS

        {UINT,                      "Repetition Total",         4},
        {PLACEHOLDER,               "NB Index",                 0},
        {PLACEHOLDER,               "SIB1-BR Collision"},
        {PLACEHOLDER,               "SIBx Collision"},
};

const Fmt LtePhyPdschDemapperConfigFmt_v103[] = {
        // TODO: just copy from v23.
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "Frequency Selective PMI",  1},    // right shift 1 bit, 2 bits
        {PLACEHOLDER,               "PMI Index",                0},    // 4 bits
        {UINT,                      "Transmission Scheme",      1},    // 4 bits
        {SKIP,                      NULL,                       2},
        {UINT,                      "TBS 0",                    2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        {UINT,                      "TBS 1",                    2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "Carrier Index",            0},
        {SKIP,                      NULL,                       4},
};


const Fmt LtePhyPdschDemapperConfigFmt_v104[] = {
        // TODO: just copy from v23.
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "Frequency Selective PMI",  1},    // right shift 1 bit, 2 bits
        {PLACEHOLDER,               "PMI Index",                0},    // 4 bits
        {UINT,                      "Transmission Scheme",      1},    // 4 bits
        {SKIP,                      NULL,                       2},
        {UINT,                      "TBS 0",                    2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        {UINT,                      "TBS 1",                    2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "Carrier Index",            0},
        {SKIP,                      NULL,                       4},
};

const Fmt LtePhyPdschDemapperConfigFmt_v122[] = {
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {PLACEHOLDER,               "Frequency Selective PMI",  0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "PMI Index",                2},
        {PLACEHOLDER,               "Transmission Scheme",      0},
        {PLACEHOLDER,               "BMOD FD Sym Index",        0},
        {SKIP,                      NULL,                       1},
        {UINT,                      "MVC",                      1},
        {PLACEHOLDER,               "MVC Clock",                0},
        {PLACEHOLDER,               "MVC Request Up",           0},
        {UINT,                      "TBS 0",                    2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        {UINT,                      "TBS 1",                    2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "PB",                       0},
        {PLACEHOLDER,               "Carrier Index",            0},
        {PLACEHOLDER,               "CSI-RS Exist",             0},
        {PLACEHOLDER,               "ZP CSI-RS Exist",          0},
        {PLACEHOLDER,               "CSI-RS Symbol Skipped",    0},
        {UINT,                      "Op Mode",                  4},
        {PLACEHOLDER,               "Strong ICell ID",          0},
        {PLACEHOLDER,               "Joint Demod Skip Reason",  0},
};

const Fmt LtePhyPdschDemapperConfigFmt_v123[] = {
        {UINT,                      "Serving Cell ID",          1},
        {UINT,                      "System Frame Number",      2},
        {PLACEHOLDER,               "Subframe Number",          0},
        {UINT,                      "PDSCH RNTIl ID",           2},
        {UINT,                      "Number of Tx Antennas(M)", 2},
        {PLACEHOLDER,               "PDSCH RNTI Type",          0},
        {PLACEHOLDER,               "Number of Rx Antennas(N)", 0},
        {PLACEHOLDER,               "Spatial Rank",             0},
        {PLACEHOLDER,               "Frequency Selective PMI",  0},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]",  8},
        {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]",  8},
        {UINT,                      "PMI Index",                2},
        {PLACEHOLDER,               "Transmission Scheme",      0},
        {PLACEHOLDER,               "BMOD FD Sym Index",        0},
        {SKIP,                      NULL,                       2},
        {UINT,                      "TBS 0",                    2},
        {UINT,                      "MCS 0",                    2},
        {PLACEHOLDER,               "Traffic to Pilot Ratio",   0},
        {UINT,                      "TBS 1",                    2},
        {UINT,                      "MCS 1",                    2},
        {PLACEHOLDER,               "PB",                       0},
        {PLACEHOLDER,               "Carrier Index",            0},
        {PLACEHOLDER,               "CSI-RS Exist",             0},
        {PLACEHOLDER,               "ZP CSI-RS Exist",          0},
        {PLACEHOLDER,               "CSI-RS Symbol Skipped",    0},
        {UINT,                      "Op Mode",                  4},
        {PLACEHOLDER,               "Strong ICell ID",          0},
        {PLACEHOLDER,               "Joint Demod Skip Reason",  0},
};

const Fmt LtePhyPdschDemapperConfigFmt_header_144[] = {
        {UINT,        "Carrier Index",  4},    // shift 0 bits,total 4 bits
        {PLACEHOLDER, "Num of Records", 0},    // shift 4 bits,total 8 bits
        {SKIP,        NULL,             3},
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 12 bits,total 44 bits
};

const Fmt LtePhyPdschDemapperConfigFmt_v144[] = {
        {UINT,        "Serving Cell ID",               4},    // shift 0 bits,total 9 bits
        {PLACEHOLDER, "Sub-frame Number",              0},    // shift 9 bits,total 4 bits
        {PLACEHOLDER, "System Frame Number",           0},    // shift 13 bits,total 10 bits
        {PLACEHOLDER, "PDSCH RNTI Type",               0},    // shift 23 bits,total 4 bits
        {PLACEHOLDER, "Number of Tx Antennas (M)",     0},    // shift 27 bits,total 2 bits
        {PLACEHOLDER, "Number of Rx Antennas (N)",     0},    // shift 29 bits,total 2 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 31 bits,total 1 bits

        {UINT,        "PDSCH RNTIl ID",                2},    // shift 0 bits,total 16 bits
        //{UINT,"Reserved",1}, 	    // shift 0 bits,total 1 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 1 bits,total 1 bits
        {UINT,        "Spatial Rank",                  1},    // shift 2 bits,total 2 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 4 bits,total 1 bits
        {PLACEHOLDER, "Frequency Selective PMI",       0},    // shift 5 bits,total 1 bits
        {PLACEHOLDER, "MU Receiver Mode",              0},    // shift 6 bits,total 2 bits

        {UINT,        "PMI Index",                     1},    // shift 0 bits,total 4 bits
        {PLACEHOLDER, "Transmission Scheme",           0},    // shift 4 bits,total 4 bits

        {UINT,        "RB Allocation Slot 0[0]",       8},    // shift 0 bits,total 64 bits
        {UINT,        "RB Allocation Slot 0[1]",       8},    // shift 0 bits,total 64 bits
        {UINT,        "RB Allocation Slot 1[0]",       8},    // shift 0 bits,total 64 bits
        {UINT,        "RB Allocation Slot 1[1]",       8},    // shift 0 bits,total 64 bits
        {UINT,        "UERS Port Enabled",             4},    // shift 0 bits,total 3 bits
        {PLACEHOLDER, "BMOD FD Sym Index",             0},    // shift 3 bits,total 4 bits
        {PLACEHOLDER, "Transport Block Size Stream 0", 0},    // shift 7 bits,total 18 bits
        {PLACEHOLDER, "Modulation Stream 0",           0},    // shift 25 bits,total 2 bits
        {PLACEHOLDER, "PB",                            0},    // shift 27 bits,total 2 bits
        {PLACEHOLDER, "RhoB/RhoA",                     0},    // shift 29 bits,total 2 bits
        {PLACEHOLDER, "CSI-RS Exist",                  0},    // shift 31 bits,total 1 bits

        {UINT,        "ZP CSI-RS Exist",               4},    // shift 0 bits,total 1 bits
        {PLACEHOLDER, "CSI-RS Symbol Skipped",         0},    // shift 1 bits,total 1 bits
        {PLACEHOLDER, "Traffic to Pilot Ratio Data",   0},    // shift 2 bits,total 12 bits
        {PLACEHOLDER, "Transport Block Size Stream 1", 0},    // shift 14 bits,total 18 bits

        {UINT,        "Modulation Stream 1",           4},    // shift 0 bits,total 2 bits
        {PLACEHOLDER, "SCH0 Memory Map Mode",          0},    // shift 2 bits,total 2 bits
        {PLACEHOLDER, "SCH1 Memory Map Mode",          0},    // shift 4 bits,total 2 bits
        {PLACEHOLDER, "Strong ICell ID",               0},    // shift 6 bits,total 9 bits
        {PLACEHOLDER, "Qice Enable Mode",              0},    // shift 15 bits,total 2 bits
        {PLACEHOLDER, "Qice Skip Reason",              0},    // shift 17 bits,total 3 bits
        {PLACEHOLDER, "Csf Dual Rnn Sel",              0},    // shift 20 bits,total 1 bits
        {PLACEHOLDER, "Plq Num Enabled Rd Groups",     0},    // shift 21 bits,total 5 bits
        {PLACEHOLDER, "Plg Num Loaded Rb Groups",      0},    // shift 26 bits,total 5 bits
        {PLACEHOLDER, "Qed Mode",                      0},    // shift 31 bits,total 3 bits

        {SKIP,        NULL,                            4},
        //{UINT,"Reserved",4}, 	    // shift 2 bits,total 8 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 10 bits,total 8 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 18 bits,total 8 bits
        //{PLACEHOLDER,"Reserved",0}, 	    // shift 26 bits,total 8 bits
};

const ValueName LtePhyPdschDemapperConfig_v23_Modulation[] = {
        {0, "QPSK"},
        {1, "16QAM"},
        {2, "64QAM"},
        {3, "256QAM"},
};

const ValueName LtePhyPdschDemapperConfig_v23_Carrier_Index[] = {
        {0, "PCC"},
        {1, "SCC"}
};

const ValueName LtePhyPdschDemapperConfig_v122_antenna[] = {
        {0, "1 antenna"},
        {1, "2 antennas"},
        {2, "4 antennas"},
        {3, "4 antennas"},
};

const ValueName LtePhyPdschDemapperConfig_v122_MVC_Clock[] = {
        {0, "MIN SVS"},
};

const ValueName LtePhyPdschDemapperConfig_v122_OPMode[] = {
        {1, "TILE0_4RX"},
};

const ValueName LtePhyPdschDemapperConfig_Joint_Demod_Skip_Reason[] = {
        {0, "NO_SKIP"},
        {1, "SW_DISABLE"},
};

const ValueName ValueNameRankIndex[] = {
        {0, "Rank 1"},
        {1, "Rank 2"},
        {2, "Rank 3"},
        {3, "Rank 4"},
        {4, "Rank 5"},
        {5, "Rank 6"},
        {6, "Rank 7"},
        {7, "Rank 8"}

};

const ValueName ValueNameFrequencySelectivePMI[] = {
        {0, "WideBand"},
};

const ValueName ValueNameTransmissionScheme[] = {
        // 4 bits
        {1, "Single Antenna Port (SISO or SIMO)"},
        {2, "Transmit diversity"},
        {3, "Open-loop spatial multiplexing"},
        {4, "Closed-loop spatial multiplexing"},
        {5, "Multi-User MIMO"},
        {6, "Closed-loop rank-1 spatial multiplexing"},
        {7, "Single Antenna Port Beamforming"},
        {8, "Dual-Layer Beamforming"},
};


// LTE NAS EMM State
const Fmt LteNasEmmStateFmt[] = {
        {UINT, "Version", 1}
};

const Fmt LteNasEmmStateFmt_v2[] = {
        {UINT,        "EMM State",         1},
        {UINT,        "EMM Substate",      2},
        {PLMN_MK2,    "PLMN",              3},
        {UINT,        "GUTI Valid",        1},
        {UINT,        "GUTI UE Id",        1},
        {PLMN_MK2,    "GUTI PLMN",         3},
        {BYTE_STREAM, "GUTI MME Group ID", 2},
        {BYTE_STREAM, "GUTI MME Code",     1},
        {BYTE_STREAM, "GUTI M-TMSI",       4}
};

const Fmt LteNasEsmStateFmt[] = {
        {UINT, "Version", 1}
};

const Fmt LteNasEsmStateFmt_v1[] = {
        {UINT, "EPS bearer type",  1},
        {UINT, "EPS bearer ID",    1},
        {UINT, "EPS bearer state", 1},
        {SKIP, NULL,               6},
        {UINT, "Len",              1},
        {UINT, "QCI",              1},
        {UINT, "UL MBR",           1},
        {UINT, "DL MBR",           1},
        {UINT, "UL GBR",           1},
        {UINT, "DL GBR",           1},
        {UINT, "UL MBR ext",       1},
        {UINT, "DL MBR ext",       1},
        {UINT, "UL GBR ext",       1},
        {UINT, "DL GBR ext",       1},
};

const ValueName LteNasEmmState_v2_EmmState[] = {
        {0, "EMM_NULL"},    // No substate
        {1, "EMM_DEREGISTERED"},    // Substate table: Deregistered
        {2, "EMM_REGISTERED_INITIATED"},    // Substate table: Registered_Initiated
        {3, "EMM_REGISTERED"},    // Substate table: Registered
        {4, "EMM_TRACKING_AREA_UPDATING_INITIATED"},    // The same as above
        {5, "EMM_SERVICE_REQUEST_INITIATED"},    // The same as above
        {6, "EMM_DEREGISTERED_INITIATED"}    // No substate
};

const ValueName LteNasEmmState_v2_EmmSubstate_Deregistered[] = {
        {0, "EMM_DEREGISTERED_NO_IMSI"},
        {1, "EMM_DEREGISTERED_PLMN_SEARCH"},
        {2, "EMM_DEREGISTERED_ATTACH_NEEDED"},
        {3, "EMM_DEREGISTERED_NO_CELL_AVAILABLE"},
        {4, "EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH"},
        {5, "EMM_DEREGISTERED_NORMAL_SERVICE"},
        {6, "EMM_DEREGISTERED_LIMITED_SERVICE"},
        {7, "EMM sub-state = 7"},
};

const ValueName LteNasEmmState_v2_EmmSubstate_Registered_Initiated[] = {
        {0, "EMM_WAITING_FOR_NW_RESPONSE"},
        {1, "EMM_WAITING_FOR_ESM_RESPONSE"}
};

const ValueName LteNasEmmState_v2_EmmSubstate_Registered[] = {
        {0, "EMM_REGISTERED_NORMAL_SERVICE"},
        {1, "EMM_REGISTERED_UPDATE_NEEDED"},
        {2, "EMM_REGISTERED_ATTEMPTING_TO_UPDATE"},
        {3, "EMM_REGISTERED_NO_CELL_AVAILABLE"},
        {4, "EMM_REGISTERED_PLMN_SEARCH"},
        {5, "EMM_REGISTERED_LIMITED_SERVICE"}
};

const Fmt LteMacRachAttempt_Fmt[] = {
        {UINT, "Version",              1},
        {UINT, "Number of Subpackets", 1},
        {SKIP, NULL,                   2},
};

const Fmt LteMacRachAttempt_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteMacRachAttempt_SubpktPayload[] = {
        // Version 2
        {UINT, "Retx counter",         1},
        {UINT, "Rach result",          1},
        {UINT, "Contention procedure", 1},
        {UINT, "Rach msg bmasks",      1},
};

const Fmt LteMacRachAttempt_SubpktPayload_v3[] = {
        // Version 3
        {UINT, "Sub Id",               1},
        {UINT, "Cell Id",              1},
        {UINT, "Retx counter",         1},
        {UINT, "Rach result",          1},
        {UINT, "Contention procedure", 1},
        {UINT, "Rach msg bmasks",      1},
};

const Fmt LteMacRachAttempt_SubpktPayload_v4[] = {
        // Version 2
        {UINT, "Retx counter",         1},
        {UINT, "Rach result",          1},
        {UINT, "Contention procedure", 1},
        {UINT, "Rach msg bmasks",      1},
};

const ValueName LteMacRachAttempt_Subpkt_RachResult[] = {
        {0, "Success"},
        {1, "Failure at MSG2"},
        {4, "Aborted"},
};
const ValueName LteMacRachAttempt_Subpkt_ContentionProcedure[] = {
        {0, "Contention Free RACH Procedure"},
        {1, "Contention Based RACH procedure"},
};

const Fmt LteMacRachAttempt_Subpkt_Msg1[] = {
        {UINT,        "Preamble Index",        1},
        {BYTE_STREAM, "Preamble index mask",   1},
        {WCDMA_MEAS,  "Preamble power offset", 1},
        {SKIP,        NULL,                    1},
};

const Fmt LteMacRachAttempt_Subpkt_Msg1_v3[] = {
        {UINT,        "Preamble Index",        1},
        {BYTE_STREAM, "Preamble index mask",   1},
        {UINT,        "Preamble power offset", 2},
};

const Fmt LteMacRachAttempt_Subpkt_Msg1_v4[] = {
        {UINT,        "Preamble Index",        1},
        {UINT,        "Preamble index mask",   1},
        {UINT,        "Preamble power offset", 2},
        {UINT,        "CE Level",              1},
};

const ValueName LteMacRachAttempt_Subpkt_Preamble_index_mask[] = {
        {0xff, "Invalid"},
};

const ValueName LteMacRachAttempt_Subpkt_CELEVEL[] = {
        {0, "CE_LEVEL_0"},
};

const Fmt LteMacRachAttempt_Subpkt_Msg2[] = {
        {UINT, "Backoff Value (ms)", 2},
        {UINT, "Result",             1},
        {UINT, "TCRNTI",             2},
        {UINT, "TA value",           2},
};
const ValueName LteMacRachAttempt_Subpkt_Msg2_Result[] = {
        {0, "No"},
        {1, "True"},
};

const Fmt LteMacRachAttempt_Subpkt_Msg3[] = {
        {SKIP,        NULL,        1},
        {BYTE_STREAM, "Grant Raw", 3},
        {UINT,        "Grant",     2},
        {UINT,        "Harq ID",   1},
};

const Fmt LteMacRachAttempt_Subpkt_Msg3_v4[] = {
        {UINT,        "Grant Raw", 4},
        {UINT,        "Grant",     2},
        {UINT,        "Harq ID",   1},
};

const Fmt LteMacRachAttempt_Subpkt_Msg3_MACPDU[] = {
        {BYTE_STREAM, "MAC PDU", 1},
};

const ValueName BSREvent[] = {
        {0, "None"},
        {1, "Periodic"},
        {2, "High Data Arrival"},
        {3, "Robustness BSR"},
};

const ValueName BSRTrig[] = {
        {0, "No BSR"},
        {1, "Cancelled"},
        {2, "L-BSR"},
        {3, "S-BSR"},
        {4, "Pad L-BSR"},
        {5, "Pad S-BSR"},
        {6, "Pad T-BSR"},
};

const Fmt LteMacULTransportBlockFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2}
};

const Fmt LteMacULTransportBlock_SubpktHeaderFmt[] = {
        {UINT, "SubPacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "SubPacket Size", 2},
        {UINT, "Num Samples",    1},
};

const Fmt LteMacULTransportBlock_SubpktV1_SampleFmt[] = {
        {UINT,        "HARQ ID",         1},
        {UINT,        "RNTI Type",       1},
        {UINT,        "Sub-FN",          2},    // 4 bits
        {PLACEHOLDER, "SFN",             0},
        {UINT,        "Grant (bytes)",   2},
        {UINT,        "RLC PDUs",        1},
        {UINT,        "Padding (bytes)", 2},
        {UINT,        "BSR event",       1},
        {UINT,        "BSR trig",        1},
        {UINT,        "HDR LEN",         1},
        // Mac Hdr + CE and UL TB Other Structure
};

const Fmt LteMacULTransportBlock_SubpktV2_SampleFmt[] = {
        {UINT,        "Sub Id",          1},
        {UINT,        "Cell Id",         1},
        {UINT,        "HARQ ID",         1},
        {UINT,        "RNTI Type",       1},
        {UINT,        "Sub-FN",          2},    // 4 bits
        {PLACEHOLDER, "SFN",             0},
        {UINT,        "Grant (bytes)",   2},
        {UINT,        "RLC PDUs",        1},
        {UINT,        "Padding (bytes)", 2},
        {UINT,        "BSR event",       1},
        {UINT,        "BSR trig",        1},
        {UINT,        "HDR LEN",         1},
};

const ValueName LteMacULTransportBlock_Mac_Hdr_LCId[] = {
        {0, "CCCH"},
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {4, "4"},
        {5, "5"},
        {6, "6"},
        {7, "7"},
        {8, "8"},
        {9, "9"},
        {10, "10"},
        {11, "CCCH (unsupported)"},
        {12, "CCCH (unsupported)"},
        {13, "CCCH and Extended PHR (unsupported)"},
        {14, "Reserved (unsupported)"},
        {15, "Reserved (unsupported)"},
        {16, "Extended LC ID field (unsupported)"},
        {17, "Reserved (unsupported)"},
        {18, "AUL confirmation (4 octets) (unsupported)"},
        {19, "AUL confirmation (1 octet) (unsupported)"},
        {20, "Recommended bit rate query"},
        {21, "SPS confirmation (unsupported)"},
        {22, "Truncated Sidelink BSR (unsupported)"},
        {23, "Sidelink BSR (unsupported)"},
        {24, "Dual Connectivity PHR (unsupported)"},
        {25, "Extended PHR (unsupported)"},
        {26, "PHR"},
        {27, "C-RNTI"},
        {28, "T-BSR"},
        {29, "S-BSR"},
        {30, "L-BSR"},
        {31, "Padding"},
};

const ValueName LteMacULTransportBlock_Mac_CE_RBRQ_ULorDL[] = {
        {0, "DL"},
        {1, "UL"},
};

const int LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[] = {
        0, 10, 12, 14, 17, 19, 22, 26,
        31, 36, 42, 49, 57, 67, 78, 91,
        107, 125, 146, 171, 200, 234, 274, 321,
        376, 440, 515, 603, 706, 826, 967, 1132,
        1326, 1552, 1817, 2127, 2490, 2915, 3413, 3995,
        4677, 5476, 6411, 7505, 8787, 10287, 12403, 14099,
        16507, 19325, 22624, 24687, 31009, 36304, 42502, 49759,
        58255, 68201, 79846, 93479, 109439, 128125, 150000, 0x7fffffff
};

const Fmt LteMacULTransportBlock_Mac_Hdr[] = {
        {UINT,        "Header Field",    1},
        {PLACEHOLDER, "LC ID",           0},
        {PLACEHOLDER, "Len",             0},
};

const Fmt LteMacULTransportBlock_Mac_Hdr_L1[] = {
        {UINT,        "L1 Field",        1},
};

const Fmt LteMacULTransportBlock_Mac_Hdr_L2[] = {
        {UINT,        "L2 Field",        1},
};

const Fmt LteMacULTransportBlock_Mac_CE_L_BSR[] = {
        {UINT,        "L-BSR Field 1",   1},
        {UINT,        "L-BSR Field 2",   1},
        {UINT,        "L-BSR Field 3",   1},
        {PLACEHOLDER, "BSR LCG 0",       0},
        {PLACEHOLDER, "BSR LCG 1",       0},
        {PLACEHOLDER, "BSR LCG 2",       0},
        {PLACEHOLDER, "BSR LCG 3",       0},
        {PLACEHOLDER, "BSR LCG 0 (bytes)", 0},
        {PLACEHOLDER, "BSR LCG 1 (bytes)", 0},
        {PLACEHOLDER, "BSR LCG 2 (bytes)", 0},
        {PLACEHOLDER, "BSR LCG 3 (bytes)", 0},
};

const Fmt LteMacULTransportBlock_Mac_CE_S_T_BSR[] = {
        {UINT,        "S/T-BSR Field",     1},
};

const Fmt LteMacULTransportBlock_Mac_CE_S_T_BSR_LCG0[] = {
        {PLACEHOLDER, "BSR LCG 0",       0},
        {PLACEHOLDER, "BSR LCG 0 (bytes)", 0},
};

const Fmt LteMacULTransportBlock_Mac_CE_S_T_BSR_LCG1[] = {
        {PLACEHOLDER, "BSR LCG 1",       0},
        {PLACEHOLDER, "BSR LCG 1 (bytes)", 0},
};

const Fmt LteMacULTransportBlock_Mac_CE_S_T_BSR_LCG2[] = {
        {PLACEHOLDER, "BSR LCG 2",       0},
        {PLACEHOLDER, "BSR LCG 2 (bytes)", 0},
};

const Fmt LteMacULTransportBlock_Mac_CE_S_T_BSR_LCG3[] = {
        {PLACEHOLDER, "BSR LCG 3",       0},
        {PLACEHOLDER, "BSR LCG 3 (bytes)", 0},
};

const Fmt LteMacULTransportBlock_Mac_CE_C_RNTI[] = {
        {BYTE_STREAM, "C-RNTI",          2},
};

const Fmt LteMacULTransportBlock_Mac_CE_PHR[] = {
        {UINT,        "PHR Field",       1},
        {PLACEHOLDER, "PHR Ind",         0},
};

const Fmt LteMacULTransportBlock_Mac_CE_RBRQ[] = {
        {UINT,        "RBRQ Field 1",     1},
        {UINT,        "RBRQ Field 2",     1},
        {PLACEHOLDER, "LCID (RBRQ)",      0},
        {PLACEHOLDER, "UL/DL",            0},
        {PLACEHOLDER, "Bit Rate",         0},
        {PLACEHOLDER, "Bit Rate Multiplier", 0},
};

const Fmt LteMacDLTransportBlockFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2}
};

const Fmt LteMacDLTransportBlock_SubpktHeaderFmt[] = {
        {UINT, "SubPacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "SubPacket Size", 2},
        {UINT, "Num Samples",    1},
};

const Fmt LteMacDLTransportBlock_SubpktV2_SampleFmt[] = {
        {UINT,        "Sub-FN",          2},
        {PLACEHOLDER, "SFN",             0},
        {UINT,        "RNTI Type",       1},
        {UINT,        "HARQ ID",         1},
        {PLACEHOLDER, "Area ID",         0},
        {UINT,        "PMCH ID",         2},
        {UINT,        "DL TBS (bytes)",  2},
        {UINT,        "RLC PDUs",        1},
        {UINT,        "Padding (bytes)", 2},
        {UINT,        "HDR LEN",         1},
        // Mac Hdr + CE and UL TB Other Structure
};

const Fmt LteMacDLTransportBlock_SubpktV4_SampleFmt[] = {
        {UINT,        "Sub Id",          1},
        {UINT,        "Cell Id",         1},
        {UINT,        "Sub-FN",          2},
        {PLACEHOLDER, "SFN",             0},
        {UINT,        "RNTI Type",       1},
        {UINT,        "HARQ ID",         1},
        {PLACEHOLDER, "Area ID",         0},
        {UINT,        "PMCH ID",         2},
        {UINT,        "DL TBS (bytes)",  2},
        {UINT,        "RLC PDUs",        1},
        {UINT,        "Padding (bytes)", 2},
        {UINT,        "HDR LEN",         1},
        // Mac Hdr + CE and UL TB Other Structure
};

const ValueName LteMacDLTransportBlock_Mac_Hdr_LCId[] = {
        {0, "CCCH"},
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {4, "4"},
        {5, "5"},
        {6, "6"},
        {7, "7"},
        {8, "8"},
        {9, "9"},
        {10, "10"},
        {11, "Reserved (unsupported)"},
        {12, "Reserved (unsupported)"},
        {13, "Reserved (unsupported)"},
        {14, "Reserved (unsupported)"},
        {15, "Reserved (unsupported)"},
        {16, "Extended LC ID field (unsupported)"},
        {17, "DCQR Command (unsupported)"},
        {18, "Activation/Deactivation of PDCP Duplication (unsupported)"},
        {19, "Hibernation (1 octet) (unsupported)"},
        {20, "Hibernation (4 octet) (unsupported)"},
        {21, "Activation/Deactivation of CSI-RS (unsupported)"},
        {22, "Recommended bit rate (unsupported)"},
        {23, "SC-PTM Stop Indication (unsupported)"},
        {24, "Activation/Deactivation (4 octet) (unsupported)"},
        {25, "SC-MCCH, SC-MTCH (unsupported)"},
        {26, "Long DRX Command"},
        {27, "Activation/Deactivation (1 octet)"},
        {28, "CRID"},
        {29, "TA"},
        {30, "DRX Command"},
        {31, "Padding"},
};

const Fmt LteMacDLTransportBlock_Mac_Hdr[] = {
        {UINT,        "Header Field",    1},
        {PLACEHOLDER, "LC ID",           0},
        {PLACEHOLDER, "Len",             0},
};

const Fmt LteMacDLTransportBlock_Mac_Hdr_L1[] = {
        {UINT,        "L1 Field",        1},
};

const Fmt LteMacDLTransportBlock_Mac_Hdr_L2[] = {
        {UINT,        "L2 Field",        1},
};

const Fmt LteMacDLTransportBlock_Mac_CE_TA[] = {
        {UINT,        "TA Field",        1},
        {PLACEHOLDER, "TAG Id",          0},
        {PLACEHOLDER, "TA Command",      0},
};

const Fmt LteMacDLTransportBlock_Mac_CE_AD1[] = {
        {UINT,        "AD Field",        1},
};

const Fmt LteMacDLTransportBlock_Mac_CE_CRID[] = {
        {BYTE_STREAM, "CRID",            6},
};

const Fmt LteMacULTxStatisticsFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2}
};

const Fmt LteMacULTxStatistics_SubpktHeaderFmt[] = {
        {UINT, "SubPacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "SubPacket Size", 2},
};

const Fmt LteMacULTxStatistics_ULTxStatsSubPacketFmt[] = {
        {UINT, "Number of samples",      1},
        {UINT, "Number of padding BSR",  1},
        {UINT, "Number of regular BSR",  1},
        {UINT, "Number of periodic BSR", 1},
        {UINT, "Number of cancel BSR",   1},
        {UINT, "Grant received",         4},
        {UINT, "Grant utilized",         4},
        {SKIP, NULL,                     3}
};

const Fmt LteMacULTxStatistics_ULTxStatsSubPacketFmtV2[] = {
        {UINT, "Sub Id",                 1},
        {UINT, "Number of samples",      1},
        {UINT, "Number of padding BSR",  1},
        {UINT, "Number of regular BSR",  1},
        {UINT, "Number of periodic BSR", 1},
        {UINT, "Number of cancel BSR",   1},
        {UINT, "Grant received",         4},
        {UINT, "Grant utilized",         4},
        {SKIP, NULL,                     2}
};

const ValueName LteMacConfigurationSubpkt_SubpktType[] = {
        {0,  "Config Type Subpacket"},
        {1,  "DL Config SubPacket"},
        {2,  "UL Config SubPacket"},
        {3,  "RACH Config SubPacket"},
        {4,  "LC Config SubPacket"},
        {7,  "DL Transport Block"},
        {8,  "UL Transport Block"},
        {10, "UL Buffer Status SubPacket"},
        {11, "UL Tx Statistics SubPacket"},
        {13, "eMBMS Config SubPacket"},
        {14, "All Rach Config SubPacket"},
        {18, "ELS SubPacket"}
};

const Fmt LteMacConfigurationFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2}
};

const Fmt LteMacConfiguration_SubpktHeader[] = {
        {UINT, "SubPacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "SubPacket Size", 2}
};

const Fmt LteMacConfigurationSubpkt_ConfigType[] = {
        {UINT, "Config reason", 4}
};


const Fmt LteMacConfigurationSubpkt_ConfigType_v2[] = {
        {UINT, "Sub Id",         1},
        {UINT, "Config reason",  1},
        {UINT, "Config Bitmask", 1},
        {SKIP, "NULL",           1},
};

const ValueName LteMacConfigurationConfigType_ConfigReason[] = {
        {2050, "CONNECTION RELEASE"}
};

const Fmt LteMacConfigurationSubpkt_DLConfig[] = {
        {UINT, "TA Timer", 2},    // 0xFF need to be read as infinity
        {SKIP, "NULL",     2}
};

const Fmt LteMacConfigurationSubpkt_DLConfig_v2[] = {
        {UINT, "Sub Id",          1},
        {UINT, "Num Active Stag", 1},
};

const Fmt LteMacConfigurationSubpkt_DLConfig_Scell_Tag_Info_v2[] = {
        {UINT, "STAG Id",          1},
        {UINT, "Scell Id Mask",    1},
        {UINT, "Ta Timer Present", 1},
        {UINT, "TA Timer",         2},
        {SKIP, "NULL",             1},
};

const ValueName LteMacConfigurationConfigType_DLConfig_TA_Timer[] = {
        {0xffff, "Infinity"},
};

const Fmt LteMacConfigurationSubpkt_ULConfig[] = {
        {UINT, "SR resource present",       1},
        {UINT, "SR periodicity",            2},
        {UINT, "BSR timer",                 2},
        {UINT, "SPS Number of Tx released", 2},
        {UINT, "Retx BSR timer",            2},    // 0xFF need to be read as infinity
        {SKIP, "NULL",                      3}
};

const Fmt LteMacConfigurationSubpkt_ULConfig_v2[] = {
        {UINT, "Sub Id",                    1},
        {UINT, "SR resource present",       1},
        {UINT, "SR periodicity",            2},
        {UINT, "BSR timer",                 2},
        {UINT, "SPS Number of Tx released", 2},
        {UINT, "Retx BSR timer",            2},    // 0xFF need to be read as infinity
        {SKIP, "NULL",                      2},
};

const ValueName LteMacConfigurationConfigType_ULConfig_BSR_Timer[] = {
        {0xffff, "Infinity"},
};

const Fmt LteMacConfigurationSubpkt_RACHConfig[] = {
        {WCDMA_MEAS, "Preamble initial power",      1},
        {UINT, "Power ramping step",          1},
        {UINT, "RA index1",                   1},
        {UINT, "RA index2",                   1},
        {UINT, "Preamble trans max",          1},
        {UINT, "Contention resolution timer", 1},
        {SKIP, "NULL",                        4},
        {UINT, "PMax",                        2},
        {UINT, "Delta preamble Msg3",         2},
        {UINT, "PRACH config",                1},
        {UINT, "CS zone length",              1},
        {UINT, "Root seq index",              2},
        {UINT, "PRACH Freq Offset",           2},
        {UINT, "Max retx Msg3",               1},
        {UINT, "RA rsp win size",             1},
        {SKIP, "NULL",                        1},
};

const Fmt LteMacConfigurationSubpkt_RACHConfig_v2[] = {
        {WCDMA_MEAS, "Preamble initial power", 1},
        {SKIP, "NULL",                        1},
        {UINT, "Power ramping step",          1},
        {UINT, "RA index1",                   1},
        {UINT, "RA index2",                   1},
        {UINT, "Preamble trans max",          1},
        {UINT, "Contention resolution timer", 2},
        {UINT, "Message size Group_A",        2},
        {UINT, "Power offset Group_B",        1},
        {UINT, "PMax",                        2},
        {UINT, "Delta preamble Msg3",         2},
        {UINT, "PRACH config",                1},
        {UINT, "CS zone length",              1},
        {UINT, "Root seq index",              2},
        {UINT, "PRACH Freq Offset",           1},
        {UINT, "High speed flag",             1},
        {UINT, "Max retx Msg3",               1},
        {UINT, "RA rsp win size",             1},
};

const Fmt LteMacConfigurationSubpkt_RACHConfig_v5[] = {
        {WCDMA_MEAS, "Preamble initial power",      1},
        {UINT, "Power ramping step",          1},
        {UINT, "RA index1",                   1},
        {UINT, "RA index2",                   1},
        {UINT, "Preamble trans max",          1},
        {UINT, "Contention resolution timer", 2},
        {UINT, "Message size Group_A",        2},
        {UINT, "Power offset Group_B",        1},
        {UINT, "PMax",                        2},
        {UINT, "Delta preamble Msg3",         2},
        {UINT, "PRACH config",                1},
        {UINT, "CS zone length",              1},
        {UINT, "Root seq index",              2},
        {UINT, "PRACH Freq Offset",           1},
        {PLACEHOLDER, "Preamble Format",      0},
        {UINT, "High speed flag",             1},
        {UINT, "Max retx Msg3",               1},
        {UINT, "RA rsp win size",             1},
        {UINT, "PRACH Cfg R13 Present",       1},
};

const ValueName LteMacConfigurationSubpkt_RACHConfig_Power_offset_Group_B[] = {
        {0x00, "-Infinity"},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_size_v5[] = {
        {UINT, "RSRP Thresh PRACH List Size", 1},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_v5[] = {
        {UINT, "RSRP Thresh PRACH List", 1},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_hidden_rsrp_prach_list_v5[] = {
        {UINT, "Hidden RSRP Thresh PRACH List", 1},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_prach_param_ce_list_size_v5[] = {
        {UINT, "PRACH Param CE List", 1},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_prach_list_v5[] = {
        {UINT,"First Preamble",                 1},
        {UINT,"Last Preamble",                  1},
        {UINT,"Max Preamble Tx Attempt Per CE", 1},
        {UINT,"Contention Resol Timer",         2},
        {UINT,"Prach Cfg Index",                1},
        {UINT,"RA RSP Win Size",                1},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_hidden_prach_list_v5[] = {
        {BYTE_STREAM, "Hidden PRACH Param Ce", 7},
};

const Fmt LteMacConfiguration_RachConfigSubpktPayload_prach_last_part[] = {
        {UINT, "Initial CE Level",      2},
        {UINT, "Preamble Trans Max CE", 2},
};

const ValueName LteMacConfiguration_RachConfigSubpktPayload_prach_initial_ce_level[] = {
        {0xffff, "NA"},
};

const Fmt LteMacConfigurationSubpkt_LCConfig[] = {
        {UINT, "Number of deleted LC",        1},
        {SKIP, "NULL",                        32},
        {UINT, "Number of added/modified LC", 1}
        //    {SKIP, "NULL", 290}
};

const Fmt LteMacConfigurationSubpkt_LCConfig_v2[] = {
        {UINT, "Sub Id",                      1},
        {UINT, "Number of deleted LC",        1},
        {SKIP, "NULL",                        32},
        {UINT, "Number of added/modified LC", 1}
        //    {SKIP, "NULL", 290}
};

const Fmt LteMacConfiguration_LCConfig_LC[] = {
        {UINT, "LC ID",                     1},
        {UINT, "PBR(KBytes/s)",             2},
        {UINT, "Priority",                  1},
        {UINT, "LC group",                  1},
        {UINT, "Token bucket size (bytes)", 4}
};

const Fmt LteMacConfigurationSubpkt_eMBMSConfig[] = {
        {UINT, "Num eMBMS Active LCs", 2},    // Not sure if this offset and length of this field is correct
        {SKIP, "NULL",                 98}
};

const Fmt LteMacConfigurationSubpkt_eMBMSConfig_v2[] = {
        {UINT, "Sub Id",               1},
        {UINT, "Num eMBMS Active LCs", 2},    // Not sure if this offset and length of this field is correct
        {SKIP, "NULL",                 98}
};

const Fmt LteMacConfigurationSubpkt_ELS[] = {
        {UINT, "Sub Id",                  1},
        {UINT, "ELS UL LC Id",            1},
        {UINT, "ELS DL LC Id",            1},
        {UINT, "ELS MCE ReTx TMR Length", 1}
};

const Fmt LteMacConfigurationSubpkt_All_Rach_Config[] = {
        {UINT,        "Sub Id",              1},
        {BYTE_STREAM, "Valid Cell Cfg Mask", 1},
        {BYTE_STREAM, "New Cell Cfg Mask",   1},
};

const Fmt LteMacConfigurationSubpkt_All_Rach_Config_Cell_Info[] = {
        {UINT,       "Scell Id",                         1},
        {WCDMA_MEAS, "Preamble initial power (dB)",      1},    // Note sure if it is correct
        {UINT,       "Power ramping step (dB)",          1},
        {UINT,       "RA index1",                        1},
        {UINT,       "RA index2",                        1},
        {UINT,       "Preamble trans max",               1},
        {UINT,       "Contention resolution timer (ms)", 2},
        {UINT,       "Message size Group_A",             2},
        {UINT,       "Power offset Group_B",             1},
        {UINT,       "PMax (dBm)",                       2},
        {UINT,       "Delta preamble Msg3",              2},
        {UINT,       "PRACH config",                     1},
        {UINT,       "CS zone length",                   1},
        {UINT,       "Root seq index",                   2},
        {UINT,       "PRACH Freq Offset",                1},
        {UINT,       "High speed flag",                  1},
        {UINT,       "Max retx Msg3",                    1},
        {UINT,       "RA rsp win size",                  1},
};

const Fmt LteRrcOtaPacketFmt[] = {
        {UINT, "Pkt Version", 1},
        {UINT, "RRC Release Number", 1},
};

// --- Version Specific Formats ---
const Fmt LteRrcOtaPacketFmt_v2[] = {
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 2},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "Msg Length", 1},
        {UINT, "SIB Mask in SI", 1},
};

const Fmt LteRrcOtaPacketFmt_v4[] = {
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 2},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "Msg Length", 2}, // Changed length
};

const Fmt LteRrcOtaPacketFmt_v7[] = {
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 2},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {SKIP, NULL, 4}, // Added skip
        {UINT, "Msg Length", 1},
        {UINT, "SIB Mask in SI", 1},
};

const Fmt LteRrcOtaPacketFmt_v8[] = {
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4}, // Changed length
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v9[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v12[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v13[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v15[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v19[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v20[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v24[] = { // Same as v8
        {UINT, "Major/minor", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 4},
        {UINT, "SysFrameNum/SubFrameNum", 2},
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

const Fmt LteRrcOtaPacketFmt_v26[] = {
        {BYTE_STREAM, "RRC Version Number", 1}, // Changed type
        {UINT, "NR RRC Release Number", 1},     // New field
        {BYTE_STREAM, "NR RRC Version Number", 1},// New field
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Physical Cell ID", 2},
        {UINT, "Freq", 2},                     // Changed length back to 2
        {UINT, "SysFrameNum/SubFrameNum", 4},  // Changed length to 4
        {UINT, "PDU Number", 1},
        {UINT, "SIB Mask in SI", 1},
        {SKIP, NULL, 3},
        {UINT, "Msg Length", 2},
};

// --- ValueName Arrays ---
const ValueName LteRrcOtaPduType[] = { // For versions < 15
        {0x02, "LTE-RRC_BCCH_DL_SCH"},
        {0x04, "LTE-RRC_PCCH"},
        {0x05, "LTE-RRC_DL_CCCH"},
        {0x06, "LTE-RRC_DL_DCCH"},
        {0x07, "LTE-RRC_UL_CCCH"},
        {0x08, "LTE-RRC_UL_DCCH"},
};

const ValueName LteRrcOtaPduType_v15[] = { // For versions >= 15 and < 19
        {0x02, "LTE-RRC_BCCH_DL_SCH"},
        {0x05, "LTE-RRC_PCCH"},
        {0x06, "LTE-RRC_DL_CCCH"},
        {0x07, "LTE-RRC_DL_DCCH"},
        {0x08, "LTE-RRC_UL_CCCH"},
        {0x09, "LTE-RRC_UL_DCCH"},
};

const ValueName LteRrcOtaPduType_v19[] = { // For versions 19 and 26
        {0x03, "LTE-RRC_BCCH_DL_SCH"},
        {0x07, "LTE-RRC_PCCH"},
        {0x08, "LTE-RRC_DL_CCCH"},
        {0x09, "LTE-RRC_DL_DCCH"},
        {0x0a, "LTE-RRC_UL_CCCH"},
        {0x0b, "LTE-RRC_UL_DCCH"},
        {0x2e, "LTE-RRC_BCCH_DL_SCH_NB"},
        {0x30, "LTE-RRC_DL_CCCH_NB"},
        {0x31, "LTE-RRC_DL_DCCH_NB"},
        {0x34, "LTE-RRC_UL_DCCH_NB"},
        {0x32, "LTE-RRC_UL_CCCH_NB"},
};


// LTE Power Comtrol

const Fmt LtePucchPowerControl_Fmt[] = {
        {UINT, "Version", 1},
};
const Fmt LtePucchPowerControl_Fmt_v4[] = {
        {SKIP, NULL,                2},
        {UINT, "Number of Records", 1},
};

const Fmt LtePucchPowerControl_Record_Fmt_v4[] = {
        {UINT,        "SFN",                   4},
        // include Sub-FN,  Tx Power, DCI Format, PUCCH Format, N_HARQ
        {PLACEHOLDER, "Sub-FN",                0},
        {PLACEHOLDER, "PUCCH Tx Power (dBm)",  0},
        {PLACEHOLDER, "DCI Format",            0},
        {PLACEHOLDER, "PUCCH Format",          0},
        {PLACEHOLDER, "N_HARQ",                0},

        {UINT,        "TPC Command",           4},
        // include N_CQI, DL Pass Loss,
        {PLACEHOLDER, "N_CQI",                 0},
        {PLACEHOLDER, "DL Path Loss",          0},

        {UINT,        "g(i)",                  2},
        {UINT,        "PUCCH Actual Tx Power", 1},
        {SKIP,        NULL,                    1},
};
const ValueName LtePucchPowerControl_Record_v4_DCI_Format[] = {
        // Release 8
        // http:    //www.sharetechnote.com/html/LTE_Advanced_DCI.html
        {0,  "Format 0"},
        {1,  "Format 1"},
        {2,  "Format 1A"},
        {3,  "Format 1B"},
        {4,  "Format 1C"},
        {5,  "Format 1D"},
        {6,  "Format 2"},
        {7,  "Format 2A"},
        // {8, "Format 2B"},
        // {9, "Format 2C"},
        {10, "Format 3"},
        {11, "Format 3A"},

        {12, "NO DCI"},
        {13, "NO DCI"},
        // {12, "Format 4"},
};
const ValueName LtePucchPowerControl_Record_v4_PUCCH_Format[] = {
        {0, "Format 1"},
        {1, "Format 1A"},
        {2, "Format 1B"},
        {3, "Format 2"},
        {4, "Format 2A"},
        {5, "Format 2B"},
        //    {6, "Format 3"},
        {6, "Format 1bCS"},
        {7, "Format 3"},
};


const ValueName LtePucchPowerControl_Record_v4_TPC[] = {
        {31, "Not present"},
        {63, "-1"},
};


const Fmt LtePucchPowerControl_Fmt_v24[] = {
        {SKIP, NULL,                2},
        {UINT, "Number of Records", 1},
};


const Fmt LtePucchPowerControl_Record_Fmt_v24[] = {
        {UINT,        "SFN",                   4},
        // include Sub-FN,  Tx Power, DCI Format, PUCCH Format, N_HARQ
        {PLACEHOLDER, "Sub-FN",                0},
        {PLACEHOLDER, "PUCCH Tx Power (dBm)",  0},
        {PLACEHOLDER, "DCI Format",            0},
        {PLACEHOLDER, "PUCCH Format",          0},
        {PLACEHOLDER, "N_HARQ",                0},

        {UINT,        "TPC Command",           4},
        // include N_CQI, DL Pass Loss,
        {PLACEHOLDER, "N_CQI",                 0},
        {PLACEHOLDER, "DL Path Loss",          0},

        {UINT,        "g(i)",                  2},
        {UINT,        "PUCCH Actual Tx Power", 1},
        {SKIP,        NULL,                    1},
};

const ValueName LtePucchPowerControl_Record_v24_DCI_Format[] = {
        // Release 8
        // http:    //www.sharetechnote.com/html/LTE_Advanced_DCI.html
        {0,  "Format 0"},
        {1,  "Format 1"},
        {2,  "Format 1A"},
        {3,  "Format 1B"},
        {4,  "Format 1C"},
        {5,  "Format 1D"},
        {6,  "Format 2"},
        {7,  "Format 2A"},
        // {8, "Format 2B"},
        // {9, "Format 2C"},
        {10, "Format 3"},
        {11, "Format 3A"},
        // {12, "Format 4"},
};
const ValueName LtePucchPowerControl_Record_v24_PUCCH_Format[] = {
        {0, "Format 1"},
        {1, "Format 1A"},
        {2, "Format 1B"},
        {3, "Format 2"},
        {4, "Format 2A"},
        {5, "Format 2B"},

        {6, "Format 1bcs"},
};
const ValueName LtePucchPowerControl_Record_v24_TPC[] = {
        {31, "Not present"},
        {63, "-1"},
};

// --------

const Fmt LtePhyInterlogFmt [] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyInterlogFmt_v2_Header[] = {
        {SKIP, NULL, 3},
        {UINT, "Serving Cell E-ARFCN", 4},
        {UINT, "Serving Physical Cell ID", 2},
        {UINT, "Sub-frame Number", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 2},
        {UINT, "E-ARFCN", 4},
        {UINT, "Number of Neighbor Cells", 1},
        {UINT, "Number of Detected Cells", 1},
        {UINT, "Meas BW", 2}
};

const Fmt LtePhyInterlogFmt_v2_Neighbor_Cell[] = {
        {UINT, "Physical Cell ID", 2},
        {RSRP, "RSRP(dBm)", 2},
        {SKIP, NULL, 2},
        {RSRQ, "RSRQ(dB)", 2},
        {SKIP, NULL, 4}
};

const Fmt WcdmaCellIdFmt [] = {
        {UINT, "Uplink RF channel number", 4},    //Uplink RF channel number
        {UINT, "Download RF channel number", 4},    //Download RF channel number
        {UINT, "Cell ID", 4},               //Cell ID
        {UINT, "UTRA registration area (overlapping URAs)", 1}, // UTRA registration area (overlapping URAs)
        {SKIP, NULL, 2},    // Unknown yet
        {UINT, "Allowed call access", 1},   //Allowed call access
        {UINT, "PSC", 2},   //PSC
        {PLMN_MK1, "PLMN", 6},  //PLMN
        {UINT, "LAC", 4},    //Location area code
        {UINT, "RAC", 4}     //routing area code
};

bool is_log_packet (const char *b, size_t length);
std::string decode_log_packet (const char *b, size_t length, bool skip_decoding);

#endif //SIGNAL_LOG_PACKET_H
