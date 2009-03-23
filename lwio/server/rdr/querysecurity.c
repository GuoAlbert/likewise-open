#include "rdr.h"

NTSTATUS
RdrTransactNtTransQuerySecurityDesc(
    PSMB_TREE pTree,
    USHORT usFid,
    SECURITY_INFORMATION securityInformation,
    PSECURITY_DESCRIPTOR_RELATIVE pSecurityDescriptor,
    ULONG ulLength,
    PULONG ulLengthUsed
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    SMB_PACKET packet = {0};
    uint32_t packetByteCount = 0;
    NT_TRANSACTION_REQUEST_HEADER *pHeader = NULL;
    PNT_TRANSACTION_SECONDARY_RESPONSE_HEADER pResponseHeader = NULL;
    SMB_RESPONSE *pResponse = NULL;
    PSMB_PACKET pResponsePacket = NULL;
    USHORT usMid = 0;
    USHORT usSetup[0];
    SMB_NT_TRANS_QUERY_SECURITY_DESC_REQUEST_HEADER queryHeader = {0};
    USHORT usQueryHeaderOffset = 0;
    USHORT usSetDataOffset = 0;
    ULONG ulTotalDataBytes = ulLength;
    ULONG ulReceivedDataBytes = 0;

    ntStatus = SMBPacketBufferAllocate(
        pTree->pSession->pSocket->hPacketAllocator,
        1024*64,
        &packet.pRawBuffer,
        &packet.bufferLen);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = SMBTreeAcquireMid(
        pTree,
        &usMid);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = SMBPacketMarshallHeader(
        packet.pRawBuffer,
        packet.bufferLen,
        COM_NT_TRANSACT,
        0,
        0,
        pTree->tid,
        0,
        pTree->pSession->uid,
        usMid,
        TRUE,
        &packet);
    BAIL_ON_NT_STATUS(ntStatus);

    packet.pData = packet.pParams + sizeof(NT_TRANSACTION_REQUEST_HEADER);
    packet.bufferUsed += sizeof(NT_TRANSACTION_REQUEST_HEADER);
    packet.pSMBHeader->wordCount = 19 + sizeof(usSetup)/sizeof(USHORT);

    pHeader = (NT_TRANSACTION_REQUEST_HEADER *) packet.pParams;

    queryHeader.usFid = usFid;
    queryHeader.securityInformation = securityInformation;

    ntStatus = WireMarshallTransactionRequestData(
        packet.pData,
        packet.bufferLen - packet.bufferUsed,
        &packetByteCount,
        usSetup,
        sizeof(usSetup)/sizeof(USHORT),
        NULL,
        (PBYTE) &queryHeader,
        sizeof(queryHeader),
        &usQueryHeaderOffset,
        NULL,
        0,
        &usSetDataOffset);
    BAIL_ON_NT_STATUS(ntStatus);

    assert(packetByteCount <= UINT16_MAX);
    packet.bufferUsed += packetByteCount;

    pHeader->usFunction = SMB_SUB_COMMAND_NT_TRANSACT_QUERY_SECURITY_DESC;
    pHeader->ulTotalParameterCount = sizeof(queryHeader);
    pHeader->ulTotalDataCount = 0;
    pHeader->ulMaxParameterCount = sizeof(queryHeader);
    pHeader->ulMaxDataCount = ulLength;
    pHeader->ucMaxSetupCount = sizeof(usSetup)/sizeof(USHORT);
    pHeader->ulParameterCount = sizeof(queryHeader);
    pHeader->ulParameterOffset = usQueryHeaderOffset + (packet.pData - (PBYTE) packet.pSMBHeader);
    pHeader->ulDataCount = 0;
    pHeader->ulDataOffset = 0;
    pHeader->ucSetupCount = sizeof(usSetup)/sizeof(USHORT);

    ntStatus = SMBPacketMarshallFooter(&packet);
    BAIL_ON_NT_STATUS(ntStatus);

    do
    {
        if (pResponsePacket)
        {
            SMBPacketFree(
                pTree->pSession->pSocket->hPacketAllocator,
                pResponsePacket);
        }

        if (pResponse)
        {
            SMBResponseFree(pResponse);
        }

        ntStatus = SMBResponseCreate(usMid, &pResponse);
        BAIL_ON_NT_STATUS(ntStatus);

        ntStatus = SMBSrvClientTreeAddResponse(pTree, pResponse);
        BAIL_ON_NT_STATUS(ntStatus);

        if (!pResponsePacket)
        {
            ntStatus = SMBSocketSend(pTree->pSession->pSocket, &packet);
            BAIL_ON_NT_STATUS(ntStatus);
        }

        ntStatus = SMBTreeReceiveResponse(
            pTree,
            packet.haveSignature,
            packet.sequence + 1,
            pResponse,
            &pResponsePacket);
        BAIL_ON_NT_STATUS(ntStatus);

        ntStatus = pResponsePacket->pSMBHeader->error;
        BAIL_ON_NT_STATUS(ntStatus);

        /* Skip intermediate response */
        if (pResponsePacket->pSMBHeader->wordCount == 0)
        {
            continue;
        }

        pResponseHeader = (NT_TRANSACTION_SECONDARY_RESPONSE_HEADER *) pResponsePacket->pParams;

        ulTotalDataBytes = pResponseHeader->ulTotalDataCount;

        if (ulTotalDataBytes > ulLength)
        {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
            BAIL_ON_NT_STATUS(ntStatus);
        }

        memcpy(
            ((PBYTE)pSecurityDescriptor) + ulReceivedDataBytes,
            ((PBYTE) pResponsePacket->pSMBHeader) + pResponseHeader->ulDataOffset,
            pResponseHeader->ulDataCount);

        ulReceivedDataBytes +=  pResponseHeader->ulDataCount;
    } while (ulReceivedDataBytes < ulTotalDataBytes);

    *ulLengthUsed = ulTotalDataBytes;

cleanup:

    if (pResponsePacket)
    {
        SMBPacketFree(
            pTree->pSession->pSocket->hPacketAllocator,
            pResponsePacket);
    }

    if (packet.bufferLen)
    {
        SMBPacketBufferFree(pTree->pSession->pSocket->hPacketAllocator,
                            packet.pRawBuffer,
                            packet.bufferLen);
    }

    if (pResponse)
    {
        SMBResponseFree(pResponse);
    }

    return ntStatus;

error:

    goto cleanup;
}
