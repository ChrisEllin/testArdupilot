/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Notify display driver for 128 x 64 pixel displays */
#include "Display.h"
#include "Display_SH1106_I2C.h"
#include "Display_SSD1306_I2C.h"
#include "Display_SITL.h"

#include "AP_Notify.h"

#include <stdio.h>
#include <AP_GPS/AP_GPS.h>
#include <AP_BattMonitor/AP_BattMonitor.h>

#include <utility>

extern const AP_HAL::HAL& hal;

// Bit Map
// Rotate right 90 degrees
// Ex: 0x7C, 0x12, 0x11, 0x12, 0x7C,  // 'A'
// 0XXXXX00
// 000X00X0
// 000X000X
// 000X00X0
// 0XXXXX00
static const uint8_t _font[] = {
#ifdef AP_NOTIFY_DISPLAY_USE_EMOJI
    // 0x Group
    0x00, 0x00, 0x00, 0x00, 0x00,  // 00
    0x3E, 0x5B, 0x4F, 0x5B, 0x3E,  // 01
    0x3E, 0x6B, 0x4F, 0x6B, 0x3E,  // 02
    0x1C, 0x3E, 0x7C, 0x3E, 0x1C,  // 03
    0x18, 0x3C, 0x7E, 0x3C, 0x18,  // 04
    0x1C, 0x57, 0x7D, 0x57, 0x1C,  // 05
    0x1C, 0x5E, 0x7F, 0x5E, 0x1C,  // 06
    0x00, 0x18, 0x3C, 0x18, 0x00,  // 07
    0xFF, 0xE7, 0xC3, 0xE7, 0xFF,  // 08
    0x00, 0x18, 0x24, 0x18, 0x00,  // 09
    0xFF, 0xE7, 0xDB, 0xE7, 0xFF,  // 0A
    0x30, 0x48, 0x3A, 0x06, 0x0E,  // 0B
    0x26, 0x29, 0x79, 0x29, 0x26,  // 0C
    0x40, 0x7F, 0x05, 0x05, 0x07,  // 0D
    0x40, 0x7F, 0x05, 0x25, 0x3F,  // 0E
    0x5A, 0x3C, 0xE7, 0x3C, 0x5A,  // 0F
    // 1x Group
    0x7F, 0x3E, 0x1C, 0x1C, 0x08,  // 10
    0x08, 0x1C, 0x1C, 0x3E, 0x7F,  // 11
    0x14, 0x22, 0x7F, 0x22, 0x14,  // 12
    0x5F, 0x5F, 0x00, 0x5F, 0x5F,  // 13
    0x06, 0x09, 0x7F, 0x01, 0x7F,  // 14
    0x00, 0x66, 0x89, 0x95, 0x6A,  // 15
    0x60, 0x60, 0x60, 0x60, 0x60,  // 16
    0x94, 0xA2, 0xFF, 0xA2, 0x94,  // 17
    0x08, 0x04, 0x7E, 0x04, 0x08,  // 18
    0x10, 0x20, 0x7E, 0x20, 0x10,  // 19
    0x08, 0x08, 0x2A, 0x1C, 0x08,  // 1A
    0x08, 0x1C, 0x2A, 0x08, 0x08,  // 1B
    0x1E, 0x10, 0x10, 0x10, 0x10,  // 1C
    0x0C, 0x1E, 0x0C, 0x1E, 0x0C,  // 1D
    0x30, 0x38, 0x3E, 0x38, 0x30,  // 1E
    0x06, 0x0E, 0x3E, 0x0E, 0x06,  // 1F
#endif
    // 2x Group
    0x00, 0x00, 0x00, 0x00, 0x00,  // ' '
    0x00, 0x00, 0x5F, 0x00, 0x00,  // '!'
    0x00, 0x07, 0x00, 0x07, 0x00,  // '"'
    0x14, 0x7F, 0x14, 0x7F, 0x14,  // '#'
    0x24, 0x2A, 0x7F, 0x2A, 0x12,  // '$'
    0x23, 0x13, 0x08, 0x64, 0x62,  // '%'
    0x36, 0x49, 0x56, 0x20, 0x50,  // '&'
    0x00, 0x08, 0x07, 0x03, 0x00,  // '''
    0x00, 0x1C, 0x22, 0x41, 0x00,  // '('
    0x00, 0x41, 0x22, 0x1C, 0x00,  // ')'
    0x2A, 0x1C, 0x7F, 0x1C, 0x2A,  // '*'
    0x08, 0x08, 0x3E, 0x08, 0x08,  // '+'
    0x00, 0x80, 0x70, 0x30, 0x00,  // ','
    0x08, 0x08, 0x08, 0x08, 0x08,  // '-'
    0x00, 0x00, 0x60, 0x60, 0x00,  // '.'
    0x20, 0x10, 0x08, 0x04, 0x02,  // '/'
    // 3x Group
    0x3E, 0x51, 0x49, 0x45, 0x3E,  // '0'
    0x00, 0x42, 0x7F, 0x40, 0x00,  // '1'
    0x72, 0x49, 0x49, 0x49, 0x46,  // '2'
    0x21, 0x41, 0x49, 0x4D, 0x33,  // '3'
    0x18, 0x14, 0x12, 0x7F, 0x10,  // '4'
    0x27, 0x45, 0x45, 0x45, 0x39,  // '5'
    0x3C, 0x4A, 0x49, 0x49, 0x31,  // '6'
    0x41, 0x21, 0x11, 0x09, 0x07,  // '7'
    0x36, 0x49, 0x49, 0x49, 0x36,  // '8'
    0x46, 0x49, 0x49, 0x29, 0x1E,  // '9'
    0x00, 0x00, 0x14, 0x00, 0x00,  // ':'
    0x00, 0x40, 0x34, 0x00, 0x00,  // ';'
    0x00, 0x08, 0x14, 0x22, 0x41,  // '<'
    0x14, 0x14, 0x14, 0x14, 0x14,  // '='
    0x00, 0x41, 0x22, 0x14, 0x08,  // '>'
    0x02, 0x01, 0x59, 0x09, 0x06,  // '?'
    // 4x Group
    0x3E, 0x41, 0x5D, 0x59, 0x4E,  // '@'
    0x7C, 0x12, 0x11, 0x12, 0x7C,  // 'A'
    0x7F, 0x49, 0x49, 0x49, 0x36,  // 'B'
    0x3E, 0x41, 0x41, 0x41, 0x22,  // 'C'
    0x7F, 0x41, 0x41, 0x41, 0x3E,  // 'D'
    0x7F, 0x49, 0x49, 0x49, 0x41,  // 'E'
    0x7F, 0x09, 0x09, 0x09, 0x01,  // 'F'
    0x3E, 0x41, 0x41, 0x51, 0x73,  // 'G'
    0x7F, 0x08, 0x08, 0x08, 0x7F,  // 'H'
    0x00, 0x41, 0x7F, 0x41, 0x00,  // 'I'
    0x20, 0x40, 0x41, 0x3F, 0x01,  // 'J'
    0x7F, 0x08, 0x14, 0x22, 0x41,  // 'K'
    0x7F, 0x40, 0x40, 0x40, 0x40,  // 'L'
    0x7F, 0x02, 0x1C, 0x02, 0x7F,  // 'M'
    0x7F, 0x04, 0x08, 0x10, 0x7F,  // 'N'
    0x3E, 0x41, 0x41, 0x41, 0x3E,  // 'O'
    // 5x Group
    0x7F, 0x09, 0x09, 0x09, 0x06,  // 'P'
    0x3E, 0x41, 0x51, 0x21, 0x5E,  // 'Q'
    0x7F, 0x09, 0x19, 0x29, 0x46,  // 'R'
    0x26, 0x49, 0x49, 0x49, 0x32,  // 'S'
    0x03, 0x01, 0x7F, 0x01, 0x03,  // 'T'
    0x3F, 0x40, 0x40, 0x40, 0x3F,  // 'U'
    0x1F, 0x20, 0x40, 0x20, 0x1F,  // 'V'
    0x3F, 0x40, 0x38, 0x40, 0x3F,  // 'W'
    0x63, 0x14, 0x08, 0x14, 0x63,  // 'X'
    0x03, 0x04, 0x78, 0x04, 0x03,  // 'Y'
    0x61, 0x59, 0x49, 0x4D, 0x43,  // 'Z'
    0x00, 0x7F, 0x41, 0x41, 0x41,  // '['
    0x02, 0x04, 0x08, 0x10, 0x20,  // '\'
    0x00, 0x41, 0x41, 0x41, 0x7F,  // ']'
    0x04, 0x02, 0x01, 0x02, 0x04,  // '^'
    0x40, 0x40, 0x40, 0x40, 0x40,  // '_'
    // 6x Group
    0x00, 0x03, 0x07, 0x08, 0x00,  // '`'
    0x20, 0x54, 0x54, 0x78, 0x40,  // 'a'
    0x7F, 0x28, 0x44, 0x44, 0x38,  // 'b'
    0x38, 0x44, 0x44, 0x44, 0x28,  // 'c'
    0x38, 0x44, 0x44, 0x28, 0x7F,  // 'd'
    0x38, 0x54, 0x54, 0x54, 0x18,  // 'e'
    0x00, 0x08, 0x7E, 0x09, 0x02,  // 'f'
    0x18, 0xA4, 0xA4, 0x9C, 0x78,  // 'g'
    0x7F, 0x08, 0x04, 0x04, 0x78,  // 'h'
    0x00, 0x44, 0x7D, 0x40, 0x00,  // 'i'
    0x20, 0x40, 0x40, 0x3D, 0x00,  // 'j'
    0x7F, 0x10, 0x28, 0x44, 0x00,  // 'k'
    0x00, 0x41, 0x7F, 0x40, 0x00,  // 'l'
    0x7C, 0x04, 0x78, 0x04, 0x78,  // 'm'
    0x7C, 0x08, 0x04, 0x04, 0x78,  // 'n'
    0x38, 0x44, 0x44, 0x44, 0x38,  // 'o'
    // 7x Group
    0xFC, 0x18, 0x24, 0x24, 0x18,  // 'p'
    0x18, 0x24, 0x24, 0x18, 0xFC,  // 'q'
    0x7C, 0x08, 0x04, 0x04, 0x08,  // 'r'
    0x48, 0x54, 0x54, 0x54, 0x24,  // 's'
    0x04, 0x04, 0x3F, 0x44, 0x24,  // 't'
    0x3C, 0x40, 0x40, 0x20, 0x7C,  // 'u'
    0x1C, 0x20, 0x40, 0x20, 0x1C,  // 'v'
    0x3C, 0x40, 0x30, 0x40, 0x3C,  // 'w'
    0x44, 0x28, 0x10, 0x28, 0x44,  // 'x'
    0x4C, 0x90, 0x90, 0x90, 0x7C,  // 'y'
    0x44, 0x64, 0x54, 0x4C, 0x44,  // 'z'
    0x00, 0x08, 0x36, 0x41, 0x00,  // '{'
    0x00, 0x00, 0x77, 0x00, 0x00,  // '|'
    0x00, 0x41, 0x36, 0x08, 0x00,  // '}'
    0x02, 0x01, 0x02, 0x04, 0x02,  // '~'
#ifdef AP_NOTIFY_DISPLAY_USE_EMOJI
    0x3C, 0x26, 0x23, 0x26, 0x3C,  // 7F
    // 8x Group
    0x1E, 0xA1, 0xA1, 0x61, 0x12,  // 80
    0x3A, 0x40, 0x40, 0x20, 0x7A,  // 81
    0x38, 0x54, 0x54, 0x55, 0x59,  // 82
    0x21, 0x55, 0x55, 0x79, 0x41,  // 83
    0x22, 0x54, 0x54, 0x78, 0x42,  // 84
    0x21, 0x55, 0x54, 0x78, 0x40,  // 85
    0x20, 0x54, 0x55, 0x79, 0x40,  // 86
    0x0C, 0x1E, 0x52, 0x72, 0x12,  // 87
    0x39, 0x55, 0x55, 0x55, 0x59,  // 88
    0x39, 0x54, 0x54, 0x54, 0x59,  // 89
    0x39, 0x55, 0x54, 0x54, 0x58,  // 8A
    0x00, 0x00, 0x45, 0x7C, 0x41,  // 8B
    0x00, 0x02, 0x45, 0x7D, 0x42,  // 8C
    0x00, 0x01, 0x45, 0x7C, 0x40,  // 8D
    0x7D, 0x12, 0x11, 0x12, 0x7D,  // 8E
    0xF0, 0x28, 0x25, 0x28, 0xF0,  // 8F
    // 9x Group
    0x7C, 0x54, 0x55, 0x45, 0x00,  // 90
    0x20, 0x54, 0x54, 0x7C, 0x54,  // 91
    0x7C, 0x0A, 0x09, 0x7F, 0x49,  // 92
    0x32, 0x49, 0x49, 0x49, 0x32,  // 93
    0x3A, 0x44, 0x44, 0x44, 0x3A,  // 94
    0x32, 0x4A, 0x48, 0x48, 0x30,  // 95
    0x3A, 0x41, 0x41, 0x21, 0x7A,  // 96
    0x3A, 0x42, 0x40, 0x20, 0x78,  // 97
    0x00, 0x9D, 0xA0, 0xA0, 0x7D,  // 98
    0x3D, 0x42, 0x42, 0x42, 0x3D,  // 99
    0x3D, 0x40, 0x40, 0x40, 0x3D,  // 9A
    0x3C, 0x24, 0xFF, 0x24, 0x24,  // 9B
    0x48, 0x7E, 0x49, 0x43, 0x66,  // 9C
    0x2B, 0x2F, 0xFC, 0x2F, 0x2B,  // 9D
    0xFF, 0x09, 0x29, 0xF6, 0x20,  // 9E
    0xC0, 0x88, 0x7E, 0x09, 0x03,  // 9F
    // Ax Group
    0x20, 0x54, 0x54, 0x79, 0x41,  // A0
    0x00, 0x00, 0x44, 0x7D, 0x41,  // A1
    0x30, 0x48, 0x48, 0x4A, 0x32,  // A2
    0x38, 0x40, 0x40, 0x22, 0x7A,  // A3
    0x00, 0x7A, 0x0A, 0x0A, 0x72,  // A4
    0x7D, 0x0D, 0x19, 0x31, 0x7D,  // A5
    0x26, 0x29, 0x29, 0x2F, 0x28,  // A6
    0x26, 0x29, 0x29, 0x29, 0x26,  // A7
    0x30, 0x48, 0x4D, 0x40, 0x20,  // A8
    0x38, 0x08, 0x08, 0x08, 0x08,  // A9
    0x08, 0x08, 0x08, 0x08, 0x38,  // AA
    0x2F, 0x10, 0xC8, 0xAC, 0xBA,  // AB
    0x2F, 0x10, 0x28, 0x34, 0xFA,  // AC
    0x00, 0x00, 0x7B, 0x00, 0x00,  // AD
    0x08, 0x14, 0x2A, 0x14, 0x22,  // AE
    0x22, 0x14, 0x2A, 0x14, 0x08,  // AF
    // Bx Group
    0x55, 0x00, 0x55, 0x00, 0x55,  // B0
    0xAA, 0x55, 0xAA, 0x55, 0xAA,  // B1
    0xFF, 0x55, 0xFF, 0x55, 0xFF,  // B2
    0x00, 0x00, 0x00, 0xFF, 0x00,  // B3
    0x10, 0x10, 0x10, 0xFF, 0x00,  // B4
    0x14, 0x14, 0x14, 0xFF, 0x00,  // B5
    0x10, 0x10, 0xFF, 0x00, 0xFF,  // B6
    0x10, 0x10, 0xF0, 0x10, 0xF0,  // B7
    0x14, 0x14, 0x14, 0xFC, 0x00,  // B8
    0x14, 0x14, 0xF7, 0x00, 0xFF,  // B9
    0x00, 0x00, 0xFF, 0x00, 0xFF,  // BA
    0x14, 0x14, 0xF4, 0x04, 0xFC,  // BB
    0x14, 0x14, 0x17, 0x10, 0x1F,  // BC
    0x10, 0x10, 0x1F, 0x10, 0x1F,  // BD
    0x14, 0x14, 0x14, 0x1F, 0x00,  // BE
    0x10, 0x10, 0x10, 0xF0, 0x00,  // BF
    // Cx Group
    0x00, 0x00, 0x00, 0x1F, 0x10,  // C0
    0x10, 0x10, 0x10, 0x1F, 0x10,  // C1
    0x10, 0x10, 0x10, 0xF0, 0x10,  // C2
    0x00, 0x00, 0x00, 0xFF, 0x10,  // C3
    0x10, 0x10, 0x10, 0x10, 0x10,  // C4
    0x10, 0x10, 0x10, 0xFF, 0x10,  // C5
    0x00, 0x00, 0x00, 0xFF, 0x14,  // C6
    0x00, 0x00, 0xFF, 0x00, 0xFF,  // C7
    0x00, 0x00, 0x1F, 0x10, 0x17,  // C8
    0x00, 0x00, 0xFC, 0x04, 0xF4,  // C9
    0x14, 0x14, 0x17, 0x10, 0x17,  // CA
    0x14, 0x14, 0xF4, 0x04, 0xF4,  // CB
    0x00, 0x00, 0xFF, 0x00, 0xF7,  // CC
    0x14, 0x14, 0x14, 0x14, 0x14,  // CD
    0x14, 0x14, 0xF7, 0x00, 0xF7,  // CE
    0x14, 0x14, 0x14, 0x17, 0x14,  // CF
    // Dx Group
    0x10, 0x10, 0x1F, 0x10, 0x1F,  // D0
    0x14, 0x14, 0x14, 0xF4, 0x14,  // D1
    0x10, 0x10, 0xF0, 0x10, 0xF0,  // D2
    0x00, 0x00, 0x1F, 0x10, 0x1F,  // D3
    0x00, 0x00, 0x00, 0x1F, 0x14,  // D4
    0x00, 0x00, 0x00, 0xFC, 0x14,  // D5
    0x00, 0x00, 0xF0, 0x10, 0xF0,  // D6
    0x10, 0x10, 0xFF, 0x10, 0xFF,  // D7
    0x14, 0x14, 0x14, 0xFF, 0x14,  // D8
    0x10, 0x10, 0x10, 0x1F, 0x00,  // D9
    0x00, 0x00, 0x00, 0xF0, 0x10,  // DA
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // DB
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0,  // DC
    0xFF, 0xFF, 0xFF, 0x00, 0x00,  // DD
    0x00, 0x00, 0x00, 0xFF, 0xFF,  // DE
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  // DF
    // Ex Group
    0x38, 0x44, 0x44, 0x38, 0x44,  // E0
    0xFC, 0x4A, 0x4A, 0x4A, 0x34,  // E1
    0x7E, 0x02, 0x02, 0x06, 0x06,  // E2
    0x02, 0x7E, 0x02, 0x7E, 0x02,  // E3
    0x63, 0x55, 0x49, 0x41, 0x63,  // E4
    0x38, 0x44, 0x44, 0x3C, 0x04,  // E5
    0x40, 0x7E, 0x20, 0x1E, 0x20,  // E6
    0x06, 0x02, 0x7E, 0x02, 0x02,  // E7
    0x99, 0xA5, 0xE7, 0xA5, 0x99,  // E8
    0x1C, 0x2A, 0x49, 0x2A, 0x1C,  // E9
    0x4C, 0x72, 0x01, 0x72, 0x4C,  // EA
    0x30, 0x4A, 0x4D, 0x4D, 0x30,  // EB
    0x30, 0x48, 0x78, 0x48, 0x30,  // EC
    0xBC, 0x62, 0x5A, 0x46, 0x3D,  // ED
    0x3E, 0x49, 0x49, 0x49, 0x00,  // EE
    0x7E, 0x01, 0x01, 0x01, 0x7E,  // EF
    // Fx Group
    0x2A, 0x2A, 0x2A, 0x2A, 0x2A,  // F0
    0x44, 0x44, 0x5F, 0x44, 0x44,  // F1
    0x40, 0x51, 0x4A, 0x44, 0x40,  // F2
    0x40, 0x44, 0x4A, 0x51, 0x40,  // F3
    0x00, 0x00, 0xFF, 0x01, 0x03,  // F4
    0xE0, 0x80, 0xFF, 0x00, 0x00,  // F5
    0x08, 0x08, 0x6B, 0x6B, 0x08,  // F6
    0x36, 0x12, 0x36, 0x24, 0x36,  // F7
    0x06, 0x0F, 0x09, 0x0F, 0x06,  // F8
    0x00, 0x00, 0x18, 0x18, 0x00,  // F9
    0x00, 0x00, 0x10, 0x10, 0x00,  // FA
    0x30, 0x40, 0xFF, 0x01, 0x01,  // FB
    0x00, 0x1F, 0x01, 0x01, 0x1E,  // FC
    0x00, 0x19, 0x1D, 0x17, 0x12,  // FD
    0x00, 0x3C, 0x3C, 0x3C, 0x3C,  // FE
    0x00, 0x00, 0x00, 0x00, 0x00   // FF
#endif
};

#ifdef AP_NOTIFY_DISPLAY_USE_EMOJI
static_assert(ARRAY_SIZE(_font) == 1280, "_font is correct size");
#else
static_assert(ARRAY_SIZE(_font) == 475, "_font is correct size");
#endif

bool Display::init(void)
{
    // exit immediately if already initialised
    if (_driver != nullptr) {
        return true;
    }

    // initialise driver
    FOREACH_I2C(i) {
        switch (pNotify->_display_type) {
        case DISPLAY_SSD1306: {
            _driver = Display_SSD1306_I2C::probe(std::move(hal.i2c_mgr->get_device(i, NOTIFY_DISPLAY_I2C_ADDR)));
            break;
        }
        case DISPLAY_SH1106: {
            _driver = Display_SH1106_I2C::probe(std::move(hal.i2c_mgr->get_device(i, NOTIFY_DISPLAY_I2C_ADDR)));
            break;
        }
        case DISPLAY_SITL: {
#ifdef WITH_SITL_OSD
            _driver = Display_SITL::probe(); // never fails
#elif CONFIG_HAL_BOARD == HAL_BOARD_SITL
            ::fprintf(stderr, "SITL Display ineffective without --osd\n");
#endif
            break;
        }
        case DISPLAY_OFF:
        default:
            break;
        }
        if (_driver != nullptr) {
            break;
        }
    }

    if (_driver == nullptr) {
        return false;
    }

    // update all on display
    update_all();
    _driver->hw_update();

    return true;
}

void Display::update()
{
    // max update frequency 2Hz
    static uint8_t timer = 0;
    if (timer++ < 25) {
        return;
    }
    timer = 0;

    if (AP_Notify::flags.armed) {
        if (_screenpage != 1) {
            _driver->clear_screen();
            update_arm(3);
            _screenpage = 1;
            _driver->hw_update(); //update hw once , do not transmition to display in fly
        }
        return;
    }

    if (_screenpage != 2) {
        _driver->clear_screen(); //once clear screen when page changed
        _screenpage = 2;
    }

    update_all();
    _driver->hw_update(); //update at 2 Hz in disarmed mode

}

void Display::update_all()
{
    update_text(0);
    update_mode(1);
    update_battery(2);
    update_gps(3);
    //update_gps_sats(4);
    update_prearm(4);
    update_ekf(5);
}

void Display::draw_text(uint16_t x, uint16_t y, const char* c)
{
    if (nullptr == c) {
        return;
    }
    while (*c != 0) {
#ifndef AP_NOTIFY_DISPLAY_USE_EMOJI
        if (*c >= ' ' && *c <= '~') {
            draw_char(x, y, *c - ' ');
        } else {
            // convert oob characters to spaces
            draw_char(x, y, 0);
        }
#else
        draw_char(x, y, *c);
#endif
        x += 7;
        c++;
    }
}

void Display::draw_char(uint16_t x, uint16_t y, const char c)
{
    uint8_t line;

    // draw char to pixel
    for (uint8_t i = 0; i < 6; i++) {
        if (i == 5) {
            line = 0;
        } else {
            line = _font[(c * 5) + i];
        }

        for (uint8_t j = 0; j < 8; j++) {
            if (line & 1) {
                _driver->set_pixel(x + i, y + j);
            } else {
                _driver->clear_pixel(x + i, y + j);
            }
            line >>= 1;
        }
    }
}

void Display::update_arm(uint8_t r)
{
    if (AP_Notify::flags.armed) {
        draw_text(COLUMN(0), ROW(r), ">>>>> ARMED! <<<<<");
    } else {
        draw_text(COLUMN(0), ROW(r), "     disarmed     ");
    }
}

void Display::update_prearm(uint8_t r)
{
    if (AP_Notify::flags.pre_arm_check) {
        draw_text(COLUMN(0), ROW(r), "Prearm: passed    ");
    } else {
        draw_text(COLUMN(0), ROW(r), "Prearm: failed    ");
    }
}

void Display::update_gps(uint8_t r)
{
    static const char * gpsfixname[] = {"Other", "NoGPS","NoFix","2D","3D","DGPS", "RTK f", "RTK F"};
    char msg [DISPLAY_MESSAGE_SIZE];
    const char * fixname;
    switch  (AP_Notify::flags.gps_status) {
        case AP_GPS::NO_GPS:
            fixname = gpsfixname[1];
            break;
        case AP_GPS::NO_FIX:
            fixname = gpsfixname[2];
            break;
        case AP_GPS::GPS_OK_FIX_2D:
            fixname = gpsfixname[3];
            break;
        case AP_GPS::GPS_OK_FIX_3D:
            fixname = gpsfixname[4];
            break;
        case AP_GPS::GPS_OK_FIX_3D_DGPS:
            fixname = gpsfixname[5];
            break;
        case AP_GPS::GPS_OK_FIX_3D_RTK_FLOAT:
            fixname = gpsfixname[6];
            break;
        case AP_GPS::GPS_OK_FIX_3D_RTK_FIXED:
            fixname = gpsfixname[7];
            break;
        default:
            fixname = gpsfixname[0];
            break;
    }
    snprintf(msg, DISPLAY_MESSAGE_SIZE, "GPS:%-5s Sats:%2u", fixname, (unsigned)AP_Notify::flags.gps_num_sats) ;
    draw_text(COLUMN(0), ROW(r), msg);
}

void Display::update_gps_sats(uint8_t r)
{
    draw_text(COLUMN(0), ROW(r), "Sats:");
    draw_char(COLUMN(8), ROW(r), (AP_Notify::flags.gps_num_sats / 10) + '0');
    draw_char(COLUMN(9), ROW(r), (AP_Notify::flags.gps_num_sats % 10) + '0');
}

void Display::update_ekf(uint8_t r)
{
    if (AP_Notify::flags.ekf_bad) {
        draw_text(COLUMN(0), ROW(r), "EKF:    fail");
    } else {
        draw_text(COLUMN(0), ROW(r), "EKF:    ok  ");
    }
}

void Display::update_battery(uint8_t r)
{
    char msg [DISPLAY_MESSAGE_SIZE];
    AP_BattMonitor &battery = AP::battery();
    uint8_t pct = battery.capacity_remaining_pct();
    snprintf(msg, DISPLAY_MESSAGE_SIZE, "BAT:%4.2fV %2d%% ", (double)battery.voltage(), pct) ;
    draw_text(COLUMN(0), ROW(r), msg);
 }

void Display::update_mode(uint8_t r)
{
    char msg [DISPLAY_MESSAGE_SIZE];
    if (pNotify->get_flight_mode_str()) {
        snprintf(msg, DISPLAY_MESSAGE_SIZE, "Mode: %s", pNotify->get_flight_mode_str()) ;
        draw_text(COLUMN(0), ROW(r), msg);
    }
}

void Display::update_text_empty(uint8_t r)
{
    char msg [DISPLAY_MESSAGE_SIZE] = {};
    memset(msg, ' ', sizeof(msg)-1);
    _movedelay = 0;
    _mstartpos = 0;
    draw_text(COLUMN(0), ROW(r), msg);
}

void Display::update_text(uint8_t r)
{
    char msg [DISPLAY_MESSAGE_SIZE] = {};
    char txt [NOTIFY_TEXT_BUFFER_SIZE] = {};

    const bool text_is_valid = AP_HAL::millis() - pNotify->_send_text_updated_millis < _send_text_valid_millis;
    if (!text_is_valid) {
        update_text_empty(r);
        return;
    }

    if (_movedelay > 0) {
        _movedelay--;
        return;
    }

    snprintf(txt, sizeof(txt), "%s", pNotify->get_text());

    memset(msg, ' ', sizeof(msg)-1); // leave null termination
    const uint8_t len = strlen(&txt[_mstartpos]);
    const uint8_t to_copy = (len < sizeof(msg)-1) ? len : (sizeof(msg)-1);
    memcpy(msg, &txt[_mstartpos], to_copy);

    if (len <= sizeof(msg)-1) {
        // end-of-message reached; pause scrolling a while
        _movedelay = 4;
        // reset startpos so we start scrolling from the start again:
        _mstartpos = 0;
    } else {
        _mstartpos++;
    }

    draw_text(COLUMN(0), ROW(0), msg);
 }
