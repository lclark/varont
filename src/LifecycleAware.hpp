#ifndef __DISRUPTOR_LIFECYCLEAWARE_HPP__
#define __DISRUPTOR_LIFECYCLEAWARE_HPP__

namespace disruptor {

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

#endif /* __DISRUPTOR_LIFECYCLEAWARE_HPP__ */
