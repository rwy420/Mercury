#ifndef __MERCURY__TYPES_H
#define __MERCURY__TYPES_H

// TODO: use these everywhere
#define ALIGN_DOWN(value, alignment) ((value) & ~((alignment) - 1))
#define ALIGN_UP(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef uint32_t size_t;

#endif
