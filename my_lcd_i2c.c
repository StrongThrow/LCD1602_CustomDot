#include "my_lcd_i2c.h"
#include "string.h"
#include "stm32f4xx_hal.h"

#define min(x, y) ((x) < (y) ? (x):(y))
#define max(x, y) ((x) > (y) ? (x):(y))

static I2C_HandleTypeDef *lcd_i2c = NULL;

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_RW 0x00
#define LCD_RS 0x01

void lcd_send_raw_cmd(uint8_t cmd);
void lcd_send_internal(uint8_t data, uint8_t flags);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_byte(uint8_t data);

void lcd_init(I2C_HandleTypeDef *hi2c) {
	lcd_i2c = hi2c;
	
	//HAL_Delay(50);
	HAL_Delay(100);
	//lcd_send_cmd(0x30);
	////HAL_Delay(5);
	//HAL_Delay(50);
	//lcd_send_cmd(0x30);
	////HAL_Delay(1);
	//HAL_Delay(2);
	//lcd_send_cmd(0x30);
	//HAL_Delay(10);
	for (int i = 0; i < 3; i++)
	{
		lcd_send_raw_cmd(0x30);
		HAL_Delay(i == 0 ? 5 : 1);
	}
	lcd_send_raw_cmd(0x20); // 4-bit mode
	HAL_Delay(1);

	lcd_send_cmd(0x28); // Function set: 4-bit, 2-line, 5x8 dots
	lcd_send_cmd(0x08); // Display off
	lcd_send_cmd(0x01); // Clear display
	HAL_Delay(2);
	lcd_send_cmd(0x06); // Entry mode set: increment, no shift
	lcd_send_cmd(0x0C); // Display on, cursor off, blink off
}

void lcd_send_cmd(uint8_t cmd) {
	lcd_send_internal(cmd, 0);
}

void lcd_send_data(uint8_t data) {
	lcd_send_internal(data, LCD_RS);
}

void lcd_send_raw_cmd(uint8_t cmd) {
	uint8_t high_nibble = cmd & 0xF0;
	uint8_t buf[2];

	buf[0] = high_nibble | LCD_BACKLIGHT | LCD_ENABLE;
	buf[1] = high_nibble | LCD_BACKLIGHT;

	HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR, buf, 2, HAL_MAX_DELAY);
}

void lcd_send_internal(uint8_t data, uint8_t flags) {
	uint8_t high_nibble = data & 0xF0;
	uint8_t low_nibble = (data << 4) & 0xF0;
	uint8_t buf[4];

	buf[0] = high_nibble | flags | LCD_BACKLIGHT | LCD_ENABLE;
	buf[1] = high_nibble | flags | LCD_BACKLIGHT;
	buf[2] = low_nibble | flags | LCD_BACKLIGHT | LCD_ENABLE;
	buf[3] = low_nibble | flags | LCD_BACKLIGHT;

	HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR, buf, 4, HAL_MAX_DELAY);
}

void lcd_send_string(char *str) {
	while (*str) {
		lcd_send_data((uint8_t)(*str));
		str++;
	}
}

void lcd_clear(void) {
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};	// 2??
	lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_create_char(uint8_t location, uint8_t charmap[])
{
	location &= 0x07; // 0~7 ??? ??
	lcd_send_cmd(0x40 | (location << 3)); // CGRAM ?? ??


	for (int i = 0; i < 8; i++) {
			lcd_send_data(charmap[i]);
	}

}

//LCD CustomDot 구현

//전체 픽셀
uint8_t PIXEL_MAP[LCD_CNT_HEIGHT][LCD_DOT_WIDTH * LCD_CNT_WIDTH] = {0};
//2 x 80

uint8_t LCD_INDEX[LCD_CNT_HEIGHT][LCD_CNT_WIDTH * LCD_CNT_HEIGHT] = {0};

uint8_t LCD_INDEX_HISTORY[LCD_CNT_HEIGHT][LCD_CNT_WIDTH / 8] = {0};

uint8_t max_x, max_y = 0;

void lcd_set_dot(int y, int x){
	
	if((x >= LCD_CNT_WIDTH * LCD_DOT_WIDTH) || (x < 0) || (y >= LCD_DOT_HEIGHT * LCD_CNT_HEIGHT) || (y < 0)) return;
	
	if(y < LCD_DOT_HEIGHT){
		PIXEL_MAP[0][x] |= (1 << y);
	}
	else if(y >= LCD_DOT_HEIGHT){
		PIXEL_MAP[1][x] |= (1 << (y - 8));
	}
	
	max_x = max(x, max_x);
	max_y = max(y, max_y);
	
}


void lcd_clear_dot(int y, int x){
	
	if((x >= LCD_CNT_WIDTH * LCD_DOT_WIDTH) || (x < 0) || (y >= LCD_DOT_HEIGHT * LCD_CNT_HEIGHT) || (y < 0)) return;
	
	if(y < LCD_DOT_HEIGHT){
		PIXEL_MAP[0][x] &= ~(1 << y);
	}
	else if(y >= LCD_DOT_HEIGHT){
		PIXEL_MAP[1][x] &= ~(1 << (y - 8));
	}
}

void lcd_print_custom(){
	//LCD 클리어
	
	//LCD 검사
	for(int y = 0; y < LCD_CNT_HEIGHT; y++){
		for(int x = 0; x < LCD_CNT_WIDTH; x++){
			
			uint8_t idx = x / 8;
			uint8_t bit = x % 8;
			
			//해당 칸이 사용되었었다면
			if(((LCD_INDEX_HISTORY[y][idx] >> bit) & 0x01) == 0x01){
				//해당 좌표에 공백 문자 전송
				lcd_set_cursor(y, x);
				lcd_send_data(0x20);
				//다시 사용이 안되었음을 알림
				LCD_INDEX_HISTORY[y][idx] &= ~(1 << bit);
			}
		}
	}
	
	
	//1. 찍은 도트를 검사 왼쪽 상단부터 오른쪽 하단 순서 
	
	uint8_t CGRAM_CNT = 0; 
	
	//LCD_INDEX 배열 초기화
	memset(LCD_INDEX, 0, sizeof(LCD_INDEX));
		
	//LCD의 1번째 줄부터 시작
	for(int y = 0; y < LCD_CNT_HEIGHT; y++){
		
		if(CGRAM_CNT >= 8) break;
		
		//LCD의 1번째 칸부터 시작 
		for(int khan = 0; khan <  LCD_CNT_WIDTH; khan++){
			
			if(CGRAM_CNT >= 8) break;
			
			//LCD의 x축 첫 번째 도트부터 시작
			for(int x = khan * LCD_DOT_WIDTH; x < khan * LCD_DOT_WIDTH + LCD_DOT_WIDTH; x++){
				
				if(CGRAM_CNT >= 8) break;
				
				//각 칸에서 도트가 검출되면
				if(PIXEL_MAP[y][x] > 0){
					
					//CGRAM 개수가 남아 있으면
					if(CGRAM_CNT < 8){
						
						//LCD_INDEX에 해당 칸 저장
						LCD_INDEX[y][khan] = CGRAM_CNT;
						LCD_INDEX[y][khan] |= 0x80;
						
						//해당 칸이 사용됐음을 저장
						
						uint8_t idx = khan / 8; 
						uint8_t bit = khan % 8;
						
						LCD_INDEX_HISTORY[y][idx] |= (1 << bit);
						CGRAM_CNT++;
						//하나라도 검출되면 break하고 다음 칸으로 이동
						break;
					}
					else break;

				}
				
			}
						 
		}
		
	}
	
	//2. 이전 픽셀 지우기, 새로운 곳에 픽셀 셋
	
	for(int y = 0; y < LCD_CNT_HEIGHT; y++){//0 ~ 1
		
		for(int khan = 0; khan < LCD_CNT_WIDTH; khan++){//0 ~ 15
			
			if((LCD_INDEX[y][khan] & 0x80) == 0x80){// available Custom Char
				
				LCD_INDEX[y][khan] &= ~(0x80);
				
				//reset data buffer
				uint8_t data[8] = {0};

				for(int i = 0; i < 5; i++){
					data[0] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 0) & 0x01) << (4-i));
					data[1] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 1) & 0x01) << (4-i));
					data[2] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 2) & 0x01) << (4-i));
					data[3] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 3) & 0x01) << (4-i));
					data[4] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 4) & 0x01) << (4-i));
					data[5] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 5) & 0x01) << (4-i));
					data[6] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 6) & 0x01) << (4-i));
					data[7] |= (((PIXEL_MAP[y][(khan * LCD_DOT_WIDTH) + i] >> 7) & 0x01) << (4-i));
				}
				
				
				lcd_create_char(LCD_INDEX[y][khan], data);
				lcd_set_cursor(y,khan);
				lcd_send_data(LCD_INDEX[y][khan]);
			}
			
		}
	}
	
	//PIXEL_MAP 배열 초기화
	//memset(PIXEL_MAP, 0, sizeof(PIXEL_MAP));
	
}

void lcd_clear_custom(){
	//memset(PIXEL_MAP, 0, sizeof(PIXEL_MAP));
	
	uint8_t y_max_index = min(LCD_CNT_HEIGHT - 1, max_y / LCD_DOT_HEIGHT);
	
	for(int y = 0; y <= y_max_index; y++){
		for(int x = 0; x <= max_x; x++){
			PIXEL_MAP[y][x] = 0;
		}
	}
	max_x =max_y = 0;
	
}

