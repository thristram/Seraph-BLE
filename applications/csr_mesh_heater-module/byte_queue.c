/*******************************************************************************
 *  Copyright (c) 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.6.1
 *  Application version 2.6.1.0
 *
 *  FILE
 *      byte_sendqueue.c
 *
 *  DESCRIPTION
 *      Circular buffer implementation.
 *
 *
 ******************************************************************************/
/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <mem.h>
/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "byte_queue.h"
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* Number of 512 bytes buffer required to store the data from UART/BLE */
#define BUFFER_SIZE              (256)

/* Largest amount of data that can be stored in the buffer */
#define BUFFER_LEN               (BUFFER_SIZE - 1)

/* Send and Receive queue only */
#define NO_OF_QUEUES             (2)

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* Queue data structure */
typedef struct _QUEUE_T
{
    /* Pointer to head of queue (next byte to be read out) */
    uint16 g_head ;

    /* Pointer to head of queue after committing most recent peek */
    uint16 g_peek ;
    
    /* Pointer to tail of queue (next byte to be inserted) */
    uint16 g_tail ;
}QUEUE_T;

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* Length of data currently held in queue */
#define QUEUE_LENGTH(id) \
  ((g_queue[id].g_tail >= g_queue[id].g_head) ? \
  g_queue[id].g_tail - g_queue[id].g_head \
  : BUFFER_SIZE - g_queue[id].g_head + g_queue[id].g_tail)

/* Amount of free space left in queue (= BUFFER_LEN - QUEUE_LENGTH) */
#define QUEUE_FREE(id) \
  ((g_queue[id].g_tail >= g_queue[id].g_head) ? \
  BUFFER_LEN - g_queue[id].g_tail + g_queue[id].g_head \
  : g_queue[id].g_head - g_queue[id].g_tail - 1)
  
/*============================================================================*
 *  Private Data
 *============================================================================*/

/* g_queue holds the head,peek,tail variables for send and recv queues */
static QUEUE_T g_queue[NO_OF_QUEUES];

/* receive queue buffer */
uint8 g_recv_queue[BUFFER_SIZE];

/* send queue buffer */
uint8 g_send_queue[BUFFER_SIZE];

      
/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* Append the supplied data to the queue */
static void copyIntoBuffer(const uint8 *p_data, uint16 len,uint8 queue_id);

/* Read up to the requested number of bytes out of the queue */
static uint16 peekBuffer(uint8 *p_data, uint16 len,uint8 queue_idx);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      copyIntoBuffer
 *
 *  DESCRIPTION
 *      Copy a given number of bytes in to the buffer. Assumes there is enough
 *      space available in the buffer. If not, the existing data will be
 *      overwritten to accommodate the new data.
 *
 *      At the end of the function g_head points to the oldest queue entry and
 *      g_tail the next insertion point.
 *
 * PARAMETERS
 *      p_data   [in]     Pointer to the data to be copied
 *      len      [in]     Number of bytes of data to be copied
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
static void copyIntoBuffer(const uint8 *p_data, uint16 len,uint8 queue_id)
{
    /* Sanity check */
    if ((len == 0) || (p_data == NULL))
        return;

    /* No point copying more data into the queue than the queue can hold */
    if (len > BUFFER_LEN)
    {
        /* Advance input pointer to the last BUFFER_LEN bytes */
        p_data += len - BUFFER_LEN;

        /* Adjust len */
        len = BUFFER_LEN;
    }

    /* Check whether the queue will overflow */
    if (len > QUEUE_FREE(queue_id))
    {
        /* Advance g_head to point to the oldest item, after the overflow*/
        g_queue[queue_id].g_head += len - (QUEUE_FREE(queue_id));

        /* If this goes past the end of the buffer, wrap around */
        if (g_queue[queue_id].g_head >= BUFFER_SIZE)
            g_queue[queue_id].g_head -= BUFFER_SIZE;

        /* Update g_peek similarly */
        g_queue[queue_id].g_peek = g_queue[queue_id].g_head;
    }

    /* Check whether we're going past the end of the buffer */
    if (g_queue[queue_id].g_tail + len >= BUFFER_SIZE)
    {
        /* Calculate how much space there is till the end of the buffer */
        const uint16 available = BUFFER_SIZE - g_queue[queue_id].g_tail;

        /* Copy data into the queue up to end of buffer */
        if(queue_id == SEND_QUEUE_ID)
        {
            MemCopy(&g_send_queue[g_queue[queue_id].g_tail], p_data, available);
        }
        else
        {
            MemCopy(&g_recv_queue[g_queue[queue_id].g_tail], p_data, available);
        }

        /* Update g_tail */
        g_queue[queue_id].g_tail = len - available;

        /* Copy data into the queue up to end of buffer */
        if(queue_id == SEND_QUEUE_ID)
        {
            MemCopy(g_send_queue, p_data + available, g_queue[queue_id].g_tail);
        }
        else
        {
            MemCopy(g_recv_queue, p_data + available, g_queue[queue_id].g_tail);
        }
    }
    else
    {
        /* Copy data into the queue up to end of buffer */
        if(queue_id == SEND_QUEUE_ID)
        {
           MemCopy(&g_send_queue[g_queue[queue_id].g_tail], p_data, len);
        } 
        else
        {
           MemCopy(&g_recv_queue[g_queue[queue_id].g_tail], p_data, len);
        }

        /* Update g_tail */
        g_queue[queue_id].g_tail += len;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      peekBuffer
 *
 *  DESCRIPTION
 *      Read a given number of bytes from the buffer without removing any data.
 *      If more data is requested than is available, then only the available
 *      data is read.
 *
 * PARAMETERS
 *      p_data [in]     Pointer to buffer to store read data in
 *      len    [in]     Number of bytes of data to peek
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Number of bytes of data peeked.
 *----------------------------------------------------------------------------*/
static uint16 peekBuffer(uint8 *p_data, uint16 len,uint8 queue_id)
{
    uint16 peeked = len;    /* Number of bytes of data peeked */

    /* Sanity check */
    if ((len == 0) || (p_data == NULL))
        return 0;

    /* Cannot peek more data than is available */
    if (peeked> QUEUE_LENGTH(queue_id))
    {
       peeked = QUEUE_LENGTH(queue_id);
       
       if(peeked > len)
          peeked = len;
    }

    /* Check whether we're going past the end of the buffer */
    if (g_queue[queue_id].g_head + peeked >= BUFFER_SIZE)
    {
        /* Calculate how much space there is till the end of the buffer */
        const uint16 available = BUFFER_SIZE - g_queue[queue_id].g_head;

        if(queue_id == SEND_QUEUE_ID)
        {
            if(available > len)
            {
              /* Copy data up to end of buffer */
              MemCopy(p_data, &g_send_queue[g_queue[queue_id].g_head], len);
            }
            else
            {
              /* Copy data up to end of buffer */
              MemCopy(p_data,&g_send_queue[g_queue[queue_id].g_head],available);
            }
        }
        else
        {
            if(available>len)
            {
              /* Copy data up to end of buffer */
              MemCopy(p_data, &g_recv_queue[g_queue[queue_id].g_head], len);
            }
            else
            {
              /* Copy data up to end of buffer */
              MemCopy(p_data,&g_recv_queue[g_queue[queue_id].g_head],available);
            }
        }

        /* Update g_peek */
        g_queue[queue_id].g_peek = peeked - available;

        if(queue_id == SEND_QUEUE_ID)
        {
            /* Copy data from start of buffer */
            MemCopy(p_data + available,g_send_queue, g_queue[queue_id].g_peek);
        }
        else
        {
            /* Copy data from start of buffer */
            MemCopy(p_data + available,g_recv_queue, g_queue[queue_id].g_peek);
        }
    }
    else
    {
        if(queue_id == SEND_QUEUE_ID)
        {
            /* Peek data starting from the head of the queue */
            MemCopy(p_data, &g_send_queue[g_queue[queue_id].g_head], peeked);
        }
        else
        {
            /* Peek data starting from the head of the queue */
            MemCopy(p_data, &g_recv_queue[g_queue[queue_id].g_head], peeked);
        }
        
        /* Update g_peek */
        g_queue[queue_id].g_peek = g_queue[queue_id].g_head + peeked;
    }

    return peeked;
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQSafeQueueBytes
 *
 *  DESCRIPTION
 *      Queue the supplied data if there is sufficient space available.
 *      If there is not enough space FALSE is returned instead.
 *
 * PARAMETERS
 *      p_data [in]     Pointer to the data to be queued
 *      len    [in]     Number of bytes of data to be queued
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      TRUE if the data is queued successfully
 *      FALSE if there is not enough space in the queue
 *----------------------------------------------------------------------------*/
bool BQSafeQueueBytes(const uint8 *p_data, uint16 len,uint8 queue_id)
{
    /* Check whether there is enough space available in the buffer */
    bool ret_val = (QUEUE_FREE(queue_id) >= len);

    /* If so, copy the data into the buffer */
    if (ret_val)
    {
        copyIntoBuffer(p_data, len,queue_id);
    }
    return ret_val;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQForceQueueBytes
 *
 *  DESCRIPTION
 *      Queue the supplied data. If there is not enough space then data at the
 *      head of the queue is overwritten and the head of the queue moved to
 *      the end of the new data.
 *
 * PARAMETERS
 *      p_data [in]     Pointer to the data to be queued
 *      len    [in]     Number of bytes of data to be queued
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
void BQForceQueueBytes(const uint8 *p_data, uint16 len,uint8 queue_id)
{
    /* Copy data into the buffer whether or not space is available */
    copyIntoBuffer(p_data, len,queue_id);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQSGetBufferCapacity
 *
 *  DESCRIPTION
 *      Return the total size of the buffer.
 *
 * PARAMETERS
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Total buffer size in bytes
 *----------------------------------------------------------------------------*/
uint16 BQGetBufferCapacity(uint8 queue_id)
{
    return BUFFER_LEN;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQGetDataSize
 *
 *  DESCRIPTION
 *      Return the amount of data currently in the queue.
 *
 * PARAMETERS
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Size of data currently stored in the queue in bytes.
 *----------------------------------------------------------------------------*/
uint16 BQGetDataSize(uint8 queue_id)
{
    return QUEUE_LENGTH(queue_id);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQGetAvailableSize
 *
 *  DESCRIPTION
 *      Return the amount of free space available in the buffer.
 *
 * PARAMETERS
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Size of free space available in the buffer in bytes.
 *----------------------------------------------------------------------------*/
uint16 BQGetAvailableSize(uint8 queue_id)
{
    return QUEUE_FREE(queue_id);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQClearBuffer
 *
 *  DESCRIPTION
 *      Clear buffer contents leaving the queue empty.
 *
 * PARAMETERS
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
void BQClearBuffer(uint8 queue_id)
{
 /* Reset queue pointers */
 g_queue[queue_id].g_head=g_queue[queue_id].g_peek=g_queue[queue_id].g_tail=0;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQPopBytes
 *
 *  DESCRIPTION
 *      Extract up to the specified number of bytes from the queue increasing
 *      the available size by the number of bytes extracted. If not enough data
 *      is held in the queue then the function returns immediately with
 *      whatever data is available.
 *
 * PARAMETERS
 *      p_data   [out]    Pointer to a buffer to store the extracted data in
 *      len      [in]     Number of bytes of data to be extracted
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Number of bytes actually extracted, may be fewer than requested if not
 *      enough data is available.
 *----------------------------------------------------------------------------*/
uint16 BQPopBytes(uint8 *p_data, uint16 len,uint8 queue_id)
{
    /* Copy the data into the return buffer */
    uint16 peeked = peekBuffer(p_data,len,queue_id);

    /* Remove the peeked data from the queue */
    BQCommitLastPeek(queue_id);

    /* Return number of bytes peeked */
    return peeked;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQPeekBytes
 *
 *  DESCRIPTION
 *      Peek up to the specified number of bytes from the queue, without
 *      modifying the buffer. If not enough data is held in the queue then
 *      the function returns immediately with whatever data is available.
 *
 * PARAMETERS
 *      p_data [out]    Pointer to a buffer to store the peeked data in
 *      len    [in]     Number of bytes of data to be peeked
 *      queue_id [in]   Identifier of the Queue 
 *
 * RETURNS
 *      Number of bytes actually peeked, may be fewer than requested if not
 *      enough data is available.
 *----------------------------------------------------------------------------*/
uint16 BQPeekBytes(uint8 *p_data, uint16 len,uint8 queue_id)
{
    /* Peek into the buffer */
    return peekBuffer(p_data, len,queue_id);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BQCommitLastPeek
 *
 *  DESCRIPTION
 *      Remove from the queue the data that was returned in the last call to
 *      BQPeekBytes.
 *
 * PARAMETERS
 *      queue_id [in]     Identifier of the Queue 
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
void BQCommitLastPeek(uint8 queue_id)
{
    /* Update g_head to point to current g_peek location */
    g_queue[queue_id].g_head = g_queue[queue_id].g_peek;
}
