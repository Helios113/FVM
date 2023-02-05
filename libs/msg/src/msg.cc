#include <msg.h>
void MsgPrefix(std::string& prefix, fmt::color& col, msg::MsgType type) {
    switch (type) {
        case msg::Error:
            col = fmt::color::red;
            prefix = "ERROR";
            break;
        case msg::Warning:
            col = fmt::color::yellow;
            prefix = "WARNING";
            break;
        case msg::Info:
            col = fmt::color::blue;
            prefix = "INFO";
            break;
        case msg::Happy:
            col = fmt::color::green;
            prefix = "SUCCESS";
            break;
        case msg::Std:
            col = fmt::color::white;
            break;
        default:
            col = fmt::color::white;
            break;
    }
}
