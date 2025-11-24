#ifndef __MY_LCD_I2C_H__
#define __MY_LCD_I2C_H__

#include "stm32f4xx_hal.h"

// LCD 주소 (0x27 또는 0x3F 중 하나일 수 있음)
#define LCD_I2C_ADDR (0x27 << 1)  // 0x27 또는 0x3F 사용 가능
//lcd dot width, height

//LCD의 한 칸의 픽셀수
#define LCD_DOT_WIDTH 5
#define LCD_DOT_HEIGHT 8

//LCD의 가로 세로 칸 갯수
#define LCD_CNT_WIDTH 16
#define LCD_CNT_HEIGHT 2

#define MAP_HEIGHT LCD_DOT_HEIGHT * LCD_CNT_HEIGHT
#define MAP_WIDTH LCD_DOT_WIDTH * LCD_CNT_WIDTH

//CGRAM_MAX
#define CGRAM_MAX 8

//LCD MAP

//LCD의 전체 픽셀
extern uint8_t PIXEL_MAP[LCD_CNT_HEIGHT][LCD_DOT_WIDTH * LCD_CNT_WIDTH];

//LCD의 각 칸
extern uint8_t LCD_MAP[LCD_CNT_HEIGHT][LCD_CNT_WIDTH][LCD_DOT_HEIGHT];

//각 칸의 데이터
extern uint8_t LCD_INDEX[LCD_CNT_HEIGHT][LCD_CNT_WIDTH * LCD_CNT_HEIGHT];

//LCD 각 칸의 커스텀 램 사용history
extern uint8_t LCD_INDEX_HISTORY[LCD_CNT_HEIGHT][LCD_CNT_WIDTH / 8];

extern uint8_t max_x, max_y;


void lcd_init(I2C_HandleTypeDef *hi2c);
void lcd_send_data(uint8_t data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_create_char(uint8_t location, uint8_t charmap[]);
void lcd_print_custom();
void lcd_set_dot(int y, int x);
void lcd_clear_dot(int y, int x);
void lcd_clear_custom();


#endif // __MY_LCD_I2C_H__