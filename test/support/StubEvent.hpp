#include <string>

namespace disruptor {
namespace test {

using std::string;

struct StubEvent {
  int value;
  string testString;

  StubEvent()
      : value(-1)
  {}

  StubEvent(int i)
      : value(i)
  {}

  StubEvent(const StubEvent& orig)
      : value(orig.value)
  {}

  StubEvent& operator=(const StubEvent& orig) {
    value = orig.value;
    return *this;
  }

  const int get() const {
    return value;
  }

  void setValue(int i) {
    value = i;
  }

  bool operator==(const StubEvent& other) const {
    return value == other.value;
  }
};

}
}
