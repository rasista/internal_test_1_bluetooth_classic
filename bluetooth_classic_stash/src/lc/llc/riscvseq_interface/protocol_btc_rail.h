/**
 * @file protocol_btc_rail.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-10-01
 *
 * @copyright Copyright (c) 2024-2025
 *
 */
#ifndef PROTOCOL_BTC_RAIL_H_
#define PROTOCOL_BTC_RAIL_H_

#include "stdbool.h"
#include "stdint.h"

#define setBtcFhKosAndN(seqselect, nval, kos) seqselect | nval | kos // TODO: Dummy definition for UT SIMPY tests
#define setBtcFhKnudgeAndIlos(knudge, ilos) knudge | ilos // Dummy definition for UT SIMPY tests

#define setBtcLmacDayCounter(dir, dayCounter) (((dir << 16) & 0x00000001) | ((dayCounter << 17) & 0x00000002)) // Dummy definition for UT SIMPY tests

/**
 * @brief Status codes for RAIL API function calls
 */
typedef enum RAIL_Status {
  RAIL_STATUS_NO_ERROR = 0,     /**< No error */
  RAIL_STATUS_INVALID_PARAMETER = 1,   /**< Invalid parameter passed in */
  RAIL_STATUS_INVALID_STATE = 2,     /**< API called in invalid state */
  RAIL_STATUS_INVALID_CALL = 3,     /**< Invalid call sequence */
  RAIL_STATUS_SUSPENDED = 4     /**< Operation suspended */
} RAIL_Status_t;

/**
 * @brief Idle modes for RAIL radio operation
 */
typedef enum RAIL_IdleMode {
  RAIL_IDLE = 0,    /**< Idle without aborting ongoing operations */
  RAIL_IDLE_ABORT = 1,    /**< Idle and abort ongoing operations */
  RAIL_IDLE_FORCE_SHUTDOWN = 2,    /**< Force shutdown regardless of state */
  RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS = 3    /**< Force shutdown and clear pending events */
} RAIL_IdleMode_t;

typedef struct RAIL_SEQ_BtcAccessAddress {
  uint32_t syncWord[2U];
} RAIL_SEQ_BtcAccessAddress_t;

typedef struct RAIL_SEQ_BtcInitConfig {
  RAIL_SEQ_BtcAccessAddress_t accessAddr;
  uint8_t bdAddr[6U];
} RAIL_SEQ_BtcInitConfig_t;

typedef struct RAIL_SEQ_BtcPacketConfig_d {
  uint8_t unused : 4;
  uint8_t packetType : 4;
} RAIL_SEQ_BtcPacketConfig_d_t;

typedef struct RAIL_SEQ_BtcRxPktTypeEscoPlLnth_d {
  uint16_t payloadLen : 11; // RXESCOPLLNTHFRMREG field, needed for EV packets and ACL packets
  uint16_t enhancedRate : 1;
  uint16_t sco : 1;
  uint16_t esco : 1;
  uint16_t acl : 1;
  uint16_t unused : 1;
} RAIL_SEQ_BtcRxPktTypeEscoPlLnth_d_t;

typedef enum sl_rail_btc_special_pkt {
  SL_RAIL_BTC_NORMAL_PKT = 0,
  SL_RAIL_BTC_ID_PKT = 1,
  SL_RAIL_BTC_DV_PKT = 2,
} sl_rail_btc_special_pkt_t;

typedef struct RAIL_SEQ_BtcTrxConfig {
  bool isRx;
  sl_rail_btc_special_pkt_t specialPktType;  // replaces isIdPkt and isDvPkt
  uint8_t txPower;       // Transmit Power variable for rail both connected and non_connected
  RAIL_SEQ_BtcPacketConfig_d_t packetConfig;
  uint32_t clk;          // BD Clock value at which TXRX is expected
  uint32_t tsf;          // protimer TSF value for TXRX activity
  uint16_t txPacketLen;  // needed for EV packets and ACL packets
  RAIL_SEQ_BtcRxPktTypeEscoPlLnth_d_t linkConfig;
  uint32_t rxTimeout;      // timeout used in tx/rx
  uint8_t encryptLink;   // 0: no encryption, 1: E0 encryption, 2: AES encryption
} RAIL_SEQ_BtcTrxConfig_d_t;

typedef enum BtcSeqSelect {
  PGSN,  // 0x0 page scan
  IPGSN, // 0x1 generalized interlaced page scan
  IQSN,  // 0x2 inquiry scan
  IIQSN, // 0x3 generalized interlaced inquiry scan
  PAGE,  // 0x4 page
  PGRSM, // 0x5 central page response
  PGRSS, // 0x6 peripheral page response
  IQRY,  // 0x7 inquiry
  IQRS,  // 0x8 inquiry response
  BASIC, // 0x9 connection (basic)
  ADAPT, // 0xA connection (adaptive)
} BtcSeqSelect_t;

#if 0
typedef struct RAIL_SEQ_BtcFhKosAndN {
  uint16_t kOffset : 1;    // k_offset used for deciding the train value (24 or 8)
  uint16_t NVal : 7;       // Phase offset used in different states
  uint16_t unused : 3;
  uint16_t seqSelect : 4;
} RAIL_SEQ_BtcFhKosAndN_t;
typedef union RAIL_SEQ_BtcFhKosAndN {
  uint32_t val;
  struct {
    uint16_t kOffset : 1;    // k_offset used for deciding the train value (24 or 8)
    uint16_t NVal : 8;       // Phase offset used in different states
    uint16_t unused : 3;
    uint16_t seqSelect : 4;
  } regFields;
} RAIL_SEQ_BtcFhKosAndN_t;

typedef union RAIL_SEQ_BtcFhKnudgeAndIlos {
  uint32_t val;
  struct {
    uint16_t kNudge : 5;     // k_nudge: needed in page hopping
    uint16_t IlOs : 5;       // interlace offset: used in Generalized Interlaced Page/Inquiry Scan (0x10 default)
    uint16_t unused : 6;
  } regFields;
} RAIL_SEQ_BtcFhKnudgeAndIlos_t;
#endif

typedef struct RAIL_SEQ_BtcFhConfig {
  /*  These parameters are loaded to frequency hopping engine to
   *  calculate the channel index
   */
  /* The ulap represents the upper mac address, but in the case of
   * inquiry it is generic fixed value
   */
  uint32_t ulap;
  uint16_t kosAndN;
  uint16_t knudgeAndIlos;
  uint32_t chlMap0Init;
  uint32_t chlMap1Init;
  uint32_t chlMap2Init;
  uint32_t masterClk;    // master clock: slot number
  /* The clock when the id packet is received in the page and page scan states */
  uint32_t frozenClk;    // frozen clock: used in page peripheral/central response
  /*  Read from hardware and update the channel num */
  uint8_t channelNum;
} RAIL_SEQ_BtcFhConfig_t;

typedef struct RAIL_SEQ_BtcFhResult {
  /* This structure is used to return the result of the frequency hopping.
   * 2 outputs are gathered from the Frequency Hopping copro engine :
   *  1. Channel number
   *  2. XVAL : the scrambler seed for the non-connected procedures
   */
  uint8_t channelNum;
  uint8_t xVal;
} RAIL_SEQ_BtcFhResult_t;

/**
 * @enum RAIL_SeqPacketStatus_t
 * @brief The packet status code associated with a packet transmitted or
 *   currently being transmitted.
 */
typedef enum RAIL_SeqTxPacketStatus_t {
  /** Indicate that the packet was successfully transmitted. */
  RAIL_SEQ_TX_PACKET_STATUS_SENT = 0,
  /** Indicate that the transmit was aborted by the user. */
  RAIL_SEQ_TX_PACKET_STATUS_ABORT_ABORTED = 1,
  /**
   * Indicate transmit is blocked from occurring because
   * RAIL_EnableTxHoldOff() was called.
   */
  RAIL_SEQ_TX_PACKET_STATUS_ABORT_BLOCKED = 2,
  /** Indicate transmit buffer underflow. */
  RAIL_SEQ_TX_PACKET_STATUS_ABORT_UNDERFLOW = 4,
}RAIL_SeqTxPacketStatus_t;

/**
 * @struct RAIL_PacketTimeStamp_t
 * @brief Information for calculating and representing a packet timestamp.
 */
typedef struct RAIL_PacketTimeStamp {
  /**
   * Timestamp of the packet in the RAIL timebase.
   */
  uint32_t packetTime;
  /**
   * A value specifying the total length in bytes of the packet
   * used when calculating the packetTime requested by the timePosition
   * field. This should account for all bytes sent over the air after
   * the Preamble and Sync word(s) including CRC bytes.
   */
  uint16_t totalPacketBytes;
  /**
   * A \ref RAIL_PacketTimePosition_t value specifying the packet position
   * to return in the packetTime field.
   * If this is \ref RAIL_PACKET_TIME_DEFAULT, this field will be
   * updated with the actual position corresponding to the packetTime
   * value filled in by a call using this structure.
   */
  uint8_t timePosition;
  /**
   * In RX for EFR32xG25 only:
   * A value specifying the on-air duration of the data packet,
   * starting with the first bit of the PHR (i.e., end of sync word);
   * preamble and sync word duration are hence excluded.
   *
   * In Tx for all platforms:
   * A value specifying the on-air duration of the data packet,
   * starting at the preamble (i.e. includes preamble, sync word, PHR,
   * payload and FCS). This value can be used to compute duty cycles.
   * @note This field is currently valid only for normal transmits but
   *   not Auto-Ack transmits which set the field to zero.
   */
  uint32_t packetDurationUs;
} RAIL_PacketTimeStamp_t;

/**
 * @struct RAIL_TxPacketDetails_t
 * @brief Detailed information requested about the packet that was just,
 *   or is currently being, transmitted.
 */
typedef struct RAIL_TxPacketDetails {
  /**
   * The timestamp of the transmitted packet in the RAIL timebase,
   * filled in by \ref RAIL_GetTxPacketDetails().
   */
  RAIL_PacketTimeStamp_t timeSent;
  /**
   * Indicate whether the transmitted packet was an automatic Ack. In a generic
   * sense, an automatic Ack is defined as a packet sent in response to a
   * received Ack-requesting frame when Auto-Ack is enabled. In a protocol
   * specific sense this definition may be more or less restrictive to match the
   * specification and you should refer to that protocol's documentation.
   */
  bool isAck;
} RAIL_TxPacketDetails_t;

/**
 * @struct RAIL_SeqTxPacketInfo_t
 * @brief Basic information about a packet that was just, or is currently being,
 *   transmitted.
 *
 */
typedef struct RAIL_SeqTxPacketInfo {
  /** The packet status of this packet. */
  RAIL_SeqTxPacketStatus_t pktStatus;
  /**
   * Pointer to the detailed information about the packet that was just,
   * or is currently being, transmitted.
   */
  RAIL_TxPacketDetails_t *pTxPktDetails;
} RAIL_SeqTxPacketInfo_t;

/**
 * @enum RAIL_RxPacketStatus_t
 * @brief The packet status code associated with a packet received or
 *   currently being received.
 *
 * @note RECEIVING implies some packet data may be available, but
 *   is untrustworthy (not CRC-verified) and might disappear if the packet
 *   is rolled back on error. No packet details are yet available.
 * @note In RX \ref RAIL_DataMethod_t::FIFO_MODE, ABORT statuses imply some
 *   packet data may be available, but it's incomplete and not trustworthy.
 */
typedef enum RAIL_RxPacketStatus {
  /**
   * The radio is idle or searching for a packet.
   */
  RAIL_RX_PACKET_NONE = 0,
  /**
   * The packet was aborted during filtering because of illegal frame length,
   * CRC or block decoding errors, other RAIL built-in protocol-specific
   * packet content errors, or by the application or multiprotocol scheduler
   * idling the radio with \ref RAIL_IDLE_ABORT or higher.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_ABORTED is triggered.
   */
  RAIL_RX_PACKET_ABORT_FORMAT = 1,
  /**
   * The packet failed address filtering.
   *
   * Corresponding \ref RAIL_EVENT_RX_ADDRESS_FILTERED is triggered.
   */
  RAIL_RX_PACKET_ABORT_FILTERED = 2,
  /**
   * The packet passed any filtering but was aborted by the application
   * or multiprotocol scheduler idling the radio with \ref RAIL_IDLE_ABORT
   * or higher.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_ABORTED is triggered.
   */
  RAIL_RX_PACKET_ABORT_ABORTED = 3,
  /**
   * The packet overflowed the receive buffer.
   *
   * Corresponding \ref RAIL_EVENT_RX_FIFO_OVERFLOW is triggered.
   */
  RAIL_RX_PACKET_ABORT_OVERFLOW = 4,
  /**
   * The packet passed any filtering but subsequently failed CRC check(s)
   * block decoding, or illegal frame length, and was aborted.
   *
   * Corresponding \ref RAIL_EVENT_RX_FRAME_ERROR is triggered.
   */
  RAIL_RX_PACKET_ABORT_CRC_ERROR = 5,
  /**
   * The packet passed any filtering but subsequently failed CRC check(s)
   * with \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS in effect. Can also occur
   * when the packet prematurely ended successfully during filtering,
   * and either the \ref RAIL_EVENT_RX_PACKET_ABORTED or
   * \ref RAIL_EVENT_RX_ADDRESS_FILTERED events had been enabled
   * requesting notification of such packets.
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_RECEIVED is triggered.
   */
  RAIL_RX_PACKET_READY_CRC_ERROR = 6,
  /**
   * The packet was successfully received, passing CRC check(s).
   *
   * Corresponding \ref RAIL_EVENT_RX_PACKET_RECEIVED is triggered.
   */
  RAIL_RX_PACKET_READY_SUCCESS = 7,
  /**
   * A packet is being received and is not yet complete.
   */
  RAIL_RX_PACKET_RECEIVING = 8,
}RAIL_RxPacketStatus_t;

/**
 * @struct RAIL_RxPacketInfo_t
 * @brief Basic information about a packet being received or already
 *   completed and awaiting processing, including memory pointers to
 *   its data in the circular receive FIFO buffer. This packet information
 *   refers to remaining packet data that has not already been consumed
 *   by \ref RAIL_ReadRxFifo().
 *
 * @note Because the receive FIFO buffer is circular, a packet might start
 *   near the end of the buffer and wrap around to the beginning of
 *   the buffer to finish, hence the distinction between the first
 *   and last portions. Packets that fit without wrapping only have
 *   a first portion (firstPortionBytes == packetBytes and lastPortionData
 *   will be NULL).
 */
typedef struct RAIL_RxPacketInfo {
  /** The packet status of this packet. */
  RAIL_RxPacketStatus_t packetStatus;
  /** The number of packet data bytes available to read in this packet. */
  uint16_t packetBytes;
  /** The number of bytes in the first portion. */
  uint16_t firstPortionBytes;
  /**
   * The pointer to the first portion of packet data containing
   * firstPortionBytes number of bytes.
   */
  uint8_t *firstPortionData;
  /**
   * The pointer to the last portion of a packet, if any; NULL otherwise.
   * The number of bytes in this portion is
   * packetBytes - firstPortionBytes.
   */
  uint8_t *lastPortionData;
  /**
   * A bitmask representing which address filter(s) this packet has passed.
   * Will be 0 when not filtering or if packet info is retrieved before
   * filtering has completed. It's undefined on platforms lacking \ref
   * RAIL_SUPPORTS_ADDR_FILTER_MASK.
   */
  uint8_t filterMask;
} RAIL_RxPacketInfo_t;

/**
 * @struct RAIL_RxPacketDetails_t
 * @brief Received packet details obtained via \ref RAIL_GetRxPacketDetails()
 *   or RAIL_GetRxPacketDetailsAlt().
 *
 * @note Certain details are always available, while others are only available
 *   if the \ref RAIL_RxOptions_t \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO
 *   option is not in effect and the received packet's
 *   \ref RAIL_RxPacketStatus_t is among the RAIL_RX_PACKET_READY_ set.
 *   Each detail's description indicates its availability.
 *
 */
typedef struct RAIL_RxPacketDetails {
  /**
   * The timestamp of the received packet in the RAIL timebase.
   *
   * When not available it will be \ref RAIL_PACKET_TIME_INVALID.
   */
  RAIL_PacketTimeStamp_t timeReceived;
  /**
   * Indicates whether the CRC passed or failed for the received packet.
   * It is true for \ref RAIL_RX_PACKET_READY_SUCCESS packets and false
   * for all others.
   *
   * It is always available.
   */
  bool crcPassed;
  /**
   * Indicate whether the received packet was the expected Ack.
   * It is true for the expected Ack and false otherwise.
   *
   * It is always available.
   *
   * An expected Ack is defined as a protocol-correct Ack packet
   * successfully-received (\ref RAIL_RX_PACKET_READY_SUCCESS or
   * \ref RAIL_RX_PACKET_READY_CRC_ERROR) and whose sync word was
   * detected within the
   * RAIL_AutoAckConfig_t::ackTimeout period following a transmit
   * which specified \ref RAIL_TX_OPTION_WAIT_FOR_ACK, requested
   * an Ack, and Auto-Ack is enabled. When true, the ackTimeout
   * period was terminated so no \ref RAIL_EVENT_RX_ACK_TIMEOUT
   * will be subsequently posted for the transmit.
   *
   * A "protocol-correct Ack" applies to the 802.15.4 or Z-Wave
   * protocols for which RAIL can discern the frame type and match
   * the Ack's sequence number with that of the transmitted frame.
   * For other protocols, the first packet successfully-received
   * whose sync word was detected within the \ref
   * RAIL_AutoAckConfig_t::ackTimeout period is
   * considered the expected Ack; upper layers are responsible for
   * confirming this.
   */
  bool isAck;
  /**
   * RSSI of the received packet in integer dBm. This RSSI measurement is
   * started as soon as the sync word is detected. The duration of the
   * measurement is PHY-specific.
   *
   * When not available it will be \ref RAIL_RSSI_INVALID_DBM.
   */
  int8_t rssi;
  /**
   * The link quality indicator of the received packet. A zero would
   * indicate a very low quality packet while a 255 would indicate a very
   * high quality packet.
   *
   * When not available it will be 0.
   */
  uint8_t lqi;
  /**
   * For radios and PHY configurations that support multiple sync words, this
   * number is the ID of the sync word that was used for this packet.
   *
   * It is always available.
   */
  uint8_t syncWordId;
  /**
   * In configurations where the radio has the option of receiving a given
   * packet in multiple ways, indicates which of the sub-PHY options
   * was used to receive the packet. Most radio configurations do not have
   * this ability and the subPhyId is set to 0.
   *
   * Currently, this field is used by the BLE Coded PHY, the BLE Simulscan PHY
   * and the SUN OFDM PHYs.
   * In BLE cases, a value of 0 marks a 500 kbps packet, a value of 1 marks a 125
   * kbps packet, and a value of 2 marks a 1 Mbps packet.
   * Also, see \ref RAIL_BLE_ConfigPhyCoded() and \ref RAIL_BLE_ConfigPhySimulscan().
   *
   * In SUN OFDM cases, the value corresponds to the numerical value of the
   * Modulation and Coding Scheme (MCS) level of the last received packet.
   * The packet bitrate depends on the MCS value, as well as the OFDM option.
   * Packets bitrates for SUN OFDM PHYs can be found in 802.15.4-2020 specification,
   * chapter 20.3, table 20-10.
   * Ex: Packet bitrate for OFDM option 1 MCS0 is 100kb/s and 2400kb/s for MCS6.
   *
   * In WMBUS cases, when using PHY_wMbus_ModeTC_M2O_100k_frameA with simultaneous
   * RX of T and C modes enabled (\ref RAIL_WMBUS_Config()), the value corresponds
   * to \ref RAIL_WMBUS_Phy_t.
   *
   * It is always available.
   */
  uint8_t subPhyId;
  /**
   * For \ref Antenna_Control configurations where the device has multiple
   * antennas, this indicates which antenna received the packet. When there
   * is only one antenna, this will be set to the default of 0.
   *
   * It is always available.
   */
  uint8_t antennaId;
  /**
   * When channel hopping is enabled, this field will contain the index
   * of the channel in \ref RAIL_RxChannelHoppingConfig_t::entries on which
   * this packet was received, or a sentinel value.
   *
   * It is always available.
   */
  uint8_t channelHoppingChannelIndex;
  /**
   * The channel on which the packet was received.
   *
   * It is always available.
   *
   * @note It is best to fully process (empty or clear) the receive FIFO
   *   before changing channel configurations (\ref RAIL_ConfigChannels()
   *   or a built-in configuration) as unprocessed packets' channel
   *   could reflect the wrong configuration.
   */
  uint16_t channel;
} RAIL_RxPacketDetails_t;

typedef struct RAIL_SEQ_E0EncryptionConfig {
  uint32_t masterClock;
  uint32_t encKey0;
  uint32_t encKey1;
  uint32_t encKey2;
  uint32_t encKey3;
  uint32_t bdAddr0;
  uint16_t bdAddr1;
  uint8_t keyLength;
} RAIL_SEQ_E0EncryptionConfig_t;

typedef struct RAIL_SEQ_AESEncryptionConfig {
  uint32_t aesKeyBytes_0_3;
  uint32_t aesKeyBytes_4_7;
  uint32_t aesKeyBytes_8_11;
  uint32_t aesKeyBytes_12_15;
  uint32_t aesIv1;
  uint32_t aesIv2;
  uint16_t dayCountAndDir;
  uint16_t zeroLenAclW; //1bit: struct?
  uint32_t aesPldCntr1;
  uint8_t aesPldCntr2;
} RAIL_SEQ_AESEncryptionConfig_t;

#endif
