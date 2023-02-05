#ifndef MSG_H
#define MSG_H

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#include <string>

namespace msg {
enum MsgType { Error,
               Warning,
               Info,
               Happy,
               Std };
template <typename T>
void Msg(T value, msg::MsgType type, bool bold);
}  // namespace msg

void MsgPrefix(std::string& prefix, fmt::color& col, msg::MsgType type);

template <typename T>
void msg::Msg(T value, msg::MsgType type, bool bold) {
    fmt::color col;
    std::string prefix = "";
    MsgPrefix(prefix, col, type);
    std::time_t t = std::time(nullptr);
    if (bold)
        fmt::print(fmt::emphasis::bold | fg(col), "{:%H:%M:%S} {:>7}: {}\n", fmt::localtime(t), prefix, value);
    else
        fmt::print(fg(col), "{:>18}{}\n", "", value);
}

#endif  // MSG_H