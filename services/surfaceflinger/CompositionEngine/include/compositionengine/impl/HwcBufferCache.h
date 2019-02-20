/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include <vector>

#include <gui/BufferQueue.h>
#include <utils/StrongPointer.h>

namespace android {

class GraphicBuffer;

namespace compositionengine::impl {

// With HIDLized hwcomposer HAL, the HAL can maintain a buffer cache for each
// HWC display and layer.  When updating a display target or a layer buffer,
// we have the option to send the buffer handle over or to request the HAL to
// retrieve it from its cache.  The latter is cheaper since it eliminates the
// overhead to transfer the handle over the trasport layer, and the overhead
// for the HAL to clone and retain the handle.
//
// To be able to find out whether a buffer is already in the HAL's cache, we
// use HWComposerBufferCache to mirror the cache in SF.
class HwcBufferCache {
public:
    HwcBufferCache();
    // Given a buffer, return the HWC cache slot and
    // buffer to be sent to HWC.
    //
    // outBuffer is set to buffer when buffer is not in the HWC cache;
    // otherwise, outBuffer is set to nullptr.
    void getHwcBuffer(const sp<GraphicBuffer>& buffer, uint32_t* outSlot,
                      sp<GraphicBuffer>* outBuffer);

protected:
    int getSlot(const sp<GraphicBuffer>& buffer);
    int getLeastRecentlyUsedSlot();
    uint64_t getCounter();

private:
    // an array where the index corresponds to a slot and the value corresponds to a (counter,
    // buffer) pair. "counter" is a unique value that indicates the last time this slot was updated
    // or used and allows us to keep track of the least-recently used buffer.
    std::pair<uint64_t, wp<GraphicBuffer>> mBuffers[BufferQueue::NUM_BUFFER_SLOTS];

    // The cache increments this counter value when a slot is updated or used.
    // Used to track the least recently-used buffer
    uint64_t mCounter = 1;
};

} // namespace compositionengine::impl
} // namespace android
