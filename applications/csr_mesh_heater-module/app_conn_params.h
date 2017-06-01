/******************************************************************************
 *  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
 *  Bluetooth Low Energy CSRmesh 2.1
 *  Application version 2.1.0
 *
 *  FILE
 *      app_conn_params.h
 *
 *  DESCRIPTION
 *      MACROs for connection and advertisement parameter values
 *
 *****************************************************************************/

#ifndef __APP_CONN_PARAMS_H__
#define __APP_CONN_PARAMS_H__

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Maximum number of connection parameter update requests that can be send when 
 * connected
 */
#define MAX_NUM_CONN_PARAM_UPDATE_REQS       (4)

/* Brackets should not be used around the value of a macros used in db files. 
 * The parser which creates .c and .h files from .db file doesn't understand 
 * brackets and will raise syntax errors.
 */

/* Preferred connection parameter values should be within the range specified 
 * in the Bluetooth specification.
 */

/* Minimum and maximum connection interval in number of frames */
#define PREFERRED_MAX_CON_INTERVAL          96 /* 120 ms */
#define PREFERRED_MIN_CON_INTERVAL          72 /*  90 ms */

/* Slave latency in number of connection intervals */
#define PREFERRED_SLAVE_LATENCY             0x0000 /* 0 conn_intervals */

/* Supervision time-out (ms) = PREFERRED_SUPERVISION_TIMEOUT * 10 ms */
#define PREFERRED_SUPERVISION_TIMEOUT       0x0258 /* 6 seconds */


/* APPLE Compliant connection parameters */
/* Minimum and maximum connection interval in number of frames. */
#define APPLE_MAX_CON_INTERVAL              96 /* 120 ms */
#define APPLE_MIN_CON_INTERVAL              72 /*  90 ms */

/* Slave latency in number of connection intervals. */
#define APPLE_SLAVE_LATENCY                 0x0000 /* 0 conn_intervals. */

/* Supervision time-out (ms) = PREFERRED_SUPERVISION_TIMEOUT * 10 ms */
#define APPLE_SUPERVISION_TIMEOUT           0x0258 /* 6 seconds */

/* OTAu connection parameters */
/* Minimum and maximum connection interval in number of frames. */
#define OTAU_MAX_CON_INTERVAL              0x0008 /* 10 ms */
#define OTAU_MIN_CON_INTERVAL              0x0006 /* 7.5 ms */

/* Slave latency in number of connection intervals. */
#define OTAU_SLAVE_LATENCY                 0x0000 /* 0 conn_intervals. */

/* Supervision time-out (ms) = PREFERRED_SUPERVISION_TIMEOUT * 10 ms */
#define OTAU_SUPERVISION_TIMEOUT           0x0064 /* 1 second */

/* APPLE Compliant connection parameters for faster upgrades via GAIA OTAU */
/* Minimum and maximum connection interval in number of frames. */
#define OTAU_APPLE_MAX_CON_INTERVAL         0x0020 /* 40 ms */
#define OTAU_APPLE_MIN_CON_INTERVAL         0x0010 /* 20 ms */

/* Slave latency in number of connection intervals. */
#define OTAU_APPLE_SLAVE_LATENCY            0x0000 /* 0 conn_intervals. */

/* Supervision timeout (ms) = PREFERRED_SUPERVISION_TIMEOUT * 10 ms */
#define OTAU_APPLE_SUPERVISION_TIMEOUT      0x0064 /* 1 second */

/* OTAu connection parameters */
/* Minimum and maximum connection interval in number of frames. */
#define OTAU_CENTRAL_MAX_CON_INTERVAL              0x0008 /* 10 ms */
#define OTAU_CENTRAL_MIN_CON_INTERVAL              0x0006 /* 7.5 ms */

/* Slave latency in number of connection intervals. */
#define OTAU_CENTRAL_SLAVE_LATENCY             0x0000 /* 0 conn_intervals. */

/* Supervision time-out (ms) = PREFERRED_SUPERVISION_TIMEOUT * 10 ms */
#define OTAU_CENTRAL_SUPERVISION_TIMEOUT        0x0064 /* 1 second */

#endif /* __APP_CONN_PARAMS_H__ */
