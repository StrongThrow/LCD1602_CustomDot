# 🎨 STM32 I2C LCD Custom Graphics Library

<div align="center">
  <img src="https://img.shields.io/badge/STM32-F411-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white">
  <img src="https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white">
  <img src="https://img.shields.io/badge/Library-HAL-FCC624?style=for-the-badge&logo=stm32&logoColor=black">
</div>

<br>

**1602 Character LCD(I2C)를 그래픽 LCD처럼 제어할 수 있게 해주는 라이브러리입니다.**

일반적인 텍스트 LCD는 고정된 문자만 출력할 수 있지만, 이 라이브러리는 **CGRAM(Custom Character RAM) 동적 할당 알고리즘**을 사용하여 텍스트 LCD 위에서 점(Dot)을 찍거나, 선을 긋고, 간단한 애니메이션을 구현할 수 있습니다.

---

## 📺 Demo Result

![LCD Execution Result](./assets/lcd_demo.gif)
*STM32F411 + 1602 LCD 구동 예시*

---

## ✨ Key Features

* **Pixel-Level Control:** `lcd_set_dot(y, x)` 함수로 픽셀 단위 제어 가능
* **Dynamic CGRAM:** 화면 패턴을 분석하여 커스텀 문자 자동 생성 및 배치
* **Smart Rendering:** 변경된 픽셀만 갱신하여 I2C 통신 부하 최소화
* **Compatibility:** 기존 `lcd_send_string` 등 텍스트 출력 기능 유지

---

## 🛠️ Requirements

* **MCU:** STM32F411 (Tested)
* **Display:** 1602 LCD (I2C Backpack)
* **IDE:** STM32CubeIDE
* **Driver:** STM32 HAL Driver

---

## 🚀 How to Use

### 1. Setup
프로젝트의 `Core/Src`에 `my_lcd_i2c.c`를, `Core/Inc`에 `my_lcd_i2c.h`를 복사합니다.

### 2. Main Code
`main.c` 파일 작성 예시:

```c
/* USER CODE BEGIN Includes */
#include "my_lcd_i2c.h"
/* USER CODE END Includes */

int main(void)
{
  HAL_Init();
  /* (System Clock, GPIO, I2C1 Init...) */
  MX_I2C1_Init(); 

  /* USER CODE BEGIN 2 */
  // LCD 초기화 (필수)
  lcd_init(&hi2c1);
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
    for(int i = 0; i < 68; i++){
        // 1. 버퍼에 점 찍기
        for(int x = 0; x < 12; x++){
            for(int y = 6; y < 10; y++){
                lcd_set_dot(y, i + x);
            }
        }
        
        // 2. 화면 출력 (렌더링)
        lcd_print_custom();
        
        HAL_Delay(100);
        
        // 3. 버퍼 초기화 (다음 프레임 준비)
        lcd_clear_custom();
    }
  }
}

Function Name,Parameters,Description,Note
lcd_init,I2C_HandleTypeDef *hi2c,LCD 및 I2C 연결을 초기화합니다.,main 문 초기에 1회 호출 필수
lcd_set_dot,"int y, int x","가상 좌표계(y, x)에 픽셀을 찍습니다.",Buffer에만 기록됨 (출력 X)
lcd_clear_dot,"int y, int x",특정 좌표의 픽셀을 지웁니다.,Buffer에만 기록됨 (출력 X)
lcd_print_custom,void,현재 Buffer 상태를 분석하여 LCD에 실제 출력합니다.,렌더링 함수
lcd_clear_custom,void,내부 그래픽 Buffer(PIXEL_MAP)를 0으로 초기화합니다.,다음 프레임 그리기 전 호출 권장
lcd_send_string,char *str,일반 텍스트 문자열을 출력합니다.,기본 기능
lcd_set_cursor,"uint8_t row, uint8_t col",커서 위치를 이동합니다.,기본 기능

⚠️ Technical Limitations
이 라이브러리는 하드웨어의 특성을 활용하므로 다음과 같은 물리적 제약이 있습니다.

1. CGRAM 8개 제한 (The 8-Character Limit)
1602 LCD는 사용자 정의 문자를 최대 8개까지만 저장 가능합니다.

따라서 화면 전체를 꽉 채우는 그림은 그릴 수 없습니다.

동시에 표현되는 '픽셀이 있는 문자 칸'이 8개를 넘으면 렌더링되지 않습니다.

2. 해상도 (Resolution)
전체 크기: 80px (가로) × 16px (세로)

문자 칸(5x8) 사이에는 물리적인 간격이 있어 픽셀이 끊겨 보일 수 있습니다.

<div align="center"> Author: <a href="https://github.com/StrongThrow">SongGu Kang</a> </div>
