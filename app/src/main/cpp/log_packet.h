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
