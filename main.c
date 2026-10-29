#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_eflash.h"
#include "rgb.h"

#define MAX_BRIGHT      15
#define RGB_BRIGHT(c)   (((c) * MAX_BRIGHT) / 255)

#define SCROLL_DURATION 100
#define SCROLL_PAUSE    500

static const uint8_t TITLE[] = {
    0x3E, 0x41, 0x41, 0x22,  0x00, 0x00, 0x00,  0x7F, 0x08, 0x08, 0x7F,  0x00,  0x38, 0x44, 0x44, 0x38,
    0x00,  0x7C, 0x54, 0x54, 0x28,  0x00,  0x7C, 0x50, 0x20, 0x00, 0x7C,  0x00,  0x7C, 0x08, 0x10, 0x08,
    0x7C,  0x00, 0x00, 0x00,  0x7F, 0x01, 0x01, 0x01,  0x00,  0x38, 0x44, 0x44, 0x38,  0x00,  0x30, 0x4A,
    0x4A, 0x3C,  0x00,  0x38, 0x44, 0x44, 0x38,  0x00,  0x7C, 0x08, 0x10, 0x08, 0x7C,  0x00, 0x00,  0x5F,
    0x00, 0x00,  0x5F,  0x00, 0x00,  0x5F
};
/*
static const uint8_t TITLE[] = {
    0x3E, 0x41, 0x41, 0x22,  0x00, 0x00, 0x00,  0x30, 0x4A, 0x4A, 0x3C,  0x00,  0x7C, 0x10, 0x10, 0x7C,
    0x00,  0x39, 0x54, 0x54, 0x19,  0x00,  0x7C, 0x08, 0x10, 0x08, 0x7C,  0x00, 0x00, 0x00,  0xFC, 0x24,
    0x24, 0x18,  0x00,  0x38, 0x44, 0x44, 0x38,  0x00,  0x6C, 0x10, 0x7C, 0x10, 0x6C,  0x00,  0x30, 0x4A,
    0x4A, 0x3C,  0x00,  0x38, 0x54, 0x54, 0x18,  0x00,  0x7C, 0x10, 0x10, 0x7C,  0x00,  0x3C, 0x40, 0x20,
    0x7C,  0x00,  0x48, 0x34, 0x14, 0x7C,  0x00, 0x00,  0x5F,  0x00, 0x00,  0x5F,  0x00, 0x00,  0x5F
};
*/

static void draw_title(uint8_t offset, uint8_t r, uint8_t g, uint8_t b) {
    RGB_Clear();
    for (uint8_t x = 0; x < RGB_WIDTH; ++x) {
        uint8_t row = TITLE[offset];

        for (uint8_t y = 0; y < 8; ++y) {
            if (row & (1 << y))
                RGB_Set(RGB_MatrixXY(x, y), r, g, b);
        }
        if (++offset >= sizeof(TITLE) / sizeof(TITLE[0]))
            break;
    }
}

static void CLK_Init(void) {
    /*
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 100MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 25MHz
     */

    CRU_PLL_BypassMode_Disable();

    /* Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /* Configure PLL */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /* PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /* Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /* Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /* Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_8);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(24UL);
}

/** @brief  Function to be executed by the Core 0 */
int main(void) {
    EFLASH_Init(100000000U);             /* 100 MHz. See PLL configuration */

    CLK_Init();

    RGB_Init();

    uint8_t color = 0, step = 0;

    while (1) {
        if (color == 0)
            draw_title(step, RGB_BRIGHT(255), 0, 0); // RED
        else if (color == 1)
            draw_title(step, 0, RGB_BRIGHT(255), 0); // GREEN
        else if (color == 2)
            draw_title(step, 0, 0, RGB_BRIGHT(255)); // BLUE
        else // if (color == 3)
            draw_title(step, RGB_BRIGHT(85), RGB_BRIGHT(85), RGB_BRIGHT(85)); // WHITE
        RGB_Update();
        __delay_ms((step > 0) && (step <= sizeof(TITLE) / sizeof(TITLE[0]) - RGB_WIDTH - 1) ? SCROLL_DURATION : SCROLL_PAUSE);
        if (++step > sizeof(TITLE) / sizeof(TITLE[0]) - RGB_WIDTH) {
            step = 0;
            if (++color > 3)
                color = 0;
        }
    }

    return 0;
}
