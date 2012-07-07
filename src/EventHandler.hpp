/*
 * Copyright 2012 Leonard Clark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __VARONT_EVENTHANDLER_HPP__
#define __VARONT_EVENTHANDLER_HPP__

namespace varont {

/**
 * Callback interface to be implemented for processing events as they become available in the {@link RingBuffer}
 *
 * @see BatchEventProcessor#setExceptionHandler(ExceptionHandler) if you want to handle exceptions propigated out of the handler.
 *
 * @param <T> event implementation storing the data for sharing during exchange or parallel coordination of an event.
 */
template <typename T>
class EventHandler {
 public:
  /**
   * Called when a publisher has published an event to the {@link RingBuffer}
   *
   * @param event published to the {@link RingBuffer}
   * @param sequence of the event being processed
   * @param endOfBatch flag to indicate if this is the last event in a batch from the {@link RingBuffer}
   * @throws Exception if the EventHandler would like the exception handled further up the chain.
   */
  virtual void onEvent(T& event, long sequence, bool endOfBatch) = 0;

 protected:
  ~EventHandler() {}
};

}

#endif /* __VARONT_EVENTHANDLER_HPP__ */
