/*******************************************
PeerConnection internal include file
*******************************************/
#ifndef __KINESIS_VIDEO_WEBRTC_CLIENT_PEERCONNECTION_PEERCONNECTION__
#define __KINESIS_VIDEO_WEBRTC_CLIENT_PEERCONNECTION_PEERCONNECTION__

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LOCAL_ICE_UFRAG_LEN 4
#define LOCAL_ICE_PWD_LEN   24
#define LOCAL_CNAME_LEN     16

// https://tools.ietf.org/html/rfc5245#section-15.4
#define MAX_ICE_UFRAG_LEN 256
#define MAX_ICE_PWD_LEN   256

#define PEER_FRAME_BUFFER_SIZE_INCREMENT_FACTOR 1.5

// A non-comprehensive list of valid JSON characters
#define VALID_CHAR_SET_FOR_JSON "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/"

#define ICE_CANDIDATE_JSON_TEMPLATE (PCHAR) "{\"candidate\":\"candidate:%s\",\"sdpMid\":\"0\",\"sdpMLineIndex\":0}"

#define MAX_ICE_CANDIDATE_JSON_LEN (MAX_SDP_ATTRIBUTE_VALUE_LENGTH + SIZEOF(ICE_CANDIDATE_JSON_TEMPLATE) + 1)

#define CODEC_HASH_TABLE_BUCKET_COUNT  50
#define CODEC_HASH_TABLE_BUCKET_LENGTH 2
#define RTX_HASH_TABLE_BUCKET_COUNT    50
#define RTX_HASH_TABLE_BUCKET_LENGTH   2

#define DATA_CHANNEL_HASH_TABLE_BUCKET_COUNT  200
#define DATA_CHANNEL_HASH_TABLE_BUCKET_LENGTH 2

// Environment variable to display SDPs
#define DEBUG_LOG_SDP ((PCHAR) "DEBUG_LOG_SDP")

typedef enum {
    RTC_RTX_CODEC_H264_PROFILE_42E01F_LEVEL_ASYMMETRY_ALLOWED_PACKETIZATION_MODE = 1,
    RTC_RTX_CODEC_VP8 = 2,
} RTX_CODEC;

typedef struct {
    RtcPeerConnection peerConnection;
    PIceAgent pIceAgent;
    PDtlsSession pDtlsSession;
    BOOL dtlsIsServer;

    MUTEX pSrtpSessionLock;
    PSrtpSession pSrtpSession;

#if ENABLE_DATA_CHANNEL
    PSctpSession pSctpSession;
#endif

    SessionDescription remoteSessionDescription;
    PDoubleList pTransceivers;
    BOOL sctpIsEnabled;

    CHAR localIceUfrag[LOCAL_ICE_UFRAG_LEN + 1];
    CHAR localIcePwd[LOCAL_ICE_PWD_LEN + 1];

    CHAR remoteIceUfrag[MAX_ICE_UFRAG_LEN + 1];
    CHAR remoteIcePwd[MAX_ICE_PWD_LEN + 1];

    CHAR localCNAME[LOCAL_CNAME_LEN + 1];

    CHAR remoteCertificateFingerprint[CERTIFICATE_FINGERPRINT_LENGTH + 1];

    MUTEX peerConnectionObjLock;

    BOOL isOffer;

    TIMER_QUEUE_HANDLE timerQueueHandle;

    // Codecs that we support and their payloadTypes
    // When offering we generate values starting from 96
    // When answering this is populated from the remote offer
    PHashTable pCodecTable;

    // Payload types that we use to retransmit data
    // When answering this is populated from the remote offer
    PHashTable pRtxTable;

    // DataChannels keyed by streamId
    PHashTable pDataChannels;

#if ENABLE_DATA_CHANNEL
    UINT64 onDataChannelCustomData;
    RtcOnDataChannel onDataChannel;
#endif

    UINT64 onIceCandidateCustomData;
    RtcOnIceCandidate onIceCandidate;

    UINT64 onConnectionStateChangeCustomData;
    RtcOnConnectionStateChange onConnectionStateChange;
    RTC_PEER_CONNECTION_STATE connectionState;

    UINT16 MTU;

    NullableBool canTrickleIce;
} KvsPeerConnection, *PKvsPeerConnection;

#if ENABLE_DATA_CHANNEL
typedef struct {
    UINT32 currentDataChannelId;
    PKvsPeerConnection pKvsPeerConnection;
    PHashTable unkeyedDataChannels;
} AllocateSctpSortDataChannelsData, *PAllocateSctpSortDataChannelsData;
#endif

STATUS onFrameReadyFunc(UINT64, UINT16, UINT16, UINT32);
STATUS onFrameDroppedFunc(UINT64, UINT16, UINT16, UINT32);
VOID onSctpSessionOutboundPacket(UINT64, PBYTE, UINT32);
VOID onSctpSessionDataChannelMessage(UINT64, UINT32, BOOL, PBYTE, UINT32);
VOID onSctpSessionDataChannelOpen(UINT64, UINT32, PBYTE, UINT32);

STATUS sendPacketToRtpReceiver(PKvsPeerConnection, PBYTE, UINT32);
STATUS changePeerConnectionState(PKvsPeerConnection, RTC_PEER_CONNECTION_STATE);

#ifdef __cplusplus
}
#endif
#endif /* __KINESIS_VIDEO_WEBRTC_CLIENT_PEERCONNECTION_PEERCONNECTION__ */
