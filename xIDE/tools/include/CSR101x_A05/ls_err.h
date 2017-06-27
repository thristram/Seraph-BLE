/*! \file ls_err.h
 *  \brief  CSR1000 Upper Stack Link Supervisor error codes
 *
 *  Consisting of standard HCI codes (those in the range 0x00 - 0x3F) and
 *  extended codes (0x40 onwards).
 *
 * Copyright (c) 2010 - 2012 Qualcomm Technologies International, Ltd.
 * All Rights Reserved.
 * Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 */

#ifndef __LS_ERR_H__
#define __LS_ERR_H__

#include "types.h"
#include "hci_error_codes.h"

/*! \brief   HCI and extended error codes
 *
 *  Please refer to the BlueTooth specifications V4.0,
 *  volume 2, part D for details of the HCI error codes
 *  in the range 0x00 - 0x3F.
 *
 *  The extended error codes (those above 0x40) are documented here.
 */
typedef enum
#ifdef _WIN32
: uint16
#endif
{
    ls_err_none = 0,

    /* BT4 Vol2 PartD overlay HCI error codes */
    ls_err_illegal_command,               /* 0x01 */
    ls_err_no_connection,                 /* 0x02 */
    ls_err_hardware,                      /* 0x03 */
    ls_err_page_timeout,                  /* 0x04 */
    ls_err_authentication,                /* 0x05 */
    ls_err_key,                           /* 0x06 */
    ls_err_mem,                           /* 0x07 */
    ls_err_conn_timeout,                  /* 0x08 */
    ls_err_conn_limit,                    /* 0x09 */
    ls_err_sco_limit,                     /* 0x0A */
    ls_err_acl_limit,                     /* 0x0B */
    ls_err_not_allowed,                   /* 0x0C */
    ls_err_remote_resource,               /* 0x0D */
    ls_err_remote_security,               /* 0x0E */
    ls_err_remote_user,                   /* 0x0F */
    ls_err_host_timeout,                  /* 0x10 */
    ls_err_unsupported,                   /* 0x11 */
    ls_err_format,                        /* 0x12 */
    ls_err_oetc_user,                     /* 0x13 */
    ls_err_oetc_resource,                 /* 0x14 */
    ls_err_oetc_power,                    /* 0x15 */
    ls_err_conn_terminated,               /* 0x16 */
    ls_err_auth_repeated,                 /* 0x17 */
    ls_err_pairing_not_allowed,           /* 0x18 */
    ls_err_lmp_unknown,                   /* 0x19 */
    ls_err_remote_unsupported,            /* 0x1A */
    ls_err_sco_offs,                      /* 0x1B */
    ls_err_sco_interval,                  /* 0x1C */
    ls_err_sco_rejected,                  /* 0x1D */
    ls_err_lmp_invalid,                   /* 0x1E */
    ls_err_unspecified,                   /* 0x1F */
    ls_err_lmp_param,                     /* 0x20 */
    ls_err_role_change,                   /* 0x21 */
    ls_err_lmp_timeout,                   /* 0x22 */
    ls_err_lmp_collision,                 /* 0x23 */
    ls_err_lmp_unsupported,               /* 0x24 */
    ls_err_enc_not_allowed,               /* 0x25 */
    ls_err_unit_key,                      /* 0x26 */
    ls_err_qos_unsupported,               /* 0x27 */
    ls_err_instant,                       /* 0x28 */
    ls_err_unit_key_unsupported,          /* 0x29 */
    ls_err_collision,                     /* 0x2A */
    ls_err_scm_resources,                 /* 0x2B */
    ls_err_qos_param,                     /* 0x2C */
    ls_err_qos_rejected,                  /* 0x2D */
    ls_err_channel_class_unsupported,     /* 0x2E */
    ls_err_security,                      /* 0x2F */
    ls_err_range,                         /* 0x30 */
    ls_err_scm,                           /* 0x31 */
    ls_err_role_pending,                  /* 0x32 */
    ls_err_scm_pending,                   /* 0x33 */
    ls_err_slot_violation,                /* 0x34 */
    ls_err_role_failed,                   /* 0x35 */
    ls_err_inquiry_limit,                 /* 0x36 */
    ls_err_sp_unsupported,                /* 0x37 */
    ls_err_pair_pending,                  /* 0x38 */
    ls_err_con_reject,                    /* 0x39 */
    ls_err_controller_busy,               /* 0x3A */
    ls_err_conn_interval,                 /* 0x3B */
    ls_err_diradv_timeout,                /* 0x3C */
    ls_err_conn_mic,                      /* 0x3D */
    ls_err_conn_fail,                     /* 0x3E */
    ls_err_conn_mac,                      /* 0x3F */

    /* extended error codes - Starting from 0x40 */
    /*! 0x40 One or more arguements are in error, or incompatible. */
    ls_err_arg,
    /*! 0x41 Invalid role selected in advertising. */
    ls_err_mode,

    /*! 0x42 Failure due to buffer full condition in LE controller. */
    ls_err_lc_buf_full,

    /*! 0x43 Message received in Invalid LS Connection State. */
    ls_err_con_invalid_state,

    /*! 0x44 Connection parameter udpate rejected by remote device. This
     * error can only be received in Slave mode. */
    ls_err_con_param_rej_remote_dev,

    /*! 0x45 Connection Parameter Update Rejected as Slave device is not
     * allowed to transmit another Connection Parameter Update request
     * till time TGAP(conn_param_timeout). Refer to section 9.3.9.2, Vol
     * 3, Part C of the Core 4.0 BT spec. The application should retry the
     * 'connection paramter update' procedure after time
     * TGAP(conn_param_timeout). */
    ls_err_con_param_rej_tgap_violation,

    /*! 0x46 Connection parameter udpate procedure timeout - Master device
     * didn't respond to Connection Parameter Update request from
     * Slave device within GAP_TGAP_com_param_proc_timeout (30 secs)
     * period. */
    ls_err_con_param_timeout

} ls_err;

#endif  /* __LS_ERR_H__ */
