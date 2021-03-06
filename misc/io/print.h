//
// Created by petr on 11/16/19.
//

#ifndef TERRAINGENERATION_PRINT_H
#define TERRAINGENERATION_PRINT_H

#include "logger.h"
#include <iostream>

namespace detail {
template <typename OutStream, typename... Args> void printImpl(OutStream &stream, const Args &... args) {
  using namespace LoggerStreamModifiers;
  static auto logger = Logger{stream};
  ((logger << verbose()) << ... << args) << "\n";
}

template <typename OutStream> class CustomPrint {
  OutStream &stream;

public:
  explicit CustomPrint(OutStream &stream) : stream(stream) {}

  template <typename... Args> void operator()(Args... args) { printImpl(stream, args...); }
};
} // namespace detail
template <typename... Args> void print(const Args &... args) {
  detail::printImpl(std::cout, std::forward<const Args &>(args)...);
}

template <typename... Args> void printErr(const Args &... args) {
  detail::printImpl(std::cerr, std::forward<const Args &>(args)...);
}

template <typename OutStream> auto make_print(OutStream &stream) { return detail::CustomPrint{stream}; }

/*
template <typename OutStream>
auto make_print(OutStream &stream) {
  return [&stream]<typename ...Args> (Args... args) {
    detail::printImpl(stream, std::forward<Args>(args)...);
  };
}
*/

#endif // TERRAINGENERATION_PRINT_H
