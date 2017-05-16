/****************************************************************************

 i106_decode_index.c

 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdint.h"

#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_index.h"

#ifdef __cplusplus
namespace Irig106 {
#endif


// Function Declaration

EnI106Status vFillInMsgPtrs(SuIndex_CurrMsg * psuCurrMsg);


/* These Decode_First / Decode_Next routines take an index packet and return
 * successive root or node index entries.  Indexes are complicated with a lot
 * of optional components.  The SuIndex_CurrMsg will have pointers to packet
 * data that exists.  Optional index components that aren't in the current 
 * index packet will have their data pointers set to NULL.  It is important
 * to check the SuIndex_CurrMsg data pointers for the NULL value before trying
 * to use them.
 
 * Calls to decode a root index packet will fill in a non-NULL pointer for the 
 * psuRootInfo pointer until the end of the packet is reached.  The last call to 
 * decode a root index packet will fill in a NULL pointer for psuRootInfo, and 
 * will fill in a non-NULL pointer for psuNextRootOffset, the pointer to the 
 * offset of the next root packet.
 
 * Calls to decode a node index packet will fill in a non-NULL pointer for the
 * psuNodeInfo pointer. */

EnI106Status I106_CALL_DECL enI106_Decode_FirstIndex(SuI106Ch10Header * psuHeader, void * pvBuff, SuIndex_CurrMsg * psuMsg){
    EnI106Status    RetStatus;
    int64_t         lCurrOffset = 0;

    // Null everything out to start
    memset(psuMsg, 0 , sizeof(SuIndex_CurrMsg));

    // Set pointers to the beginning of the index buffer
    psuMsg->psuChanSpec = (SuIndex_ChanSpec *)pvBuff;
    lCurrOffset += sizeof(SuIndex_ChanSpec);

    // If there is a file size then make a pointer to it
    if (psuMsg->psuChanSpec->bFileSize == 0)
        psuMsg->piFileSize = NULL;
    else {
        psuMsg->piFileSize = (int64_t *)((char *)(pvBuff) + lCurrOffset);
        lCurrOffset += sizeof(int64_t);
    }
        
    // Set pointer to the beginning of the index array
    psuMsg->pvIndexArray = (char *)(pvBuff) + lCurrOffset;
    lCurrOffset += sizeof(int64_t);

    // Check for no messages
    psuMsg->uMsgNum = 0;
    if (psuMsg->psuChanSpec->uIdxEntCount == 0)
        return I106_NO_MORE_DATA;

    // Make sure the offset to the first index isn't beyond the end of the data buffer
    psuMsg->ulDataLen    = psuHeader->ulDataLen;
    if (lCurrOffset >= psuMsg->ulDataLen)
        return I106_BUFFER_OVERRUN;

    // Get the index data pointers
    RetStatus = vFillInMsgPtrs(psuMsg);

    return RetStatus;
}


EnI106Status I106_CALL_DECL enI106_Decode_NextIndex(SuIndex_CurrMsg * psuMsg){
    EnI106Status    RetStatus;

    // Check for no more messages
    psuMsg->uMsgNum++;
    if (psuMsg->uMsgNum >= psuMsg->psuChanSpec->uIdxEntCount)
        return I106_NO_MORE_DATA;

    // Get the index data pointers
    RetStatus = vFillInMsgPtrs(psuMsg);

    return RetStatus;
}


/* Fill in the pointers to the various index packet message data items. There
 * are three different kinds of messages, a node index message, a root index
 * message, and an offset to the next root index packet. */
EnI106Status vFillInMsgPtrs(SuIndex_CurrMsg * psuCurrMsg){
    EnI106Status RetStatus;

    // Process node index message
    if (psuCurrMsg->psuChanSpec->uIndexType == 1){
        // With optional secondary header time
        if (psuCurrMsg->psuChanSpec->bIntraPckHdr == 1){
            psuCurrMsg->psuTime         = &(((SuIndex_NodeMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suTime);
            psuCurrMsg->psuOptionalTime = &(((SuIndex_NodeMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suSecondaryTime);
            psuCurrMsg->plFileOffset    = &(((SuIndex_NodeMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].lOffset);
            psuCurrMsg->psuNodeData     = &(((SuIndex_NodeMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suData);
        }

        // Without optional secondary header time
        else {
            psuCurrMsg->psuTime         = &(((SuIndex_NodeMsg *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suTime);
            psuCurrMsg->psuOptionalTime = NULL;
            psuCurrMsg->plFileOffset    = &(((SuIndex_NodeMsg *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].lOffset);
            psuCurrMsg->psuNodeData     = &(((SuIndex_NodeMsg *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suData);
        }
        RetStatus = I106_INDEX_NODE;
    }

    // Process root index messages
    else {
        // With optional secondary header time
        if (psuCurrMsg->psuChanSpec->bIntraPckHdr == 1){
            psuCurrMsg->psuTime         = &(((SuIndex_RootMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suTime);
            psuCurrMsg->psuOptionalTime = &(((SuIndex_RootMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suSecondaryTime);
            psuCurrMsg->plFileOffset    = &(((SuIndex_RootMsgOptTime *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].lOffset);
            psuCurrMsg->psuNodeData     = NULL;
        }

        // Without optional secondary header time
        else {
            psuCurrMsg->psuTime         = &(((SuIndex_RootMsg *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].suTime);
            psuCurrMsg->psuOptionalTime = NULL;
            psuCurrMsg->plFileOffset    = &(((SuIndex_RootMsg *)psuCurrMsg->pvIndexArray)[psuCurrMsg->uMsgNum].lOffset);
            psuCurrMsg->psuNodeData     = NULL;
        }

        // If not the last message then it's a root index message
        if (psuCurrMsg->uMsgNum < psuCurrMsg->psuChanSpec->uIdxEntCount-1)
            RetStatus = I106_INDEX_ROOT;

        // Since it is the last message it's a pointer to next root packet
        else
            RetStatus = I106_INDEX_ROOT_LINK;
    }

    return RetStatus;
}


#ifdef __cplusplus
} // end namespace i106
#endif
