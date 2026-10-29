#include <string.h>
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "rgb.h"

/*
Function to be executed by the Core 2

Assembler code
(built for TCMA of the Core 2, no startup code):


.set PIN, 7 // PC7

li t2, 0x4000A000 // t2 = PIO
lw t0, 8(t2) // t0 = PIO_EN
ori t0, t0, (1 << PIN)
sw t0, 8(t2) // PCx = output
lw t0, 0x0C(t2) // t0 = PIO_OU
andi t0, t0, ~(1 << PIN)
sw t0, 0x0C(t2) // PCx = 0

wait_mb0:
lw t0, 0x404(t2) // t0 = MB0_FULL0
andi t0, t0, 1
beq t0, zero, wait_mb0

lw t1, 0x400(t2) // t1 = MB0_DATA0
li t0, 0x40008000 // t0 = TCMB
add t1, t1, t0 // t1 = EOF

lw a5, 0x0C(t2) // a5 = PIO_OU
ori a5, a5, (1 << PIN) // mask for PCx = 1
xori a4, a5, (1 << PIN) // mask for PCx = 0

read_byte:
lbu t3, 0(t0) // t3 = data
li t4, (1 << 7) // t4 = bit mask

write_bit:
sw a5, 0x0C(t2) // PCx = 1
and t5, t3, t4
bne t5, zero, bit1

nop
nop
nop
nop
nop
nop
nop

sw a4, 0x0C(t2) // PCx = 0

nop
nop
nop
nop
nop
nop
nop
nop

j bit_epi

bit1:
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop

sw a4, 0x0C(t2) // PCx = 0

bit_epi:
nop
srli t4, t4, 1
beq t4, zero, next_byte

nop
nop
nop
nop
j write_bit

next_byte:
addi t0, t0, 1
bltu t0, t1, read_byte

j wait_mb0


This precompiled binary code will be copied to the TCMA memory region of the Core 2
*/
static const uint8_t core2_code[] = {
    0xb7, 0xa3, 0x00, 0x40,
    0x83, 0xa2, 0x83, 0x00,
    0x93, 0xe2, 0x02, 0x08,
    0x23, 0xa4, 0x53, 0x00,
    0x83, 0xa2, 0xc3, 0x00,
    0x93, 0xf2, 0xf2, 0xf7,
    0x23, 0xa6, 0x53, 0x00,
    0x83, 0xa2, 0x43, 0x40,
    0x93, 0xf2, 0x12, 0x00,
    0xe3, 0x8c, 0x02, 0xfe,
    0x03, 0xa3, 0x03, 0x40,
    0xb7, 0x82, 0x00, 0x40,
    0x16, 0x93,
    0x83, 0xa7, 0xc3, 0x00,
    0x93, 0xe7, 0x07, 0x08,
    0x13, 0xc7, 0x07, 0x08,
    0x03, 0xce, 0x02, 0x00,
    0x93, 0x0e, 0x00, 0x08,
    0x23, 0xa6, 0xf3, 0x00,
    0x33, 0x7f, 0xde, 0x01,
    0x63, 0x14, 0x0f, 0x02,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x23, 0xa6, 0xe3, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x1d, 0xa0,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x23, 0xa6, 0xe3, 0x00,
    0x01, 0x00,
    0x93, 0xde, 0x1e, 0x00,
    0x63, 0x87, 0x0e, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x01, 0x00,
    0x69, 0xbf,
    0x85, 0x02,
    0xe3, 0xe7, 0x62, 0xf8,
    0xa5, 0xb7
};

void RGB_Set(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < RGB_WIDTH * RGB_HEIGHT) {
        index *= 3;
#ifdef RGB_GRB
        *(__IO uint8_t*)(CORE2_TCMB_SYS_BASE + index++) = g;
        *(__IO uint8_t*)(CORE2_TCMB_SYS_BASE + index++) = r;
#else
        *(__IO uint8_t*)(CORE2_TCMB_SYS_BASE + index++) = r;
        *(__IO uint8_t*)(CORE2_TCMB_SYS_BASE + index++) = g;
#endif
        *(__IO uint8_t*)(CORE2_TCMB_SYS_BASE + index) = b;
    }
}

void RGB_Fill(uint16_t from, uint16_t count, uint8_t r, uint8_t g, uint8_t b) {
    while ((from < RGB_WIDTH * RGB_HEIGHT) && count--) {
        RGB_Set(from++, r, g, b);
    }
}

inline __attribute__((always_inline)) void RGB_Clear(void) {
//    RGB_Fill(0, RGB_WIDTH * RGB_HEIGHT, 0, 0, 0);
    memset((uint8_t*)CORE2_TCMB_SYS_BASE, 0, RGB_WIDTH * RGB_HEIGHT * 3);
}

void RGB_Init(void) {
    // Initialize the C7 pin as PIO
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_7, CRU_PIN_AF_1);

    // Reset the Core 2
    CRU_C2_ForceResetAll();

    // Releases Core 2 processor complex reset
    CRU_C2_ReleaseResetCx();
    // Releases Core 2 TCM arbitrator (Front Port) reset
    CRU_C2_ReleaseResetFP();

    // Copy Core 2 binary code to be executed to the Core 2's TCM memory area
    for (size_t i = 0U; i < sizeof(core2_code); ++i) {
        *(__IO uint8_t*)(CORE2_TCMA_SYS_BASE + i) = core2_code[i];
    }

    RGB_Clear();

    // Start the Core 2 code execution from the TCMA memory region
    CRU_C2_Start(CORE2_TCMA_BASE);
}

void RGB_Update(void) {
    MB0->DATA0 = RGB_WIDTH * RGB_HEIGHT * 3;
}

void RGB_Scroll(int16_t delta) {
    if (((delta > 0) && (delta < RGB_WIDTH * RGB_HEIGHT)) || ((delta < 0) && (delta > -(RGB_WIDTH * RGB_HEIGHT)))) {
        if (delta > 0)
            memmove((uint8_t*)(CORE2_TCMB_SYS_BASE + delta * 3), (uint8_t*)CORE2_TCMB_SYS_BASE, (RGB_WIDTH * RGB_HEIGHT - delta) * 3);
        else
            memmove((uint8_t*)CORE2_TCMB_SYS_BASE, (uint8_t*)(CORE2_TCMB_SYS_BASE - delta * 3), (RGB_WIDTH * RGB_HEIGHT + delta) * 3);
    }
}

#if RGB_HEIGHT > 1
uint16_t RGB_MatrixXY(uint8_t x, uint8_t y) {
    uint16_t result;

    result = x * RGB_HEIGHT;
    if (x & 1)
        result += ((RGB_HEIGHT - 1) - y);
    else
        result += y;
    return result;
}
#endif
