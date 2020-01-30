#ifndef AVIONICS_COMMON_H
#define AVIONICS_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define DESERIALIZE(type, size)                             \
    static inline type to_##type(uint8_t* src){             \
        union {                                             \
            type value;                                     \
            char bytes[size];                               \
        } u;                                                \
        memcpy(u.bytes, src, 4);                            \
        return u.value;                                     \
    }                                                       \

#define SERIALIZE(type, size)                                   \
    static inline void from_##type(type value, uint8_t* buf){   \
        union {                                                 \
            type value;                                         \
            char bytes[size];                                   \
        } u;                                                    \
        u.value = value;                                        \
        memcpy(buf, u.bytes, 4);                                \
    }                                                           \


SERIALIZE(double, 4)
SERIALIZE(float, 4)
SERIALIZE(uint32_t, 4)
SERIALIZE(int32_t, 4)

DESERIALIZE(double, 4)
DESERIALIZE(float,4)
DESERIALIZE(uint32_t,4)
DESERIALIZE(int32_t,4)


static inline void write_32(uint32_t src, uint8_t * dest)
{
    dest[0] = (uint8_t) ((src >> 24) & 0xFF);
    dest[1] = (uint8_t) ((src >> 16) & 0xFF);
    dest[2] = (uint8_t) ((src >> 8) & 0xFF);
    dest[3] = (uint8_t) ((src >> 0) & 0xFF);
}

static inline void write_24(uint32_t src, uint8_t * dest)
{
    dest[1] = (uint8_t) ((src >> 16) & 0xFF);
    dest[2] = (uint8_t) ((src >> 8) & 0xFF);
    dest[3] = (uint8_t) ((src >> 0) & 0xFF);
}

static inline void write_16(uint16_t src, uint8_t * dest)
{
    dest[0] = (uint8_t) ((src >> 8) & 0xFF);
    dest[1] = (uint8_t) ((src >> 0) & 0xFF);
}

static inline uint32_t read_32(const uint8_t * src)
{
    return ((((uint8_t) src[0]) << 24) & 0xFF) |
           ((((uint8_t) src[1]) << 16) & 0xFF) |
           ((((uint8_t) src[2]) << 8) & 0xFF)  |
           ((((uint8_t) src[3]) << 0) & 0xFF);
}

static inline uint32_t read_24(const uint8_t * src)
{
    return ((((uint8_t) src[1]) << 16) & 0xFF) |
           ((((uint8_t) src[2]) << 8) & 0xFF)  |
           ((((uint8_t) src[3]) << 0) & 0xFF);
}

static inline uint16_t read_16(const uint8_t * src)
{
    return ((((uint8_t) src[0]) << 8) & 0xFF) |
           ((((uint8_t) src[1]) << 0) & 0xFF);
}

static inline bool is_buffer_empty(uint8_t * buffer, size_t size)
{
    for(size_t i = 0; i < size; i++)
            if(buffer[i] != 0)
                    return false;
    return true;
}

static inline void clear_buffer(uint8_t * buffer, size_t size)
{
    for(size_t i = 0; i < size; i++)
            buffer[i] = 0;
}

static inline void float2bytes(float float_variable, uint8_t bytes_temp[4])
{
    union {
        float a;
        unsigned char bytes[4];
    } thing;
    
    thing.a = float_variable;
    memcpy(bytes_temp, thing.bytes, 4);
}

#endif //AVIONICS_COMMON_H
