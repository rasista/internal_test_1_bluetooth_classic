#ifndef ULC_SCHED_INTF_H_
#define ULC_SCHED_INTF_H_

#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "usched_api.h"
#include "usched_common_role.h"
#include "usched_pi.h"
#include "usched_pi_scan.h"
#include "usched_acl.h"

void scheduler_activity_pending_handler(void *param);

#endif
