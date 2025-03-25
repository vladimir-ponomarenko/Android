#include <ws/config.h>
#include <glib.h>
#include <ws/epan/epan.h>
#include <ws/epan/packet.h>
#include <ws/epan/dissectors/packet-pdcp-lte.h>

#include <stdio.h>
#include "packet-aww.h"

static const int PROTO_MAX = 1000;

static int proto_aww = -1;
static int proto_pdcp_lte = -1;

static dissector_table_t proto_table = NULL;

static int hf_aww_data_len  = -1;
static int hf_aww_proto     = -1;

static gint ett_aww     = -1;
static gint ett_payload = -1;


// Keep consistent with ws_wrap.cpp
static void init_proto_names (const char *protos []) {
    protos[100] = "rrc.ul.ccch";
    protos[101] = "rrc.ul.dcch";
    protos[102] = "rrc.dl.ccch";
    protos[103] = "rrc.dl.dcch";
    protos[104] = "rrc.bcch.bch";
    protos[106] = "rrc.pcch";

    protos[150] = "rrc.si.mib";
    protos[151] = "rrc.si.sib1";
    protos[152] = "rrc.si.sib2";
    protos[153] = "rrc.si.sib3";
    protos[155] = "rrc.si.sib5";
    protos[157] = "rrc.si.sib7";
    protos[162] = "rrc.si.sib12";
    protos[169] = "rrc.si.sib19";

    protos[181] = "rrc.si.sb1";

    protos[190] = "gsm_a_dtap";

    protos[200] = "lte-rrc.pcch";
    protos[201] = "lte-rrc.dl.dcch";
    protos[202] = "lte-rrc.ul.dcch";
    protos[203] = "lte-rrc.bcch.dl.sch";
    protos[204] = "lte-rrc.dl.ccch";
    protos[205] = "lte-rrc.ul.ccch";

    protos[250] = "nas-eps_plain";

    protos[300] = "pdcp-lte"; // signaling plane - Downlink
    protos[301] = "pdcp-lte"; // signaling plane - Uplink
}

// Return: new offset or -1 (if error occurs)
static gint32
dissect_extra_pdcp_lte_info(tvbuff_t *tvb, packet_info *pinfo, gint32 offset)
{
    struct pdcp_lte_info *p_pdcp_lte_info;
    guint8                tag                    = 0;
    gboolean              infoAlreadySet         = FALSE;
    gboolean              seqnumLengthTagPresent = FALSE;

    /* Needs to be at least as long as:
       - the signature string
       - fixed header bytes
       - tag for data
       - at least one byte of PDCP PDU payload */
    if (tvb_captured_length_remaining(tvb, offset) < (gint)(strlen(PDCP_LTE_START_STRING)+3+2)) {
        return -1; // TODO: move to a separate function
    }

    /* OK, compare with signature string */
    if (tvb_strneql(tvb, offset, PDCP_LTE_START_STRING, strlen(PDCP_LTE_START_STRING)) != 0) {
        return -1;
    }
    offset += (gint) strlen(PDCP_LTE_START_STRING);

    /* If redissecting, use previous info struct (if available) */
    p_pdcp_lte_info = (pdcp_lte_info *) p_get_proto_data(wmem_file_scope(), pinfo, proto_pdcp_lte, 0);
    if (p_pdcp_lte_info == NULL) {
        /* Allocate new info struct for this frame */
        p_pdcp_lte_info = wmem_new0(wmem_file_scope(), pdcp_lte_info);
        infoAlreadySet = FALSE;
    }
    else {
        infoAlreadySet = TRUE;
    }


    /* Read fixed fields */
    p_pdcp_lte_info->no_header_pdu = (gboolean)tvb_get_guint8(tvb, offset++);
    p_pdcp_lte_info->plane = (enum pdcp_plane)tvb_get_guint8(tvb, offset++);
    if (p_pdcp_lte_info->plane == SIGNALING_PLANE) {
        p_pdcp_lte_info->seqnum_length = PDCP_SN_LENGTH_5_BITS;
    }
    p_pdcp_lte_info->rohc.rohc_compression = (gboolean)tvb_get_guint8(tvb, offset++);

    /* Read optional fields */
    while (tag != PDCP_LTE_PAYLOAD_TAG) {
        /* Process next tag */
        tag = tvb_get_guint8(tvb, offset++);
        switch (tag) {
            case PDCP_LTE_SEQNUM_LENGTH_TAG:
                p_pdcp_lte_info->seqnum_length = tvb_get_guint8(tvb, offset);
                offset++;
                seqnumLengthTagPresent = TRUE;
                break;
            case PDCP_LTE_DIRECTION_TAG:
                p_pdcp_lte_info->direction = tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_LOG_CHAN_TYPE_TAG:
                p_pdcp_lte_info->channelType = (LogicalChannelType)tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_BCCH_TRANSPORT_TYPE_TAG:
                p_pdcp_lte_info->BCCHTransport = (BCCHTransportType)tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_IP_VERSION_TAG:
                p_pdcp_lte_info->rohc.rohc_ip_version = tvb_get_ntohs(tvb, offset);
                offset += 2;
                break;
            case PDCP_LTE_ROHC_CID_INC_INFO_TAG:
                p_pdcp_lte_info->rohc.cid_inclusion_info = tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_LARGE_CID_PRES_TAG:
                p_pdcp_lte_info->rohc.large_cid_present = tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_MODE_TAG:
                p_pdcp_lte_info->rohc.mode = (enum rohc_mode)tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_RND_TAG:
                p_pdcp_lte_info->rohc.rnd = tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_UDP_CHECKSUM_PRES_TAG:
                p_pdcp_lte_info->rohc.udp_checksum_present = tvb_get_guint8(tvb, offset);
                offset++;
                break;
            case PDCP_LTE_ROHC_PROFILE_TAG:
                p_pdcp_lte_info->rohc.profile = tvb_get_ntohs(tvb, offset);
                offset += 2;
                break;
            case PDCP_LTE_CHANNEL_ID_TAG:
                p_pdcp_lte_info->channelId = tvb_get_ntohs(tvb, offset);
                offset += 2;
                break;
            case PDCP_LTE_UEID_TAG:
                p_pdcp_lte_info->ueid = tvb_get_ntohs(tvb, offset);
                offset += 2;
                break;

            case PDCP_LTE_PAYLOAD_TAG:
                /* Have reached data, so get out of loop */
                continue;

            default:
                /* It must be a recognised tag */
                return -1;
        }
    }

    if ((p_pdcp_lte_info->plane == USER_PLANE) && (seqnumLengthTagPresent == FALSE)) {
        /* Conditional field is not present */
        return -1;
    }

    if (!infoAlreadySet) {
        /* Store info in packet */
        p_add_proto_data(wmem_file_scope(), pinfo, proto_pdcp_lte, (guint32) 0, p_pdcp_lte_info);
    }

    return offset;
}
static void
dissect_aww(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "Automator Wireshark Wrapper");
    /* Clear out stuff in the info column */
    col_clear(pinfo->cinfo, COL_INFO);

    if (tree) { /* we are being asked for details */
        proto_item *ti = NULL;
        proto_tree *aww_tree = NULL;

        ti = proto_tree_add_item(tree, proto_aww, tvb, 0, -1, ENC_NA);
        aww_tree = proto_item_add_subtree(ti, ett_aww);
        proto_tree_add_item(aww_tree, hf_aww_proto, tvb, 0, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(aww_tree, hf_aww_data_len, tvb, 4, 4, ENC_BIG_ENDIAN);

        guint32 proto_id = tvb_get_ntohl(tvb, 0);
        gint32 offset = 8;
        // pdcp-lte
        if (proto_id == 300 || proto_id == 301) {
            offset = dissect_extra_pdcp_lte_info(tvb, pinfo, offset);
        }
        if (offset == -1) {
            fprintf(stderr, "Error: ws_dissector fails to decode extra info for LTE PDCP packets.\n");
        } else {
            tvbuff_t* next_tvb = tvb_new_subset_remaining(tvb, offset);
            int rtBytesConsumed = dissector_try_uint(proto_table, proto_id, next_tvb, pinfo, tree);
            if (rtBytesConsumed == 0) {
                fprintf(stderr, "Error: ws_dissector fails to decode protocol %d.\n", proto_id);
            }
        }
    }
}

void
proto_register_aww(void)
{
    proto_aww = proto_register_protocol(
            "Automator Wireshark Wrapper",
            "AWW",
            "aww"
    );

    static hf_register_info hf[] = {
            { &hf_aww_proto,
                    { "Protocol", "aww.proto", FT_UINT32, BASE_DEC,
                            NULL, 0x0, "aww protocol", HFILL }
            },
            { &hf_aww_data_len,
                    { "Data length", "aww.data_len", FT_UINT32, BASE_DEC,
                            NULL, 0x0, "aww protocol data length", HFILL }
            }
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
            &ett_aww,
            &ett_payload
    };

    // subdissector table setup
    proto_table = register_dissector_table(
            "aww.proto",
            "AWW protocol",
            FT_UINT16,
            BASE_DEC
    );

    register_dissector("aww", dissect_aww, proto_aww);
    proto_register_field_array(proto_aww, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    proto_pdcp_lte = proto_get_id_by_filter_name("pdcp-lte");
}

void
proto_reg_handoff_aww(void)
{
    const char *protos[PROTO_MAX + 1] = {};
    init_proto_names(protos);

    dissector_handle_t handle = NULL;
    for (int i = 0; i <= PROTO_MAX; i++) {
        if (protos[i] != NULL) {
            handle = NULL;
            handle = find_dissector(protos[i]);
            // printf ("[MoonSky]: find_dissector[%s]: ", protos[i]);
            // if (handle != NULL) {
            //     printf ("Success\n");
            // } else {
            //     printf ("Fail\n");
            // }
            dissector_add_uint("aww.proto", i, handle);
        }
    }
}