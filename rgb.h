#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define RGB_GRB

#define RGB_WIDTH   32 // 12
#define RGB_HEIGHT  8 // 1

void RGB_Init(void);
void RGB_Set(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void RGB_Fill(uint16_t from, uint16_t count, uint8_t r, uint8_t g, uint8_t b);
void RGB_Clear(void);
void RGB_Scroll(int16_t offset);
void RGB_Update(void);
#if RGB_HEIGHT > 1
uint16_t RGB_MatrixXY(uint8_t x, uint8_t y);
#endif

#ifdef __cplusplus
}
#endif
