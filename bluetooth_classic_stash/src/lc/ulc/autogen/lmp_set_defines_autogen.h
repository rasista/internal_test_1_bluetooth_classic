#ifndef _LMP_SET_DEF_H_
#define _LMP_SET_DEF_H_

/* LMP_NAME_REQ */
#define SET_LMP_NAME_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_NAME_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x1)
#define SET_LMP_NAME_REQ_PARAMETER_NAME_OFFSET(buf, value)  buf[4] = value
/* LMP_NAME_RES */
#define SET_LMP_NAME_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_NAME_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2)
#define SET_LMP_NAME_RES_PARAMETER_NAME_OFFSET(buf, value)  buf[4] = value
#define SET_LMP_NAME_RES_PARAMETER_NAME_LENGTH(buf, value)  buf[5] = value
#define SET_LMP_NAME_RES_PARAMETER_NAME_FRAGMENT(buf, value, len)  memcpy((uint8_t *)&buf[6], &value, MIN(14,len))
/* LMP_ACCEPTED */
#define SET_LMP_ACCEPTED_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ACCEPTED_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3)
#define SET_LMP_ACCEPTED_PARAMETER_OPCODE(buf, value)  buf[4] = value
/* LMP_NOT_ACCEPTED */
#define SET_LMP_NOT_ACCEPTED_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_NOT_ACCEPTED_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x4)
#define SET_LMP_NOT_ACCEPTED_PARAMETER_OPCODE(buf, value)  buf[4] = value
#define SET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(buf, value)  buf[5] = value
/* LMP_CLKOFFSET_REQ */
#define SET_LMP_CLKOFFSET_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CLKOFFSET_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x5)
/* LMP_CLKOFFSET_RES */
#define SET_LMP_CLKOFFSET_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CLKOFFSET_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x6)
#define SET_LMP_CLKOFFSET_RES_PARAMETER_CLOCK_OFFSET(buf, value)  *((uint16_t *)&buf[4]) = value
/* LMP_DETACH */
#define SET_LMP_DETACH_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_DETACH_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x7)
#define SET_LMP_DETACH_PARAMETER_ERROR_CODE(buf, value)  buf[4] = value
/* LMP_IN_RAND */
#define SET_LMP_IN_RAND_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_IN_RAND_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x8)
#define SET_LMP_IN_RAND_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_COMB_KEY */
#define SET_LMP_COMB_KEY_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_COMB_KEY_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x9)
#define SET_LMP_COMB_KEY_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_UNIT_KEY */
#define SET_LMP_UNIT_KEY_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_UNIT_KEY_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xa)
#define SET_LMP_UNIT_KEY_PARAMETER_KEY(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_AU_RAND */
#define SET_LMP_AU_RAND_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_AU_RAND_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xb)
#define SET_LMP_AU_RAND_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_SRES */
#define SET_LMP_SRES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SRES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xc)
#define SET_LMP_SRES_PARAMETER_AUTHENTICATION_RSP(buf, value)  *((uint32_t *)&buf[4]) = value
/* LMP_TEMP_RAND */
#define SET_LMP_TEMP_RAND_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TEMP_RAND_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xd)
#define SET_LMP_TEMP_RAND_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_TEMP_KEY */
#define SET_LMP_TEMP_KEY_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TEMP_KEY_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xe)
#define SET_LMP_TEMP_KEY_PARAMETER_KEY(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_ENCRYPTION_MODE_REQ */
#define SET_LMP_ENCRYPTION_MODE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCRYPTION_MODE_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0xf)
#define SET_LMP_ENCRYPTION_MODE_REQ_PARAMETER_ENCRYPTION_MODE(buf, value)  buf[4] = value
/* LMP_ENCRYPTION_KEY_SIZE_REQ */
#define SET_LMP_ENCRYPTION_KEY_SIZE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCRYPTION_KEY_SIZE_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x10)
#define SET_LMP_ENCRYPTION_KEY_SIZE_REQ_PARAMETER_KEY_SIZE(buf, value)  buf[4] = value
/* LMP_START_ENCRYPTION_REQ */
#define SET_LMP_START_ENCRYPTION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_START_ENCRYPTION_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x11)
#define SET_LMP_START_ENCRYPTION_REQ_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_STOP_ENCRYPTION_REQ */
#define SET_LMP_STOP_ENCRYPTION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_STOP_ENCRYPTION_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x12)
/* LMP_SWITCH_REQ */
#define SET_LMP_SWITCH_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SWITCH_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x13)
#define SET_LMP_SWITCH_REQ_PARAMETER_SWITCH_INSTANT(buf, value)  *((uint32_t *)&buf[4]) = value
/* LMP_HOLD */
#define SET_LMP_HOLD_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_HOLD_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x14)
#define SET_LMP_HOLD_PARAMETER_HOLD_TIME(buf, value)  *((uint16_t *)&buf[4]) = value
#define SET_LMP_HOLD_PARAMETER_HOLD_INSTANT(buf, value)  *((uint32_t *)&buf[6]) = value
/* LMP_HOLD_REQ */
#define SET_LMP_HOLD_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_HOLD_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x15)
#define SET_LMP_HOLD_REQ_PARAMETER_HOLD_TIME(buf, value)  *((uint16_t *)&buf[4]) = value
#define SET_LMP_HOLD_REQ_PARAMETER_HOLD_INSTANT(buf, value)  *((uint32_t *)&buf[6]) = value
/* LMP_SNIFF_REQ */
#define SET_LMP_SNIFF_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SNIFF_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x17)
#define SET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_1(buf, value)  buf[4] = value
#define SET_LMP_SNIFF_REQ_PARAMETER_D_SNIFF(buf, value)  *((uint16_t *)&buf[5]) = value
#define SET_LMP_SNIFF_REQ_PARAMETER_T_SNIFF(buf, value)  *((uint16_t *)&buf[7]) = value
#define SET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_2(buf, value)  *((uint16_t *)&buf[9]) = value
#define SET_LMP_SNIFF_REQ_PARAMETER_SNIFF_TIMEOUT(buf, value)  *((uint16_t *)&buf[11]) = value
/* LMP_UNSNIFF_REQ */
#define SET_LMP_UNSNIFF_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_UNSNIFF_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x18)
/* LMP_INCR_POWER_REQ */
#define SET_LMP_INCR_POWER_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_INCR_POWER_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x1f)
#define SET_LMP_INCR_POWER_REQ_PARAMETER_RESERVED(buf, value)  buf[4] = value
/* LMP_DECR_POWER_REQ */
#define SET_LMP_DECR_POWER_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_DECR_POWER_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x20)
#define SET_LMP_DECR_POWER_REQ_PARAMETER_RESERVED(buf, value)  buf[4] = value
/* LMP_MAX_POWER */
#define SET_LMP_MAX_POWER_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_MAX_POWER_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x21)
/* LMP_MIN_POWER */
#define SET_LMP_MIN_POWER_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_MIN_POWER_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x22)
/* LMP_AUTO_RATE */
#define SET_LMP_AUTO_RATE_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_AUTO_RATE_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x23)
/* LMP_PREFERRED_RATE */
#define SET_LMP_PREFERRED_RATE_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PREFERRED_RATE_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x24)
#define SET_LMP_PREFERRED_RATE_PARAMETER_DATA_RATE(buf, value)  buf[4] = value
/* LMP_VERSION_REQ */
#define SET_LMP_VERSION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_VERSION_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x25)
#define SET_LMP_VERSION_REQ_PARAMETER_VERSION(buf, value)  buf[4] = value
#define SET_LMP_VERSION_REQ_PARAMETER_COMPANY_IDENTIFIER(buf, value)  *((uint16_t *)&buf[5]) = value
#define SET_LMP_VERSION_REQ_PARAMETER_SUBVERSION(buf, value)  *((uint16_t *)&buf[7]) = value
/* LMP_VERSION_RES */
#define SET_LMP_VERSION_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_VERSION_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x26)
#define SET_LMP_VERSION_RES_PARAMETER_VERSION(buf, value)  buf[4] = value
#define SET_LMP_VERSION_RES_PARAMETER_COMPANY_IDENTIFIER(buf, value)  *((uint16_t *)&buf[5]) = value
#define SET_LMP_VERSION_RES_PARAMETER_SUBVERSION(buf, value)  *((uint16_t *)&buf[7]) = value
/* LMP_FEATURES_REQ */
#define SET_LMP_FEATURES_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_FEATURES_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x27)
#define SET_LMP_FEATURES_REQ_PARAMETER_FEATURES(buf, value)  memcpy((uint8_t *)&buf[4], value, 8)
/* LMP_FEATURES_RES */
#define SET_LMP_FEATURES_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_FEATURES_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x28)
#define SET_LMP_FEATURES_RES_PARAMETER_FEATURES(buf, value)  memcpy((uint8_t *)&buf[4], value, 8)
/* LMP_QUALITY_OF_SERVICE */
#define SET_LMP_QUALITY_OF_SERVICE_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_QUALITY_OF_SERVICE_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x29)
#define SET_LMP_QUALITY_OF_SERVICE_PARAMETER_POLL_INTERVAL(buf, value)  *((uint16_t *)&buf[4]) = value
#define SET_LMP_QUALITY_OF_SERVICE_PARAMETER_N_BC(buf, value)  buf[6] = value
/* LMP_QUALITY_OF_SERVICE_REQ */
#define SET_LMP_QUALITY_OF_SERVICE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_QUALITY_OF_SERVICE_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2a)
#define SET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_POLL_INTERVAL(buf, value)  *((uint16_t *)&buf[4]) = value
#define SET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_N_BC(buf, value)  buf[6] = value
/* LMP_SCO_LINK_REQ */
#define SET_LMP_SCO_LINK_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SCO_LINK_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2b)
#define SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(buf, value)  buf[4] = value
#define SET_LMP_SCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(buf, value)  buf[5] = value
#define SET_LMP_SCO_LINK_REQ_PARAMETER_D_SCO(buf, value)  buf[6] = value
#define SET_LMP_SCO_LINK_REQ_PARAMETER_T_SCO(buf, value)  buf[7] = value
#define SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(buf, value)  buf[8] = value
#define SET_LMP_SCO_LINK_REQ_PARAMETER_AIR_MODE(buf, value)  buf[9] = value
/* LMP_REMOVE_SCO_LINK_REQ */
#define SET_LMP_REMOVE_SCO_LINK_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_REMOVE_SCO_LINK_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2c)
#define SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(buf, value)  buf[4] = value
#define SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_ERROR_CODE(buf, value)  buf[5] = value
/* LMP_MAX_SLOT */
#define SET_LMP_MAX_SLOT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_MAX_SLOT_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2d)
#define SET_LMP_MAX_SLOT_PARAMETER_MAX_SLOTS(buf, value)  buf[4] = value
/* LMP_MAX_SLOT_REQ */
#define SET_LMP_MAX_SLOT_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_MAX_SLOT_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2e)
#define SET_LMP_MAX_SLOT_REQ_PARAMETER_MAX_SLOTS(buf, value)  buf[4] = value
/* LMP_TIMIMG_ACCURACY_REQ */
#define SET_LMP_TIMIMG_ACCURACY_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TIMIMG_ACCURACY_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x2f)
/* LMP_TIMING_ACCURACY_RES */
#define SET_LMP_TIMING_ACCURACY_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TIMING_ACCURACY_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x30)
#define SET_LMP_TIMING_ACCURACY_RES_PARAMETER_DRIFT(buf, value)  buf[4] = value
#define SET_LMP_TIMING_ACCURACY_RES_PARAMETER_JITTER(buf, value)  buf[5] = value
/* LMP_SETUP_COMPLETE */
#define SET_LMP_SETUP_COMPLETE_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SETUP_COMPLETE_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x31)
/* LMP_USE_SEMI_PERMANENT_KEY */
#define SET_LMP_USE_SEMI_PERMANENT_KEY_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_USE_SEMI_PERMANENT_KEY_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x32)
/* LMP_HOST_CONNECTION_REQ */
#define SET_LMP_HOST_CONNECTION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_HOST_CONNECTION_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x33)
/* LMP_SLOT_OFFSET */
#define SET_LMP_SLOT_OFFSET_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SLOT_OFFSET_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x34)
#define SET_LMP_SLOT_OFFSET_PARAMETER_SLOT_OFFSET(buf, value)  *((uint16_t *)&buf[4]) = value
#define SET_LMP_SLOT_OFFSET_PARAMETER_BD_ADDR(buf, value)  memcpy((uint8_t *)&buf[6], value, 6)
/* LMP_PAGE_MODE_REQ */
#define SET_LMP_PAGE_MODE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PAGE_MODE_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x35)
#define SET_LMP_PAGE_MODE_REQ_PARAMETER_PAGE_SCHEME(buf, value)  buf[4] = value
#define SET_LMP_PAGE_MODE_REQ_PARAMETER_PAGE_SCHEME_SETTING(buf, value)  buf[5] = value
/* LMP_PAGE_SCAN_MODE_REQ */
#define SET_LMP_PAGE_SCAN_MODE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PAGE_SCAN_MODE_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x36)
#define SET_LMP_PAGE_SCAN_MODE_REQ_PARAMETER_PAGE_SCHEME(buf, value)  buf[4] = value
#define SET_LMP_PAGE_SCAN_MODE_REQ_PARAMETER_PAGE_SCHEME_SETTING(buf, value)  buf[5] = value
/* LMP_SUPERVISION_TIMEOUT */
#define SET_LMP_SUPERVISION_TIMEOUT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SUPERVISION_TIMEOUT_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x37)
#define SET_LMP_SUPERVISION_TIMEOUT_PARAMETER_SUPERVISION_TIMEOUT(buf, value)  *((uint16_t *)&buf[4]) = value
/* LMP_TEST_ACTIVATE */
#define SET_LMP_TEST_ACTIVATE_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TEST_ACTIVATE_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x38)
/* LMP_TEST_CONTROL */
#define SET_LMP_TEST_CONTROL_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_TEST_CONTROL_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x39)
#define SET_LMP_TEST_CONTROL_PARAMETER_TEST_SENARIO(buf, value)  buf[4] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_HOPPING_MODE(buf, value)  buf[5] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_TX_FREQUENCY(buf, value)  buf[6] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_RX_FREQUENCY(buf, value)  buf[7] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_POWER_MODE(buf, value)  buf[8] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_POLL_PERIOD(buf, value)  buf[9] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_PACKET_TYPE(buf, value)  buf[10] = value
#define SET_LMP_TEST_CONTROL_PARAMETER_TEST_DATA_LENGTH(buf, value)  *((uint16_t *)&buf[11]) = value
/* LMP_ENCRYPTION_KEY_SIZE_MASK_REQ */
#define SET_LMP_ENCRYPTION_KEY_SIZE_MASK_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCRYPTION_KEY_SIZE_MASK_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3a)
/* LMP_ENCRYPTION_KEY_SIZE_MASK_RES */
#define SET_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3b)
#define SET_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_PARAMETER_KEY_SIZE_MASK(buf, value)  *((uint16_t *)&buf[4]) = value
/* LMP_SET_AFH */
#define SET_LMP_SET_AFH_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SET_AFH_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3c)
#define SET_LMP_SET_AFH_PARAMETER_AFH_INSTANT(buf, value)  *((uint32_t *)&buf[4]) = value
#define SET_LMP_SET_AFH_PARAMETER_AFH_MODE(buf, value)  buf[8] = value
#define SET_LMP_SET_AFH_PARAMETER_AFH_CHANNEL_MAP(buf, value)  memcpy((uint8_t *)&buf[9], value, 10)
/* LMP_ENCAPSULATED_HEADER */
#define SET_LMP_ENCAPSULATED_HEADER_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCAPSULATED_HEADER_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3d)
#define SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MAJOR_TYPE(buf, value)  buf[4] = value
#define SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MINOR_TYPE(buf, value)  buf[5] = value
#define SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_PAYLOAD_LENGTH(buf, value)  buf[6] = value
/* LMP_ENCAPSULATED_PAYLOAD */
#define SET_LMP_ENCAPSULATED_PAYLOAD_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ENCAPSULATED_PAYLOAD_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3e)
#define SET_LMP_ENCAPSULATED_PAYLOAD_PARAMETER_ENCAP_DATA(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_SIMPLE_PAIRING_CONFIRM */
#define SET_LMP_SIMPLE_PAIRING_CONFIRM_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SIMPLE_PAIRING_CONFIRM_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x3f)
#define SET_LMP_SIMPLE_PAIRING_CONFIRM_PARAMETER_COMMITMENT_VALUE(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_SIMPLE_PAIRING_NUMBER */
#define SET_LMP_SIMPLE_PAIRING_NUMBER_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SIMPLE_PAIRING_NUMBER_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x40)
#define SET_LMP_SIMPLE_PAIRING_NUMBER_PARAMETER_NONCVALUE(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_DHKEY_CHECK */
#define SET_LMP_DHKEY_CHECK_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_DHKEY_CHECK_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x41)
#define SET_LMP_DHKEY_CHECK_PARAMETER_CONFIRMATION_VALUE(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_PAUSE_ENCRYPTION_AES_REQ */
#define SET_LMP_PAUSE_ENCRYPTION_AES_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PAUSE_ENCRYPTION_AES_REQ_OPCODE(buf) *((uint8_t *)&buf[3])  = ((*((uint8_t *)&buf[3])) & (BIT(7)))|(0x42)
#define SET_LMP_PAUSE_ENCRYPTION_AES_REQ_PARAMETER_RANDOM_NUMBER(buf, value)  memcpy((uint8_t *)&buf[4], value, 16)
/* LMP_ACCEPTED_EXT */
#define SET_LMP_ACCEPTED_EXT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ACCEPTED_EXT_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1 << 8) | (0x7f)))
#define SET_LMP_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(buf, value)  buf[5] = value
/* LMP_NOT_ACCEPTED_EXT */
#define SET_LMP_NOT_ACCEPTED_EXT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_NOT_ACCEPTED_EXT_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x2 << 8) | (0x7f)))
#define SET_LMP_NOT_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(buf, value)  buf[5] = value
#define SET_LMP_NOT_ACCEPTED_EXT_PARAMETER_ERROR_CODE(buf, value)  buf[6] = value
/* LMP_FEATURES_REQ_EXT */
#define SET_LMP_FEATURES_REQ_EXT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_FEATURES_REQ_EXT_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x3 << 8) | (0x7f)))
#define SET_LMP_FEATURES_REQ_EXT_PARAMETER_FEATURES_PAGE(buf, value)  buf[5] = value
#define SET_LMP_FEATURES_REQ_EXT_PARAMETER_MAX_SUPPORTED_PAGE(buf, value)  buf[6] = value
#define SET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(buf, value)  memcpy((uint8_t *)&buf[7], &value, 8)
/* LMP_FEATURES_RES_EXT */
#define SET_LMP_FEATURES_RES_EXT_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_FEATURES_RES_EXT_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x4 << 8) | (0x7f)))
#define SET_LMP_FEATURES_RES_EXT_PARAMETER_FEATURES_PAGE(buf, value)  buf[5] = value
#define SET_LMP_FEATURES_RES_EXT_PARAMETER_MAX_SUPPORTED_PAGE(buf, value)  buf[6] = value
#define SET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(buf, value)  memcpy((uint8_t *)&buf[7], &value, 8)
/* LMP_CLK_ADJ */
#define SET_LMP_CLK_ADJ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CLK_ADJ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x5 << 8) | (0x7f)))
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_ID(buf, value)  buf[5] = value
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_INSTANT(buf, value)  *((uint32_t *)&buf[6]) = value
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_OFFSET(buf, value)  *((uint16_t *)&buf[10]) = value
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_SLOTS(buf, value)  buf[12] = value
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_MODE(buf, value)  buf[13] = value
#define SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_CLK(buf, value)  *((uint32_t *)&buf[14]) = value
/* LMP_CLK_ADJ_ACK */
#define SET_LMP_CLK_ADJ_ACK_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CLK_ADJ_ACK_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x6 << 8) | (0x7f)))
#define SET_LMP_CLK_ADJ_ACK_PARAMETER_CLK_ADJ_ID(buf, value)  buf[5] = value
/* LMP_CLK_ADJ_REQ */
#define SET_LMP_CLK_ADJ_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CLK_ADJ_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x7 << 8) | (0x7f)))
#define SET_LMP_CLK_ADJ_REQ_PARAMETER_CLK_ADJ_OFFSET(buf, value)  *((uint16_t *)&buf[5]) = value
#define SET_LMP_CLK_ADJ_REQ_PARAMETER_CLK_ADJ_SLOTS(buf, value)  buf[7] = value
#define SET_LMP_CLK_ADJ_REQ_PARAMETER_CLK_ADJ_PERIOD(buf, value)  buf[8] = value
/* LMP_PACKET_TYPE_TABLE_REQ */
#define SET_LMP_PACKET_TYPE_TABLE_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PACKET_TYPE_TABLE_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0xb << 8) | (0x7f)))
#define SET_LMP_PACKET_TYPE_TABLE_REQ_PARAMETER_PACKET_TYPE_TABLE(buf, value)  buf[5] = value
/* LMP_ESCO_LINK_REQ */
#define SET_LMP_ESCO_LINK_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_ESCO_LINK_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0xc << 8) | (0x7f)))
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(buf, value)  buf[5] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_LT_ADDR(buf, value)  buf[6] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(buf, value)  buf[7] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_D_ESCO(buf, value)  buf[8] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_T_ESCO(buf, value)  buf[9] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_W_ESCO(buf, value)  buf[10] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_C_P(buf, value)  buf[11] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_P_C(buf, value)  buf[12] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_C_P(buf, value)  *((uint16_t *)&buf[13]) = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_P_C(buf, value)  *((uint16_t *)&buf[15]) = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_AIR_MODE(buf, value)  buf[17] = value
#define SET_LMP_ESCO_LINK_REQ_PARAMETER_NEGOTIATION_STATE(buf, value)  buf[18] = value
/* LMP_REMOVE_ESCO_LINK_REQ */
#define SET_LMP_REMOVE_ESCO_LINK_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_REMOVE_ESCO_LINK_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0xd << 8) | (0x7f)))
#define SET_LMP_REMOVE_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(buf, value)  buf[5] = value
#define SET_LMP_REMOVE_ESCO_LINK_REQ_PARAMETER_ERROR_CODE(buf, value)  buf[6] = value
/* LMP_CHANNEL_CLASSIFICATION_REQ */
#define SET_LMP_CHANNEL_CLASSIFICATION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CHANNEL_CLASSIFICATION_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x10 << 8) | (0x7f)))
#define SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_REPORTING_MODE(buf, value)  buf[5] = value
#define SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MIN_INTERVAL(buf, value)  *((uint16_t *)&buf[6]) = value
#define SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MAX_INTERVAL(buf, value)  *((uint16_t *)&buf[8]) = value
/* LMP_CHANNEL_CLASSIFICATION */
#define SET_LMP_CHANNEL_CLASSIFICATION_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_CHANNEL_CLASSIFICATION_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x11 << 8) | (0x7f)))
#define SET_LMP_CHANNEL_CLASSIFICATION_PARAMETER_AFH_CHANNEL_CLASSIFICATION(buf, value)  memcpy((uint8_t *)&buf[5], &value, 10)
/* LMP_SNIFF_SUBRATING_REQ */
#define SET_LMP_SNIFF_SUBRATING_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SNIFF_SUBRATING_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x15 << 8) | (0x7f)))
#define SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MAX_SNIFF_SUBRATE(buf, value)  buf[5] = value
#define SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MIN_SNIFF_MODE_TIEOUT(buf, value)  *((uint16_t *)&buf[6]) = value
#define SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_SNIFF_SUBRATING_INSTANT(buf, value)  *((uint32_t *)&buf[8]) = value
/* LMP_SNIFF_SUBRATING_RES */
#define SET_LMP_SNIFF_SUBRATING_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SNIFF_SUBRATING_RES_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x16 << 8) | (0x7f)))
#define SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MAX_SNIFF_SUBRATE(buf, value)  buf[5] = value
#define SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MIN_SNIFF_MODE_TIMEOUT(buf, value)  *((uint16_t *)&buf[6]) = value
#define SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_SNIFF_SUBRATING_INSTANT(buf, value)  *((uint32_t *)&buf[8]) = value
/* LMP_PAUSE_ENCRYPTION_REQ */
#define SET_LMP_PAUSE_ENCRYPTION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PAUSE_ENCRYPTION_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x17 << 8) | (0x7f)))
/* LMP_RESUME_ENCRYPTION_REQ */
#define SET_LMP_RESUME_ENCRYPTION_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_RESUME_ENCRYPTION_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x18 << 8) | (0x7f)))
/* LMP_IO_CAPABILITY_REQ */
#define SET_LMP_IO_CAPABILITY_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_IO_CAPABILITY_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x19 << 8) | (0x7f)))
#define SET_LMP_IO_CAPABILITY_REQ_PARAMETER_IO_CAPABILITIES(buf, value)  buf[5] = value
#define SET_LMP_IO_CAPABILITY_REQ_PARAMETER_OOB_AUTH_DATA(buf, value)  buf[6] = value
#define SET_LMP_IO_CAPABILITY_REQ_PARAMETER_AUTHENTICATION_REQUIREMENTS(buf, value)  buf[7] = value
/* LMP_IO_CAPABILITY_RES */
#define SET_LMP_IO_CAPABILITY_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_IO_CAPABILITY_RES_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1a << 8) | (0x7f)))
#define SET_LMP_IO_CAPABILITY_RES_PARAMETER_IO_CAPABILITIES(buf, value)  buf[5] = value
#define SET_LMP_IO_CAPABILITY_RES_PARAMETER_OOB_AUTH_DATA(buf, value)  buf[6] = value
#define SET_LMP_IO_CAPABILITY_RES_PARAMETER_AUTHENTICATION_REQUIREMENTS(buf, value)  buf[7] = value
/* LMP_NUMERIC_COMPARISON_FAILED */
#define SET_LMP_NUMERIC_COMPARISON_FAILED_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_NUMERIC_COMPARISON_FAILED_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1b << 8) | (0x7f)))
/* LMP_PASSKEY_FAILED */
#define SET_LMP_PASSKEY_FAILED_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PASSKEY_FAILED_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1c << 8) | (0x7f)))
/* LMP_OOB_FAILED */
#define SET_LMP_OOB_FAILED_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_OOB_FAILED_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1d << 8) | (0x7f)))
/* LMP_KEYPRESS_NOTIFICATION */
#define SET_LMP_KEYPRESS_NOTIFICATION_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_KEYPRESS_NOTIFICATION_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1e << 8) | (0x7f)))
#define SET_LMP_KEYPRESS_NOTIFICATION_PARAMETER_NOTIFICATION_TYPE(buf, value)  buf[5] = value
/* LMP_POWER_CONTROL_REQ */
#define SET_LMP_POWER_CONTROL_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_POWER_CONTROL_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x1f << 8) | (0x7f)))
#define SET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(buf, value)  buf[5] = value
/* LMP_POWER_CONTROL_RES */
#define SET_LMP_POWER_CONTROL_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_POWER_CONTROL_RES_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x20 << 8) | (0x7f)))
#define SET_LMP_POWER_CONTROL_RES_PARAMETER_POWER_ADJ_RSP(buf, value)  buf[5] = value
/* LMP_PING_REQ */
#define SET_LMP_PING_REQ_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PING_REQ_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x21 << 8) | (0x7f)))
/* LMP_PING_RES */
#define SET_LMP_PING_RES_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_PING_RES_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x22 << 8) | (0x7f)))
/* LMP_SAM_SET_TYPE0 */
#define SET_LMP_SAM_SET_TYPE0_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SAM_SET_TYPE0_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x23 << 8) | (0x7f)))
#define SET_LMP_SAM_SET_TYPE0_PARAMETER_UPDATE_MODE(buf, value)  buf[5] = value
#define SET_LMP_SAM_SET_TYPE0_PARAMETER_SAM_TYPE0_SUBMAP(buf, value)  memcpy((uint8_t *)&buf[6], &value, 14)
/* LMP_SAM_DEFINE_MAP */
#define SET_LMP_SAM_DEFINE_MAP_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SAM_DEFINE_MAP_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x24 << 8) | (0x7f)))
#define SET_LMP_SAM_DEFINE_MAP_PARAMETER_SAM_INDEX(buf, value)  buf[5] = value
#define SET_LMP_SAM_DEFINE_MAP_PARAMETER_T_SAM_SM(buf, value)  buf[6] = value
#define SET_LMP_SAM_DEFINE_MAP_PARAMETER_N_SAM_SM(buf, value)  buf[7] = value
#define SET_LMP_SAM_DEFINE_MAP_PARAMETER_SAM_SUBMAPS(buf, value)  memcpy((uint8_t *)&buf[8], &value, 12)
/* LMP_SAM_SWITCH */
#define SET_LMP_SAM_SWITCH_TID(buf,tid)  {\
                buf[3]= (tid << 7)|(buf[3]);\
        }
#define SET_LMP_SAM_SWITCH_OPCODE(buf)  *((uint16_t *)&buf[3])= ((buf[3] & (BIT(15)))|((0x25 << 8) | (0x7f)))
#define SET_LMP_SAM_SWITCH_PARAMETER_SAM_INDEX(buf, value)  buf[5] = value
#define SET_LMP_SAM_SWITCH_PARAMETER_TIMING_CONTROL_FLAGS(buf, value)  buf[6] = value
#define SET_LMP_SAM_SWITCH_PARAMETER_D_SAM(buf, value)  buf[7] = value
#define SET_LMP_SAM_SWITCH_PARAMETER_SAM_INSTANT(buf, value)  *((uint32_t *)&buf[8]) = value
#endif