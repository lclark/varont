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
#ifndef __VARONT_LIFECYCLEAWARE_HPP__
#define __VARONT_LIFECYCLEAWARE_HPP__

namespace varont {

/**
 * Implement this interface to be notified when a thread for the {@link BatchEventProcessor} starts and shuts down.
 */
class LifecycleAware {
 public:
  /**
   * Called once on thread start before first event is available.
   */
  virtual void onStart() = 0;

  /**
   * Called once just before the thread is shutdown.
   */
  virtual void onShutdown() = 0;

 protected:
  ~LifecycleAware() {}
};

}

#endif /* __VARONT_LIFECYCLEAWARE_HPP__ */
