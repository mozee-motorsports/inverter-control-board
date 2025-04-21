#ifndef CAN_FRAME_H
#define CAN_FRAME_H

typedef enum {
    SAFETY_SYSTEM = 0,
    BROADCAST = 1,
    THROTTLE_CONTROL_BOARD = 2,
    PEDAL_BOX = 3,
    STEERING_WHEEL = 4,
    THERMO_CONTROL_BOARD = 5,
    ISOLATION_EXPANSION_DEVICE = 6,
} eModule;

typedef enum {
    TO = 0,
    FROM = 1,
} eDirection;

typedef struct {
    uint8_t priority; 
    eModule module; 
    eDirection direction;
    uint8_t command;
} sCAN_Header; 

sCAN_Header parse_id(uint32_t id);

#endif