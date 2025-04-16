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
