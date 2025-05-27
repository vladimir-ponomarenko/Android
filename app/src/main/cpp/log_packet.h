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

const ValueName ValueNameAggregationLevel[] = {
        {0, "Agg1"},
        {1, "Agg2"},
        {2, "Agg4"},
        {3, "Agg8"},

        {6, "Agg16"},
        {7, "Agg24"},
};

const ValueName ValueNameSearchSpaceType[] = {
        {0, "Common"},
        {1, "UE-specific"},

        {3, "Common Type 2"},
        {2, "User"},
};

const ValueName ValueNameSubcarrierSpaceType[] = {
        {4, "15 kHz"},
};

const ValueName ValueNameDCIFormat[] = {
        // 4 bits
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
        {8,  "Format 3"},
        {9,  "Format 3A"},

        {12, "Format 60A"},
        {13, "Format 61A"},
        {15, "Reserved"},

        {14, "Format 62"},
};

const ValueName ValueNameMatchOrNot[] = {
        {0, "Mismatch"},
        {1, "Match"},
};

const ValueName ValueNamePruneStatus[] = {
        {1,   "SUCCESS_DCI0"},
        {3,   "SUCCESS_DCI1A"},
        {4,   "SUCCESS_DCI1C"},
        {6,   "SUCCESS_DCI2_2A_2B"},
        {8,   "TAIL_MISMATCH"},
        {9,   "FAIL_SURVIVOR_SELECT"},
        {10,  "PADDING_ERROR"},
        {13,  "RB_ALLOC_SET_NUM_ERROR_TYPE1"},
        {17,  "DUPLICATE_HARQ_ID_ERROR"},
        {19,  "UNEXPECTED_PAYLOAD_SIZE"},
        {24,  "BAD_RIV_DCI0"},
        {25,  "RB_ALLOC_ERROR_DCI0"},
        {26,  "INVALID_RB_NUM_DCI0"},
        {32,  "BAD_RIV_DCI1A"},
        {33,  "RB_ALLOC_ERROR_DCI1A"},
        {34,  "INVALID_RB_NUM_DCI1A"},
        {36,  "RETURN_DL_DATA_ARRIVAL_DCI1A"},
        {44,  "RB_ALLOC_ERROR_DCI1C"},
        {48,  "PMI_ERROR_DCI2_2A"},
        {50,  "NUM_LAYERS_ERROR_DCI2_2A_TB1"},
        {64,  "FAIL_SER_ENGYMTRC_CHECK"},

        {70,  "NUM_LAYERS_EXCEED_NUM_RX"},

        {200, "PDCCH_DEBUG_SUCCESS_DCI60A"},
        {201, "PDCCH_DEBUG_SUCCESS_DCI61A"},

        {202, "PDCCH_DEBUG_SUCCESS_DCI62"},
        {216, "PDCCH_DEBUG_SUCCESS_DCI62_EARLY_TERMINATION"},
};

const ValueName ValueNameFrameStructure[] = {
        {0, "FDD"},
        {1, "TDD"},
};

const ValueName ValueNameNumNBAntennas[] = {
        {0, "1 or 2"},
        {1, "2 antennas"},
        {2, "4 antennas"},
};

const ValueName ValueNameNumNBAntennas_PDCCH_V141[] = {
        {0, "1 or 2"},
        {1, "4 antennas"},
};

const ValueName ValueNameNormalOrNot[] = {
        {0, "Normal"},
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

const ValueName ValueNameSRSShortingfor2ndSlot[] = {
        // 1 bit
        {0, "Normal"},
        {1, "Shorten 2nd"},
};

const ValueName ValueNameOnOrOff[] = {
        {0, "Off"},
        {1, "On"},
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


const Fmt LteRlcDlAmAllPduFmt[] = {
        {UINT, "Version",              1},
        {UINT, "Number of Subpackets", 1},
        {SKIP, NULL,                   2},
};

const Fmt LteRlcDlAmAllPdu_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcDlAmAllPdu_SubpktPayload[] = {
        {UINT, "RB Cfg Idx",              1},
        {UINT, "RB Mode",                 1},
        {UINT, "SN Length",               1},
        {SKIP, NULL,                      1},
        {UINT, "Enabled PDU Log Packets", 2},
        {UINT, "VR(R)",                   2},
        {UINT, "VR(X)",                   2},
        {UINT, "VR(MS)",                  2},
        {UINT, "VR(H)",                   2},
        {UINT, "Number of PDUs",          2},
};
const ValueName LteRlcDlAmAllPdu_Subpkt_RBMode[] = {
        {1, "AM"},
};

const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_Basic[] = {
        {PLACEHOLDER, "PDU TYPE",      0},
        {PLACEHOLDER, "rb_cfg_idx",    0},
        {PLACEHOLDER, "Status",        0},
        {UINT,        "sys_fn",        2},
        {PLACEHOLDER, "sub_fn",        0},
        {UINT,        "pdu_bytes",     2},
        {UINT,        "logged_bytes",  2},
        {SKIP,        NULL,            1},
        {UINT,        "D/C LookAhead", 1},
        {UINT,        "SN",            1},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_Control[] = {
        {PLACEHOLDER, "cpt", 0},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_NACK_ALLIGN[] = {
        {UINT, "NACK_SN", 2},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_NACK_PADDING[] = {
        {UINT, "NACK_SN", 1},
};

const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_DATA[] = {
        {PLACEHOLDER, "RF", 0},
        {PLACEHOLDER, "P",  0},
        {PLACEHOLDER, "FI", 0},
        {PLACEHOLDER, "E",  0},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LI_ALLIGN[] = {
        {UINT, "LI", 2},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LI_PADDING[] = {
        {UINT, "LI", 1},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LSF_SO[] = {
        {UINT, "LSF", 1},
        {UINT, "SO",  1},
};

const Fmt LteRlcUlAmAllPduFmt[] = {
        {UINT, "Version",              1},
        {UINT, "Number of Subpackets", 1},
        {SKIP, NULL,                   2},
};

const Fmt LteRlcUlAmAllPdu_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcUlAmAllPdu_SubpktPayload[] = {
        {UINT, "RB Cfg Idx",              1},
        {UINT, "RB Mode",                 1},
        {UINT, "SN Length",               1},
        {SKIP, NULL,                      1},
        {UINT, "Enabled PDU Log Packets", 2},

        {UINT, "VT(A)",                   2},
        {UINT, "VT(S)",                   2},
        {UINT, "PDU Without Poll",        2},
        {UINT, "Byte Without Poll",       4},
        {UINT, "Poll SN",                 2},
        {UINT, "Number of PDUs",          2},
};
const ValueName LteRlcUlAmAllPdu_Subpkt_RBMode[] = {
        {1, "AM"},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Basic[] = {
        {PLACEHOLDER, "PDU TYPE",      0},
        {PLACEHOLDER, "rb_cfg_idx",    0},
        {UINT,        "sys_fn",        2},
        {PLACEHOLDER, "sub_fn",        0},
        {UINT,        "pdu_bytes",     2},
        {UINT,        "logged_bytes",  2},
        {SKIP,        NULL,            1},
        {UINT,        "D/C LookAhead", 1},
        {UINT,        "SN",            1},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Control[] = {
        {PLACEHOLDER, "cpt", 0},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN[] = {
        {UINT, "NACK_SN", 2},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING[] = {
        {UINT, "NACK_SN", 1},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_DATA[] = {
        {PLACEHOLDER, "RF", 0},
        {PLACEHOLDER, "P",  0},
        {PLACEHOLDER, "FI", 0},
        {PLACEHOLDER, "E",  0},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN[] = {
        {UINT, "LI", 2},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING[] = {
        {UINT, "LI", 1},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO[] = {
        {UINT, "LSF", 1},
        {UINT, "SO",  1},
};

const Fmt LteRlcUlAmAllPdu_SubpktPayload_v4[] = {
        {UINT, "RB Cfg Idx",              1},
        {UINT, "RB Mode",                 1},
        {UINT, "SN Length",               1},
        {SKIP, NULL,                      1},
        {UINT, "Enabled PDU Log Packets", 2},
        {UINT, "VT(A)",                   2},
        {UINT, "VT(S)",                   2},
        {UINT, "PDU Without Poll",        2},
        {UINT, "Byte Without Poll",       4},
        {UINT, "Poll SN",                 2},
        {UINT, "Number of PDUs",          2},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Basic_v4[] = {
        {PLACEHOLDER, "PDU TYPE",      0},
        {PLACEHOLDER, "rb_cfg_idx",    0},
        {PLACEHOLDER, "SFN",           0},
        {UINT,        "sys_fn",        2},
        {PLACEHOLDER, "sub_fn",        0},
        {UINT,        "pdu_bytes",     2},
        {UINT,        "logged_bytes",  2},
        {SKIP,        NULL,            1},
        {UINT,        "D/C LookAhead", 1},
        {UINT,        "SN",            1},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Control_v4[] = {
        {PLACEHOLDER, "cpt", 0},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN_v4[] = {
        {UINT, "NACK_SN", 2},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING_v4[] = {
        {UINT, "NACK_SN", 1},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_DATA_v4[] = {
        {PLACEHOLDER, "RF", 0},
        {PLACEHOLDER, "P",  0},
        {PLACEHOLDER, "FI", 0},
        {PLACEHOLDER, "E",  0},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN_v4[] = {
        {UINT, "LI", 2},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING_v4[] = {
        {UINT, "LI", 1},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO_v4[] = {
        {UINT, "LSF", 1},
        {UINT, "SO",  1},
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

const Fmt LteRlcDlConfigLogPacketFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2},    // reserved
};

const Fmt LteRlcDlConfigLogPacket_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcDlConfigLogPacket_SubpktPayload[] = {
        {UINT, "Reason",       1},
        {UINT, "Max Size RBs", 1},
};

const ValueName LteRlcDlConfigLogPacket_Subpkt_Reason[] = {
        {1, "Configuration"},
        {2, "Handover"},
        {4, "RB Release"},
        {8, "Radio Link Failure"},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header[] = {
        {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt[] = {
        {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header[] = {
        {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt[] = {
        {UINT, "Added/Modified RB Cfg Index", 1},
        {UINT, "Action",                      1},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action[] = {
        {1, "Add"},
        {2, "Modify"},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header[] = {
        {UINT, "Number of Active RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt[] = {
        {PLACEHOLDER, "RLCDL CFG",              0},
        {UINT,        "RB Mode",                1},
        {UINT,        "LC ID",                  1},
        {UINT,        "RB ID",                  1},
        {UINT,        "RB Cfg Idx",             1},
        {UINT,        "RB Type",                1},
        {UINT,        "T Reordering (ms)",      2},
        {PLACEHOLDER, "SN Length",              0},
        {UINT,        "T Status Prohibit (ms)", 2},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode[] = {
        {1, "AM"},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType[] = {
        {1, "SRB"},
        {2, "DRB"},
};

const Fmt LteRlcDlConfigLogPacket_SubpktPayload_v2[] = {
        {UINT, "Reason",       1},
        {UINT, "Max Size RBs", 1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header_v2[] = {
        {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt_v2[] = {
        {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header_v2[] = {
        {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt_v2[] = {
        {UINT, "Added/Modified RB Cfg Index", 1},
        {UINT, "Action",                      1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header_v2[] = {
        {UINT, "Number of Active RBs", 1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt_v2[] = {
        {PLACEHOLDER, "RLCDL CFG",              0},
        {UINT,        "RB Mode",                1},
        {UINT,        "LC ID",                  1},
        {UINT,        "RB ID",                  1},
        {UINT,        "RB Cfg Idx",             1},
        {UINT,        "RB Type",                1},
        {UINT,        "T Reordering (ms)",      2},
        {UINT,        "T Status Prohibit (ms)", 2},
        {UINT,        "SN Length",              1},
};

const Fmt LteRlcUlConfigLogPacketFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2},    // reserved
};

const Fmt LteRlcUlConfigLogPacket_SubpktHeader[] = {
        {UINT, "Subpacket ID",      1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size",    2},
};

const Fmt LteRlcUlConfigLogPacket_SubpktPayload[] = {
        {UINT, "Reason",       1},
        {UINT, "Max Size RBs", 1},
};

const ValueName LteRlcUlConfigLogPacket_Subpkt_Reason[] = {
        {1, "Configuration"},
        {2, "Handover"},
        {4, "RB Release"},
        {8, "Radio Link Failure"},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header[] = {
        {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt[] = {
        {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header[] = {
        {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt[] = {
        {UINT, "Added/Modified RB Cfg Index", 1},
        {UINT, "Action",                      1},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action[] = {
        {1, "Add"},
        {2, "Modify"},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header[] = {
        {UINT, "Number of Active RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt[] = {
        {PLACEHOLDER, "RLCUL CFG",          0},
        {UINT,        "RB Mode",            1},
        {UINT,        "LC ID",              1},
        {UINT,        "RB ID",              1},
        {UINT,        "RB Cfg Idx",         1},
        {UINT,        "RB Type",            1},
        {PLACEHOLDER, "SN Length",          0},
        {UINT,        "Poll Byte",          4},
        {UINT,        "Poll PDU",           2},
        {UINT,        "T Poll Retx (ms)",   2},
        {UINT,        "Max Retx Threshold", 1},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode[] = {
        {1, "AM"},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType[] = {
        {1, "SRB"},
        {2, "DRB"},
};

const Fmt LteRlcUlConfigLogPacket_SubpktPayload_v3[] = {
        {UINT, "Reason",       1},
        {UINT, "Max Size RBs", 1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header_v3[] = {
        {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt_v3[] = {
        {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header_v3[] = {
        {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt_v3[] = {
        {UINT, "Added/Modified RB Cfg Index", 1},
        {UINT, "Action",                      1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header_v3[] = {
        {UINT, "Number of Active RBs", 1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt_v3[] = {
        {PLACEHOLDER, "RLCUL CFG",          0},
        {UINT,        "RB Mode",            1},
        {UINT,        "LC ID",              1},
        {UINT,        "RB ID",              1},
        {UINT,        "RB Cfg Idx",         1},
        {UINT,        "RB Type",            1},
        //{PLACEHOLDER, "SN Length", 0},
        {UINT,        "Poll Byte",          4},
        {UINT,        "Poll PDU",           2},
        {UINT,        "T Poll Retx (ms)",   2},
        {UINT,        "Max Retx Threshold", 1},
        {UINT,        "AM SN Length",       1},
};

enum LtePhyIratType {
    LtePhyIratType_HRPD = 14,
    LtePhyIratType_WCDMA = 35,
    LtePhyIratType_1x = 41,
    LtePhyIratType_GSM = 42,
};


const Fmt LtePhyIratFmt[] = {
        {UINT, "Version",         1},
        {UINT, "Subpacket count", 1},
        {SKIP, NULL,              2},
};

const Fmt LtePhyIratSubPktFmt[] = {
        {UINT, "Subpacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "Subpacket size", 2},
};

const Fmt LtePhyIratWCDMAFmt[] = {
        {UINT, "Current DRX cycle",     4},
        {UINT, "Number of frequencies", 1},
        {SKIP, NULL,                    3},
};

const Fmt LtePhyIratWCDMACellMetaFmt[] = {
        {UINT, "Frequency",       2},
        {UINT, "Number of cells", 1},
        {SKIP, NULL,              1},
};

const Fmt LtePhyIratWCDMACellFmt[] = {
        {UINT,       "PSC+Energy",                             2},
        {UINT,       "CSG",                                    1},
        {SKIP,       NULL,                                     1},
        {UINT,       "Pn Pos",                                 4},
        {WCDMA_MEAS, "RSCP",                                   1},
        {WCDMA_MEAS, "RSSI",                                   1},
        {WCDMA_MEAS, "EcNo",                                   1},
        {UINT,       "SrxLev",                                 1},
        {UINT,       "DRX cycle count since last measurement", 2},
        {UINT,       "Treselection",                           1},
        {UINT,       "Squal",                                  1}

};

const Fmt LtePhyIratCDMACellFmt[] = {
        {UINT, "Number of Pilots", 1},
        {UINT, "Band",             1},
        {UINT, "Channel",          2},
};

const Fmt LtePhyIratCDMACellPilotFmt[] = {
        {UINT, "Pilot ID", 2},
        {UINT, "RSS (dB)", 2},
        {SKIP, NULL,       4},
        {UINT, "EcNo",     2},
        {SKIP, NULL,       6},
};


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



const ValueName ValueNameModulation[] = {
        {0, "BPSK"},
        {1, "QPSK"},
        {2, "16QAM"},
        {3, "64QAM"},
        {4, "256QAM"}
};

const Fmt LtePhyPuschCsf_Fmt[] = {
        {UINT, "Version", 1},
};


const Fmt LtePhyPuschCsf_Payload_v22[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
};


const Fmt LtePhyPuschCsf_Payload_v23[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v42[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v101[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v102[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};



const Fmt LtePhyPuschCsf_Payload_v103[] = {
        {UINT, "Start System Sub-frame Number", 4},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {PLACEHOLDER, "PUSCH Reporting Mode", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Wideband PMI1", 0},
        {PLACEHOLDER, "Number of subbands", 0},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {UINT, "WideBand CQI CW1", 2},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Size M", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {UINT, "Single MB PMI", 1},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 14},
        {UINT, "Forced Max RI", 1},
        {SKIP, NULL, 5},
        {UINT, "Alt Cqi Table Data", 1},
        {SKIP, NULL, 7},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num Csirs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v142[] = {
        {UINT, "Start System Sub-frame Number", 4},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {PLACEHOLDER, "Carrier Index", 0},
        {PLACEHOLDER, "Scell Index", 0},
        {PLACEHOLDER, "PUSCH Reporting Mode", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Wideband PMI1", 0},
        {UINT, "Number of subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Size M", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {UINT, "CSF Tx Mode", 1},
        {SKIP, NULL, 25},
        {UINT, "Num Csirs Ports", 1},
        {UINT, "CRI", 4},
        {PLACEHOLDER, "UL Frame Number", 0},
        {PLACEHOLDER, "UL Subframe Number", 0},
        {PLACEHOLDER, "UL Payload Length", 0},
};


const ValueName ValueNamePuschReportingMode[] = {
        {0, "MODE_APERIODIC_RM12"},
        {1, "MODE_APERIODIC_RM20"},
        {2, "MODE_APERIODIC_RM22"},
        {3, "MODE_APERIODIC_RM30"},
        {4, "MODE_APERIODIC_RM31"},
        {5, "MODE_PERIODIC_RM10"},
        {6, "MODE_PERIODIC_RM11"},
        {7, "MODE_PERIODIC_RM20"},
        {8, "MODE_PERIODIC_RM21"}
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


const Fmt LtePhyPdschDecodingResult_Fmt [] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyPdschDecodingResult_Payload_v24 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {SKIP, NULL, 1},
        {UINT, "Carrier Index", 1}, // 3 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Payload_v44 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {SKIP, NULL, 4},
        {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Payload_v106 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 11 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {SKIP,NULL,2},
        {UINT, "Nir",4},
        {PLACEHOLDER, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Payload_v124 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {UINT, "C_RNTI",2},
        {SKIP,NULL,2},
        {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Payload_v126 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {UINT, "C_RNTI",2},
        {SKIP,NULL,2},
        {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Record_v24 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "Subframe Number",0 },
        {UINT, "PDSCH Channel ID", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "System Information Msg Number", 2}, // 4 bits
        {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
        {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
        {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
        {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_Record_v44 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "Subframe Number",0 },
        {UINT, "PDSCH Channel ID", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "System Information Msg Number", 2}, // 4 bits
        {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
        {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
        {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
        {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
        {BYTE_STREAM, "Demap Sic Status",2},
        {SKIP, "Reserved",2},
};

const Fmt LtePhyPdschDecodingResult_Record_v105 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "Subframe Number",0 },
        {UINT, "PDSCH Channel ID", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "System Information Msg Number", 2}, // 4 bits
        {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
        {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
        {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
        {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
        {UINT, "Demap Sic Status",2},
        {SKIP, "Reserved",2},
};

const Fmt LtePhyPdschDecodingResult_Record_v106 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "Subframe Number",0 },
        {UINT, "PDSCH Channel ID",2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "System Information Msg Number",2},
        {PLACEHOLDER,"System Information Mask",0},
        {UINT, "HARQ Log Status",1},
        {PLACEHOLDER,"Codeword Swap",0},
        {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
        {UINT, "Demap Sic Status",2},
        {PLACEHOLDER, "MVC Status",0},
        {PLACEHOLDER, "MVC Clock Request",0},
        {UINT, "MVC Req Margin Data",1},
        {UINT, "MVC Rsp Margin",1},
};
const Fmt LtePhyPdschDecodingResult_Record_v124 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "Codeword Swap", 1},  // 1 bit
        {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_Record_v125 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "Subframe Number",0 },
        {UINT, "System Subframe Number",0 },
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "Codeword Swap", 1},  // 1 bit
        {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};


const Fmt LtePhyPdschDecodingResult_Record_v126 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "Codeword Swap", 1},  // 1 bit
        {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_Stream_v24 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
        {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
        {PLACEHOLDER, "RVID", 0},   // 2 bits
        {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
        {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
        {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
        {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
        {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 4 bits
};

const Fmt LtePhyPdschDecodingResult_Stream_v27 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
        {PLACEHOLDER, "RVID", 0},   // 2 bits
        {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
        {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
        {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
        {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
        {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 4 bits
        {UINT, "Retransmission Number", 4},
};

const Fmt LtePhyPdschDecodingResult_Stream_v44 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
        {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
        {PLACEHOLDER, "RVID", 0},   // 2 bits
        {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
        {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
        {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
        {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
        {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 1 bits
        {UINT, "LLR Scale", 1}, // 4 bits
        {SKIP, NULL, 3},
};

const Fmt LtePhyPdschDecodingResult_Stream_v106 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus Data", 0},
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
        {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
        {PLACEHOLDER, "RVID", 0},   // 2 bits
        {UINT, "Companding Stats", 4},   // 2 bits
        {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
        {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
        {PLACEHOLDER, "Effective Code Rate Data"},
        {PLACEHOLDER, "Num RE", 0},    // 15 bits
        {PLACEHOLDER, "Codeword Index", 0}, //  1 bits
        {UINT, "LLR Scale", 1}, // 4 bits
        {SKIP, NULL, 3},
};

const Fmt LtePhyPdschDecodingResult_TBlks_v124 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
        {PLACEHOLDER, "RVID", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max Half Iter Data", 0},    // 4 bits

        {UINT, "Num Channel Bits",4},
        {PLACEHOLDER, "CW Idx", 0},  // 4 bits
        {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
        {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

        {UINT, "First Decoded CB Index Qed Iter2 Data",4},
        {PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
        {PLACEHOLDER, "Companding Format", 0},  // 4 bits
        {SKIP, NULL, 1},
        {UINT, "Effective Code Rate Data", 2},
        {PLACEHOLDER,"Effective Code Rate", 0},
        {UINT, "HARQ Combine Enable",1},
};

const Fmt LtePhyPdschDecodingResult_TBlks_v126 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
        {PLACEHOLDER, "RVID", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
        {PLACEHOLDER, "Max Half Iter Data", 0},    // 4 bits

        {UINT, "Num Channel Bits",4},
        {PLACEHOLDER, "CW Idx", 0},  // 4 bits
        {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
        {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

        {UINT, "First Decoded CB Index Qed Iter2 Data",4},
        {PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
        {PLACEHOLDER, "Companding Format", 0},  // 4 bits

        {SKIP, NULL, 1},

        {UINT,"Effective Code Rate Data",2},
        {PLACEHOLDER,"Effective Code Rate", 0},
        {UINT, "HARQ Combine Enable",1},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v24 [] = {
        // totally 13
        {UINT, "Energy Metric", 4}, // 21 bits
        {PLACEHOLDER, "Iteration Number", 0},   // 4 bits
        {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
        {PLACEHOLDER, "Early Termination", 0},  // 1 bit
        {PLACEHOLDER, "HARQ Combine Enable", 0},    // 1 bit
        {PLACEHOLDER, "Deint Decode Bypass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v44 [] = {
        // totally 13
        {UINT, "Energy Metric", 4}, // 21 bits
        {PLACEHOLDER, "Iteration Number", 0},   // 4 bits
        {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
        {PLACEHOLDER, "Early Termination", 0},  // 1 bit
        {PLACEHOLDER, "HARQ Combine Enable", 0},    // 1 bit
        {PLACEHOLDER, "Deint Decode Bypass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v106 [] = {
        // totally 13
        {UINT, "Energy Metric", 4},
        {PLACEHOLDER, "Iteration Num", 0},
        {PLACEHOLDER, "Code Block CRC Pass", 0},
        {PLACEHOLDER, "Early Termination", 0},
        {PLACEHOLDER, "HARQ Combine Enable", 0},
        {PLACEHOLDER, "Deint Decode Bypass", 0},
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v106 [] = {
        {UINT, "Hidden Energy Metric First Half", 4},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v124 [] = {
        // totally 13
        {UINT, "Energy Metric", 4}, // 21 bits
        {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

        {UINT, "Half Iter Run Data", 4}, // 21 bits
        {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
        {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124 [] = {
        // totally 13
        {UINT, "Hidden Energy Metric First Half", 4},
        {UINT, "Hidden Energy Metric Second Half", 4},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v126 [] = {
        // totally 13
        {UINT, "Energy Metric", 4}, // 21 bits
        {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

        {UINT, "Half Iter Run Data", 4}, // 21 bits
        {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
        {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v126 [] = {
        // totally 13
        {UINT, "Hidden Energy Metric First Half", 4},
        {UINT, "Hidden Energy Metric Second Half", 4},
};

const Fmt LtePhyPdschDecodingResult_Payload_v143 [] = {
        {UINT, "Serving Cell ID", 4},   // 9 btis
        {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
        {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
        {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
        {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
        {UINT, "C_RNTI",2},
        {SKIP,NULL,2},
        {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Record_v143 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "HARQ ID", 1},   // 4 bits
        {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
        {UINT, "Codeword Swap", 1},  // 1 bit
        {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_TBlks_v143 [] = {
        {UINT, "Transport Block CRC", 4},   // 1 bit
        {PLACEHOLDER, "NDI", 0},    // 1 bit
        {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
        {PLACEHOLDER, "RVID", 0},    // 1 bit
        {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
        {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
        {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits

        {UINT, "Max Half Iter Data", 4},    // 4 bits
        {PLACEHOLDER, "Num Channel Bits",4}, // 19 bits
        {PLACEHOLDER, "CW Idx", 0},  // 1 bits
        {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
        {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

        //{UINT, "First Decoded CB Index Qed Iter2 Data",4},
        //{PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
        //{PLACEHOLDER, "Companding Format", 0},  // 4 bits

        {UINT, "Companding Format", 4},  // shift 18 bit 2 bits

        {SKIP, NULL, 1},

        {UINT,"Effective Code Rate Data",2},
        {UINT, "HARQ Combine Enable",1},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v143 [] = {
        // totally 13
        {UINT, "Energy Metric", 4}, // 21 bits
        {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

        {UINT, "Half Iter Run Data", 4}, // 21 bits
        {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
        {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v143 [] = {
        // totally 13
        {UINT, "Hidden Energy Metric First Half", 4},
        {UINT, "Hidden Energy Metric Second Half", 4},
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

const Fmt LtePhyPdcchDecodingResult_Fmt [] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v21 [] = {
        {UINT, "Subframe Number", 2},   // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {SKIP, NULL, 3},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v42 [] = {
        {UINT, "Subframe Number", 4},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
        {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
        {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
        {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
        {PLACEHOLDER, "DL CP", 0},  // 2 bits
        {PLACEHOLDER, "SSC", 0},    // 2 bits
        {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
        {SKIP, NULL, 1},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v43 [] = {
        {UINT, "Num Records", 4},   // 4 bits
        {PLACEHOLDER, "Subframe Number", 0},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 4 bits, (x-1)*5
        {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
        {PLACEHOLDER, "Num eNB Antennas", 0},   // 3 bits
        {SKIP, NULL, 2},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
};

const Fmt LtePhyPdcchDecodingResult_Payload_v101 [] = {
        {UINT, "Subframe Number", 4},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
        {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
        {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
        {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
        {PLACEHOLDER, "DL CP", 0},  // 2 bits
        {PLACEHOLDER, "SSC", 0},    // 2 bits
        {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
        {SKIP, NULL, 1},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v121 [] = {
        {UINT, "Subframe Number", 4},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
        {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
        {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
        {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
        {PLACEHOLDER, "DL CP", 0},  // 2 bits
        {PLACEHOLDER, "SSC", 0},    // 2 bits
        {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
        {UINT, "__pad", 1},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Record_v43 [] = {
        {UINT, "Subframe Offset", 2},
        {UINT, "CIF Configured", 2},    // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bit
        {PLACEHOLDER, "Number of Hypothesis", 0},   // 8 bits
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v21 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0},  // 3 bits
        {PLACEHOLDER, "Search Space Type", 0},  // 1 bit
        {PLACEHOLDER, "DCI Format", 0}, // 4 bits
        {PLACEHOLDER, "Decode Status", 0},  // 4 bits
        {PLACEHOLDER, "Start CCE", 0},  // 7 bits
        {PLACEHOLDER, "Payload Size", 0},   // 8 bits
        {PLACEHOLDER, "Tail Match", 0}, // 1 bit
        {UINT, "Prune Status", 2},
        {SKIP, NULL, 2},
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v24 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0},  // 3 bits
        {PLACEHOLDER, "Search Space Type", 0},  // 1 bit
        {PLACEHOLDER, "DCI Format", 0}, // 4 bits
        {PLACEHOLDER, "Decode Status", 0},  // 4 bits
        {PLACEHOLDER, "Start CCE", 0},  // 7 bits
        {PLACEHOLDER, "Payload Size", 0},   // 8 bits
        {PLACEHOLDER, "Tail Match", 0}, // 1 bit
        {UINT, "Prune Status", 1},

        {UINT, "Rmax",1},
        {PLACEHOLDER, "Hypothesis Repetition",0},
        {UINT, "Decoded Repetition",1},
        {PLACEHOLDER, "Mpdcch Group",0},
        {UINT, "Nb",1},

        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v42 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  //  11 bits
        {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v43 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  //  11 bits
        {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v101 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  //  11 bits
        {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v121 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Alt TBS Enabled", 0}, // 1 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  //  11 bits
        {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};


const Fmt LtePhyPdcchDecodingResult_Payload_v123 [] = {
        {UINT, "Subframe Number", 4},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
        {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
        {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
        {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
        {PLACEHOLDER, "DL CP", 0},  // 2 bits
        {PLACEHOLDER, "SSC", 0},    // 2 bits
        {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
        {UINT, "__pad", 1},
        {UINT, "Demback Mode Select", 1},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v123 [] = {
        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Alt TBS Enabled", 0}, // 1 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  //  11 bits
        {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};


const Fmt LtePhyPdcchDecodingResult_Payload_v141 [] = {
        {UINT, "Carrier Index", 2},  // shfit 0 bit, 4 bits
        {PLACEHOLDER, "Number of Records", 0}, //shift 4, total 9 bits
        {SKIP,NULL,1},
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v141 [] = {
        {UINT, "Subframe Number", 4},    // 4 bits
        {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
        {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
        {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
        {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
        {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
        {PLACEHOLDER,"Frame Structure",0}, 	// shift 20 bits,total 3 bits
        {PLACEHOLDER,"Num eNB Antennas",0}, 	// shift 23 bits,total 1 bits
        {PLACEHOLDER,"DL CP",0}, 	// shift 24 bits,total 1 bits
        {PLACEHOLDER,"SSC",0}, 	// shift 25 bits,total 4 bits
        {PLACEHOLDER,"CA FDD TDD",0}, 	// shift 29 bits,total 1 bits
        //{UINT, "__pad", 1},
        //{UINT, "Demback Mode Select", 1},   // 4 bits
        //{PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        //{UINT, "Number of Hypothesis", 1},

        {BYTE_STREAM, "Payload", 8},
        {UINT, "Aggregation Level", 4}, // 2 bits
        {PLACEHOLDER, "Candidate", 0}, // 3 bits
        {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
        {PLACEHOLDER, "DCI Format", 0},    // 4 bits
        {PLACEHOLDER, "Decode States", 0}, // 4 bits
        {PLACEHOLDER, "Payload Size", 0},  // 8 bits
        {PLACEHOLDER, "Tail Match", 0},    // 1 bit
        {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
        {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
        {PLACEHOLDER, "Alt TBS Enabled", 0}, // 1 bits
        {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
        {PLACEHOLDER, "Normal", 0},     // 15 bits
        {UINT, "Prune Status", 4},  // shift 0 bit, total 8 bits
        {PLACEHOLDER, "Energy Metric", 0},  // shift 8 bits total 21 bits
        {UINT, "Norm Energy Metric", 4},    // x/65535.0
        {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};


const Fmt LtePhyPucchCsf_Fmt [] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyPucchCsf_Payload_v22 [] = {
        {UINT, "Start System Sub-frame Number", 2}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {PLACEHOLDER, "Size BWP",0},
        {UINT, "Number of Subbands", 4},    // 4 bits
        {PLACEHOLDER, "BWP Index",0},
        {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
        {PLACEHOLDER, "SubBand Label",0},
        {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
};

const Fmt LtePhyPucchCsf_Payload_v24 [] = {
        {UINT, "Start System Sub-frame Number", 2}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {PLACEHOLDER, "Size BWP",0},
        {UINT, "Number of Subbands", 4},    // 4 bits
        {PLACEHOLDER, "BWP Index",0},
        {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
        {PLACEHOLDER, "SubBand Label",0},
        {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
        {UINT, "Num Csirs Ports", 1},
        {PLACEHOLDER, "Wideband PMI1", 0},
        {UINT,"Pti",1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchCsf_Payload_v43 [] = {
        {UINT, "Start System Sub-frame Number", 2}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {UINT, "Number of Subbands", 4},    // 4 bits
        {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
        {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
        {SKIP, NULL, 4},
};

const Fmt LtePhyPucchCsf_Payload_v102 [] = {
        {UINT, "Start System Sub-frame Number", 2}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {UINT, "Number of Subbands", 4},    // 4 bits
        {PLACEHOLDER,"Alt Cqi Table Data",0},
        {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
        {UINT,"Num Csirs Ports",1},
        {UINT, "Csi Meas Set Index",1},
        {PLACEHOLDER, "Rank Index",0},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchCsf_Payload_v103 [] = {
        {UINT, "Start System Sub-frame Number", 2}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {UINT, "Number of Subbands", 4},    // 4 bits
        {PLACEHOLDER,"Alt Cqi Table Data",0},
        {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
        {UINT,"Num Csirs Ports",1},
        {UINT, "Csi Meas Set Index",1},
        {PLACEHOLDER, "Rank Index",0},
        {UINT, "Forced Max RI",1},
        {SKIP, NULL, 1},
};

const Fmt LtePhyPucchCsf_Payload_v142 [] = {
        {UINT, "Start System Sub-frame Number", 4}, // 4 bits
        {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
        {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
        {PLACEHOLDER, "Scell Index",0},     // 5 bits
        {PLACEHOLDER, "PUCCH Reporting Mode", 0},  //reserve 1 bit, size 2
        {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
        {PLACEHOLDER, "Size BWP",0}, //3 bits, 1 bit in next

        {UINT, "Number of Subbands", 1},    // right shift 1 bit, 4 bits
        {PLACEHOLDER, "BWP Index",0}, //3 bits

        {UINT,"Alt Cqi Table Data",2}, //1 bit
        {PLACEHOLDER, "SubBand Label",0}, //2 bits
        {PLACEHOLDER, "CQI CW0", 0},   // 4 bits
        {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
        {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits

        {UINT, "CSF Tx Mode", 2},    // 4 bits
        {PLACEHOLDER, "PUCCH Reporting Submode", 0},  //size 2
        {PLACEHOLDER,"Num Csirs Ports",0}, //4 bits
        {PLACEHOLDER, "Wideband PMI1",0},  //4 bits
        {PLACEHOLDER, "Pti",0}, //1 bit
        {PLACEHOLDER, "Csi Meas Set Index",0}, //1 bit

        {UINT, "Rank Index",2}, //2 bits
        {PLACEHOLDER, "CRI",0}, //3 bits
        {PLACEHOLDER, "UL Frame Number",0}, //10 bits
        {PLACEHOLDER, "UL Channel Type",0}, //1 bit

        {UINT, "UL Subframe Number",4}, //4 bits
        {PLACEHOLDER, "UL Payload Length",0}, //5 bits,
};

const ValueName ValueNameCSFTxMode[] = {
        {0, "TM_Invalid"},
        {1, "TM_Single_Ant_Port_0"},
        {2, "TM_TD_Rank_1"},
        {3, "TM_OL_SM"},
        {4, "TM_CL_SM"},
        {5, "TM_MU_MIMO"},
        {6, "TM_CL_Rank_1_PC"},
        {7, "TM_Single_Ant_Port_5"},
};

const Fmt LtePhyPucchTxReport_Fmt [] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyPucchTxReport_Payload_v21 [] = {
        {UINT, "Serving Cell ID", 2},   // 9 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v21 [] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "CQI Payload", 2},
        {UINT, "Format", 4},    // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
        {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
        {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
        {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
        {PLACEHOLDER, "UE SRS", 0}, // 1 bit
        {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
        {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
        {PLACEHOLDER, "ACK Payload", 0},    // 2 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v21 [] = {
        {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v21 [] = {
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
        {UINT, "Tx Resampler", 4},  // X / 4294967295
        {UINT, "n_1_pucch", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p2_v22 [] = {
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
        {UINT, "Tx Resampler", 4},  // X / 4294967295
        {UINT, "n_1_pucch", 2},
        {SKIP, NULL, 2},

        //added different from v21
        {UINT, "Num Repetition",4},
        {PLACEHOLDER, "RB NB Start Index",0},
};

const Fmt LtePhyPucchTxReport_Payload_v43 [] = {
        {UINT, "Serving Cell ID", 2},   // 9 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v43 [] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "CQI Payload", 2},
        {UINT, "Format", 4},    // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
        {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
        {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
        {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
        {PLACEHOLDER, "UE SRS", 0}, // 1 bit
        {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
        {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v43 [] = {
        {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v43 [] = {
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
        {UINT, "ACK Payload", 4},   // 20 bits
        {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
        {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
        {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
        {UINT, "n_1_pucch", 2},
        {UINT, "Ack Nak Index", 2}, // 12 bits
        {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
        {PLACEHOLDER, "Csf Late", 0},   // 1 bit
        {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
        {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v101 [] = {
        {UINT, "Serving Cell ID", 2},   // 9 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v101 [] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "CQI Payload", 2},
        {UINT, "Carrier Index", 4}, // 2 bits
        {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
        {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
        {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
        {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
        {PLACEHOLDER, "UE SRS", 0}, // 1 bit
        {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
        {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v101 [] = {
        {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v101 [] = {
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
        {UINT, "ACK Payload", 4},   // 20 bits
        {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
        {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
        {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
        {UINT, "n_1_pucch", 2},
        {UINT, "Ack Nak Index", 2}, // 12 bits
        {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
        {PLACEHOLDER, "Csf Late", 0},   // 1 bit
        {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
        {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v121 [] = {
        {UINT, "Serving Cell ID", 2},   // 9 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v121 [] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "CQI Payload", 2},
        {UINT, "Carrier Index", 4}, // 2 bits
        {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
        {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
        {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
        {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
        {PLACEHOLDER, "UE SRS", 0}, // 1 bit
        {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
        {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v121 [] = {
        {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v121 [] = {
//add DL Carrier Index
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
        {UINT, "ACK Payload", 4},   // 20 bits
        {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
        {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
        {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
        {PLACEHOLDER, "DL Carrier Index", 0},    // 4 bits
        {UINT, "n_1_pucch", 2},
        {UINT, "Ack Nak Index", 2}, // 12 bits
        {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
        {PLACEHOLDER, "Csf Late", 0},   // 1 bit
        {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
        {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v142 [] = {
        {UINT, "Serving Cell ID", 2},   // 9 bits
        {PLACEHOLDER, "Number of Records", 0},  // 5 bits
        {SKIP, NULL, 1},
        {UINT, "Dispatch SFN SF", 2},
        {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v142 [] = {
        {UINT, "Current SFN SF", 2},
        {UINT, "CQI Payload", 2},
        {UINT, "Carrier Index", 4}, // 2 bits
        {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
        {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
        {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
        {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
        {PLACEHOLDER, "UE SRS", 0}, // 1 bit
        {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
        {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v142 [] = {
        {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v142 [] = {
//add DL Carrier Index
        {UINT, "PUCCH Digital Gain (dB)", 1},
        {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits

        {BYTE_STREAM, "ACK Payload[0]",4},
        {BYTE_STREAM, "ACK Payload[1]",4},
        {BYTE_STREAM, "ACK Payload[2]",4},
        {BYTE_STREAM, "ACK Payload[3]",4},

        {UINT, "ACK Payload Length", 1}, // 5 bits
        {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit

        {UINT, "Num DL Carriers", 2},    // 4 bits
        {PLACEHOLDER, "DL Carrier Index", 0},    // 4 bits
        {PLACEHOLDER, "Num RB",0},
        {PLACEHOLDER, "CDM Index",0},
        {SKIP,NULL,1},

        {UINT, "n_1_pucch", 2},
        {UINT, "Ack Nak Index", 2}, // 12 bits
        {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
        {PLACEHOLDER, "Csf Late", 0},   // 1 bit
        {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
        {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const ValueName ValueNamePucchReportingMode_v22[] = {
        {1, "MODE_1_1"},
};

const ValueName ValueNamePucchReportingMode[] = {
        {2, "MODE_1_1"},
};

const ValueName ValueNamePucchReportType_v22[] = {
        {1, "Type 2, Wideband CQI, PMI Feedback"},
        {2, "Type 3, RI Feedback"},
};

const ValueName ValueNamePucchReportType[] = {
        {2, "Type 2, Wideband CQI, PMI Feedback"},
        {3, "Type 3, RI Feedback"},
};

const ValueName ValueNameCsiMeasSetIndex[] = {
        {0, "CSI0"},
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

const Fmt LteMacULBufferStatusInternalFmt[] = {
        {UINT, "Version",    1},
        {UINT, "Num SubPkt", 1},
        {SKIP, NULL,         2}
};

const Fmt LteMacULBufferStatusInternal_SubpktHeaderFmt[] = {
        {UINT, "SubPacket ID",   1},
        {UINT, "Version",        1},
        {UINT, "SubPacket Size", 2},
        {UINT, "Num Samples",    1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt[] = {
        {UINT,        "Sub FN",                2},
        {PLACEHOLDER, "Sys FN",                0},
        {UINT,        "Number of active LCID", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt[] = {
        {UINT, "Ld Id",      1},
        {UINT, "Priority",   1},
        {UINT, "New bytes",  4},
        {UINT, "Retx bytes", 4},
        {UINT, "Ctrl bytes", 2},
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24[] = {
        {UINT,        "Sub Id",                1},
        {UINT,        "Sub FN",                2},
        {PLACEHOLDER, "Sys FN",                0},
        {UINT,        "Number of active LCID", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24[] = {
        {UINT,        "Ld Id",                  1},
        {UINT,        "Priority",               1},
        {UINT,        "New Uncompressed Bytes", 4},
        {UINT,        "New Compressed Bytes",   4},
        {UINT,        "Retx bytes",             4},
        {UINT,        "Ctrl bytes",             2},
        {PLACEHOLDER, "Total Bytes",            0},
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v5[] = {
        {UINT,        "Sub FN",                2},
        {PLACEHOLDER, "Sys FN",                0},
        {UINT,        "Number of active LCID", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v5[] = {
        {UINT,        "Ld Id",                  1},
        {UINT,        "Priority",               1},
        {UINT,        "New Uncompressed Bytes", 4},
        {UINT,        "New Compressed Bytes",   4},
        {UINT,        "Retx bytes",             4},
        {UINT,        "Ctrl bytes",             2},
        {PLACEHOLDER, "Total Bytes",            0},
};




const Fmt LteRrcServCellInfoLogPacketFmt[] = {
        {UINT, "Version", 1},
};


const Fmt LteRrcServCellInfoLogPacketFmt_v2[] = {
        {UINT,      "Cell ID",            2},
        {UINT,      "Downlink frequency", 2},
        {UINT,      "Uplink frequency",   2},
        {BANDWIDTH, "Downlink bandwidth", 1},
        {BANDWIDTH, "Uplink bandwidth",   1},
        {UINT,      "Cell Identity",      4},
        {UINT,      "TAC",                2},
        {UINT,      "Band Indicator",     4},
        {UINT,      "MCC",                2},
        {UINT,      "MNC Digit",          1},
        {UINT,      "MNC",                2},
        {UINT,      "Allowed Access",     1}
};


const Fmt LteRrcServCellInfoLogPacketFmt_v3[] = {
        {UINT,      "Cell ID",            2},
        {UINT,      "Downlink frequency", 4},
        {UINT,      "Uplink frequency",   4},
        {BANDWIDTH, "Downlink bandwidth", 1},
        {BANDWIDTH, "Uplink bandwidth",   1},
        {UINT,      "Cell Identity",      4},
        {UINT,      "TAC",                2},
        {UINT,      "Band Indicator",     4},
        {UINT,      "MCC",                2},
        {UINT,      "MNC Digit",          1},
        {UINT,      "MNC",                2},
        {UINT,      "Allowed Access",     1}
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
