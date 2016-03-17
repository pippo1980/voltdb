/* This file is part of VoltDB.
 * Copyright (C) 2008-2016 VoltDB Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UNIQUEID_H_
#define UNIQUEID_H_

#include "common/types.h"
#include <cassert>

namespace voltdb
{
class UniqueId {
private:
    const static int TIMESTAMP_BITS = 40;
    const static int COUNTER_BITS = 9;
    const static int PARTITIONID_BITS = 14;
    const static int64_t VOLT_EPOCH = 1199145600000L;
    const static int64_t TIMESTAMP_MAX_VALUE = (1L << TIMESTAMP_BITS) - 1L;
    const static int64_t COUNTER_MAX_VALUE = (1L << COUNTER_BITS) - 1L;
    const static int64_t TIMESTAMP_PLUS_COUNTER_MAX_VALUE = (1LL << (TIMESTAMP_BITS + COUNTER_BITS)) - 1LL;
    const static int64_t PARTITIONID_MAX_VALUE = (1L << PARTITIONID_BITS) - 1L;
    const static int32_t PARTITION_ID_MASK = (1L << PARTITIONID_BITS) - 1L;
    const static int32_t MP_INITIATOR_PID = PARTITION_ID_MASK;

public:
    static int64_t makeIdFromComponents(int64_t ts, int64_t seqNo, int64_t partitionId) {
        // compute the time in millis since VOLT_EPOCH
        int64_t uniqueId = ts - VOLT_EPOCH;
        // verify all fields are the right size
        assert(uniqueId <= TIMESTAMP_MAX_VALUE);
        assert(seqNo <= COUNTER_MAX_VALUE);
        assert(partitionId <= PARTITIONID_MAX_VALUE);

        // put this time value in the right offset
        uniqueId = uniqueId << (COUNTER_BITS + PARTITIONID_BITS);
        // add the counter value at the right offset
        uniqueId |= seqNo << PARTITIONID_BITS;
        // finally add the partition id at the end
        uniqueId |= partitionId;

        return uniqueId;
    }

    static int32_t pid(int64_t uid) {
        return static_cast<int32_t>(uid) & PARTITION_ID_MASK;
    }

    static bool isMpUniqueId(int64_t uid) {
        return pid(uid) == MP_INITIATOR_PID;
    }

    static int64_t timestampAndCounter(int64_t uid) {
        return (uid >> PARTITIONID_BITS) & TIMESTAMP_PLUS_COUNTER_MAX_VALUE;
    }
};
}
#endif /* UNIQUEID_H_ */
