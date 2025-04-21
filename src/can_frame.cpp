#include "can_frame.hpp"

sCAN_Header parse_id(uint32_t id) {
    return sCAN_Header {
        .priority = (uint8_t)((id >> 8) & 0b0111),
        .module  = (eModule)((id >> 5) & 0b0111),
        .direction = (eDirection)(id >> 4),
        .command = (uint8_t)(id & 0xF),
    };
}