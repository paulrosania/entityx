//  Inspired heavily by boost::noncopyable

#pragma once

namespace entityx {
namespace help {

#define NONCOPYABLE(Class)                \
  Class(const Class&) = delete;           \
  Class& operator=(const Class&) = delete

class NonCopyable {
protected:
  NonCopyable() = default;
  ~NonCopyable() = default;


  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator = (const NonCopyable &) = delete;
};


}  // namespace help
}  // namespace entityx
