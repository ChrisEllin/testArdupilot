// fielddecode.h was generated by ProtoGen version 2.18.c

#ifndef _FIELDDECODE_H
#define _FIELDDECODE_H

// C++ compilers: don't mangle us
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * fielddecode provides routines to pull numbers from a byte stream.
 *
 * fielddecode provides routines to pull numbers in local memory layout from
 * a big or little endian byte stream. It is the opposite operation from the
 * routines contained in fieldencode.h
 *
 * When compressing unsigned numbers (for example 32-bits to 16-bits) the most
 * signficant bytes are discarded and the only requirement is that the value of
 * the number fits in the smaller width. When going the other direction the
 * most significant bytes are simply set to 0x00. However signed two's
 * complement numbers are more complicated.
 *
 * If the signed value is a positive number that fits in the range then the
 * most significant byte will be zero, and we can discard it. If the signed
 * value is negative (in two's complement) then the most significant bytes are
 * 0xFF and again we can throw them away. See the example below
 *
 * 32-bit +100 | 16-bit +100 | 8-bit +100
 *  0x00000064 |      0x0064 |       0x64 <-- notice most significant bit clear
 *
 * 32-bit -100 | 16-bit -100 | 8-bit -100
 *  0xFFFFFF9C |      0xFF9C |       0x9C <-- notice most significant bit set
 *
 * The signed complication comes when going the other way. If the number is
 * positive setting the most significant bytes to zero is correct. However
 * if the number is negative the most significant bytes must be set to 0xFF.
 * This is the process of sign-extension. Typically this is handled by the
 * compiler. For example if a int16_t is assigned to an int32_t the compiler
 * (or the processor instruction) knows to perform the sign extension. However
 * in our case we can decode signed 24-bit numbers (for example) which are
 * returned to the caller as int32_t. In this instance fielddecode performs the
 * sign extension.
 */

#define __STDC_CONSTANT_MACROS
#include <stdint.h>

//! Decode a null terminated string from a byte stream
void stringFromBytes(char* string, const uint8_t* bytes, int* index, int maxLength, int fixedLength);

//! Copy an array of bytes from a byte stream without changing the order.
void bytesFromBeBytes(uint8_t* data, const uint8_t* bytes, int* index, int num);

//! Copy an array of bytes from a byte stream while reversing the order.
void bytesFromLeBytes(uint8_t* data, const uint8_t* bytes, int* index, int num);

//! Decode a 4 byte float from a big endian byte stream.
float float32FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a 4 byte float from a little endian byte stream.
float float32FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 4 byte integer from a big endian byte stream.
uint32_t uint32FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 4 byte integer from a little endian byte stream.
uint32_t uint32FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 4 byte integer from a big endian byte stream.
int32_t int32FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 4 byte integer from a little endian byte stream.
int32_t int32FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 3 byte integer from a big endian byte stream.
uint32_t uint24FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 3 byte integer from a little endian byte stream.
uint32_t uint24FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 3 byte integer from a big endian byte stream.
int32_t int24FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 3 byte integer from a little endian byte stream.
int32_t int24FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 2 byte integer from a big endian byte stream.
uint16_t uint16FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 2 byte integer from a little endian byte stream.
uint16_t uint16FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 2 byte integer from a big endian byte stream.
int16_t int16FromBeBytes(const uint8_t* bytes, int* index);

//! Decode a signed 2 byte integer from a little endian byte stream.
int16_t int16FromLeBytes(const uint8_t* bytes, int* index);

//! Decode a unsigned 1 byte integer from a byte stream.
#define uint8FromBytes(bytes, index) (uint8_t)((bytes)[(*(index))++])

//! Decode a signed 1 byte integer from a byte stream.
#define int8FromBytes(bytes, index) (int8_t)((bytes)[(*(index))++])

#ifdef __cplusplus
}
#endif
#endif
