#include <string>

namespace compileus {
// Demangles a C++ symbol. Demangling an invalid name is a
// programming error, not a runtime error.
auto demangle(const std::string& mangled) -> std::string;

template <typename CB_T>
class [[nodiscard]] on_scope_exit_t {
 public:
  explicit on_scope_exit_t(CB_T cb) : cb_(std::move(cb)) {}

  ~on_scope_exit_t() {
    if (!cancelled_) {
      cb_();
    }
  }
  void cancel() { cancelled_ = true; }

 private:
  CB_T cb_;
  bool cancelled_ = false;
};

template <typename CB_T>
auto on_scope_exit(CB_T cb) {
  return on_scope_exit_t<CB_T>{std::move(cb)};
}
}  // namespace compileus