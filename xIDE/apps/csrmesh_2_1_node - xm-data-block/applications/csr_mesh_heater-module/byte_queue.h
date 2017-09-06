/*******************************************************************************
 *  Copyright (c) 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.6.1
 *  Application version 2.6.1.0
 *
 * FILE
 *     byte_queue.h
 *
 *  DESCRIPTION
 *     Interface to send circular buffer implementation.
 *
 ******************************************************************************/

#ifndef __BYTE_SENDQUEUE_H__
#define __BYTE_SENDQUEUE_H__

/*============================================================================*
 *  SDK Header includes
 *============================================================================*/

#include <types.h>

/*============================================================================*
 *  Public definitions
 *============================================================================*/
#define SEND_QUEUE_ID            (0)
#define RECV_QUEUE_ID            (1)

/*============================================================================*
 *  Public functions prototypes
 *============================================================================*/

/* Function that queue the supplied data if there is sufficient space available.
 * If there is not enough space FALSE is returned instead.
 */
extern bool BQSafeQueueBytes(const uint8 *p_data, uint16 len,uint8 queueid);

/* Function that queues the supplied data. If there is not enough space then
 * data at the head of the queue is overwritten and the head of the queue moved
 * to the end of the new data.
 */
extern void BQForceQueueBytes(const uint8 *p_data, uint16 len,uint8 queueid);

/* Function that return the total size of the buffer. */
extern uint16 BQGetBufferCapacity(uint8 queueid);

/* Function that return the amount of data currently in the queue.*/
extern uint16 BQGetDataSize(uint8 queueid);

/* Function that return the amount of free space available in the buffer.*/
extern uint16 BQGetAvailableSize(uint8 queueid);

/* Function that clears buffer contents leaving the queue empty. */
extern void BQClearBuffer(uint8 queueid);

/* Function that extract up to the specified number of bytes from the queue
 * increasing the available size by the number of bytes extracted. If not
 * enough data is held in the queue then the function returns immediately with
 * whatever data is available.
 */
extern uint16 BQPopBytes(uint8 *p_data, uint16 len,uint8 queueid);

/* Function that peeks up to the specified number of bytes from the queue,
 * without modifying the buffer. If not enough data is held in the queue then
 * the function returns immediately with whatever data is available.
 */
extern uint16 BQPeekBytes(uint8 *p_data, uint16 len,uint8 queueid);

/* Function that remove from the queue the data that was returned in the last
 * call to BQSPeekBytes.
 */
extern void BQCommitLastPeek(uint8 queueid);

#endif /* __BYTE_QUEUE_H__ */
