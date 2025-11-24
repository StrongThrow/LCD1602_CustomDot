# 🎨 STM32 I2C LCD Custom Graphics Library

<div align="center">
  <img src="[https://img.shields.io/badge/STM32-F411-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white](https://img.shields.io/badge/STM32-F411-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)">
  <img src="[https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white](https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)">
  <img src="[https://img.shields.io/badge/Library-HAL-FCC624?style=for-the-badge&logo=stm32&logoColor=black](https://img.shields.io/badge/Library-HAL-FCC624?style=for-the-badge&logo=stm32&logoColor=black)">
</div>

<br>

**LCD1602에 글자만 띄우지 말고 그래픽 디스플레이로 변신시킬 수 있습니다.**

이 라이브러리는 **CGRAM 동적 할당 알고리즘**(Dynamic CGRAM Allocation Algorithm)을 구현하여, LCD1602(I2C)에서 픽셀 단위의 제어를 가능하게 합니다. 텍스트 전용으로 설계된 디스플레이 위에서 점을 찍거나, 선을 긋고, 간단한 애니메이션을 그릴 수 있습니다.

---

## 📺 Demo Result

![Image](https://github.com/user-attachments/assets/0eb6eda9-ed84-435e-ac18-b746cefccf8a)

*STM32F411 + 1602 LCD 구동 예시*

---

## ⚙️ 동작 원리 (Architecture)

이 라이브러리는 픽셀 좌표계를 사용하여 글자 표시만 하는 LCD1602에 임의의 그림을 그려줍니다.

1.  **가상 버퍼 (`PIXEL_MAP`)**
    * 라이브러리는 LCD 화면 전체를 픽셀로 나타내는 80x16 비트 메모리 버퍼를 내부적으로 유지 관리합니다.
    * `lcd_set_dot(y, x)`를 호출하면 실제 LCD가 아닌 이 가상 버퍼가 먼저 업데이트됩니다.

2.  **동적 분할 (Dynamic Segmentation)**
    * 1602 LCD는 5x8 픽셀 크기의 32개 블록(16열 x 2행)으로 구성되어 있습니다.
    * 라이브러리는 가상 버퍼를 스캔하여 현재 픽셀이 그려져야 하는 블록이 어디인지 식별합니다.

3.  **CGRAM 할당 (CGRAM Allocation)**
    * HD44780 컨트롤러는 **최대 8개의 사용자 정의 문자(CGRAM)**만 지원합니다.
    * 라이브러리는 활성화된 블록에 대한 픽셀 패턴을 동적으로 생성하고, 제한된 CGRAM 슬롯에 실시간으로 업로드합니다.
    * 마지막으로, 화면을 업데이트하여 해당 커스텀 문자를 올바른 위치에 표시합니다.

---

## ✨ 주요 기능

* **Pixel-Level Control:** `lcd_set_dot(y, x)`를 사용하여 자유로운 화면면 표현 가능.
* **Smart Rendering:** 변경된 블록만 업데이트하여 I2C 통신 부하 최적화.
* **Hybrid Mode:** 표준 텍스트 출력(`lcd_send_string`)과 그래픽 기능을 함께 사용 가능.
* **Optimization:** 메모리 사용량 최소화 및 효율적인 비트 연산 적용.

---

## 🛠️ 요구 사항 (Requirements)

* **MCU:** STM32F411 (테스트 완료), 대부분의 STM32 F 시리즈와 호환.
* **Display:** 1602 LCD (HD44780 호환) + I2C Backpack (PCF8574).
* **IDE:** STM32CubeIDE
* **Driver:** STM32 HAL Driver

---

## 🚀 사용 방법

### 1. 프로젝트 통합
다음 파일들을 프로젝트 폴더에 복사합니다:
* `my_lcd_i2c.c` -> 'Core/Src'
* `my_lcd_i2c.h` -> 'Core/Inc'

### 2. 메인 코드 작성 예시
`main.c` 파일에 다음과 같이 코드를 작성합니다.

~~~c
/* USER CODE BEGIN Includes */
#include "my_lcd_i2c.h"
/* USER CODE END Includes */

int main(void)
{
  /* MCU 초기화 코드 생략 */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init(); 

  /* USER CODE BEGIN 2 */
  // I2C 핸들러를 사용하여 LCD 초기화 (필수)
  lcd_init(&hi2c1);
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
    
    // 애니메이션 예제: 블록 이동시키기
    for(int i = 0; i < 68; i++){
        
        // 1. 가상 버퍼에 픽셀 그리기
        for(int x = 0; x < 12; x++){
            for(int y = 6; y < 10; y++){
                lcd_set_dot(y, i + x); // (y, x) 좌표에 점 찍기
            }
        }
        
        // 2. LCD 화면 렌더링 (실제 출력)
        lcd_print_custom();
        
        HAL_Delay(100);
        
        // 3. 다음 프레임을 위해 가상 버퍼 초기화
        lcd_clear_custom();
    }
  }
}

![Image](https://github.com/user-attachments/assets/0eb6eda9-ed84-435e-ac18-b746cefccf8a)
~~~

---

## 📚 API 문서

| 함수명 | 매개변수 | 설명 | 비고 |
| :--- | :--- | :--- | :--- |
| `lcd_init` | `hi2c` | LCD 및 I2C 연결 초기화 | **최초 1회 필수 호출** |
| `lcd_set_dot` | `y`, `x` | (y, x) 좌표에 점 찍기 | 버퍼만 업데이트됨 |
| `lcd_clear_dot` | `y`, `x` | (y, x) 좌표의 점 지우기 | 버퍼만 업데이트됨 |
| `lcd_print_custom` | `void` | **버퍼 내용을 LCD에 출력** | I2C 전송 수행 |
| `lcd_clear_custom` | `void` | 그래픽 버퍼 전체 초기화 | 애니메이션 구현 시 사용 |
| `lcd_send_string` | `str` | 일반 텍스트 출력 | 기본 기능 |
| `lcd_set_cursor` | `r`, `c` | 커서 위치 이동 | 기본 기능 |

---

## ⚠️ 기술적 제약 사항

HD44780 컨트롤러의 하드웨어 구조로 인해 다음 사항을 유의해야 합니다.

### 1. CGRAM 8개 제한 (The 8-Character Limit)
* LCD는 한 번에 **8개의 커스텀 문자**만 CGRAM에 저장할 수 있습니다.
* **제약:** 화면 전체를 복잡한 그래픽으로 채울 수는 없습니다. 픽셀이 포함된 5x8 블록의 개수가 8개를 초과하면, 초과된 블록은 렌더링되지 않습니다.

### 2. 해상도 및 간격 (Resolution & Gaps)
* **해상도:** 80px (가로) x 16px (세로).
* **물리적 간격:** 문자 블록 사이에 미세한 물리적 간격이 있어, 연속된 가로선이 약간 끊겨 보일 수 있습니다.

<br>

<div align="center">
  Author: <a href="[https://github.com/StrongThrow](https://github.com/StrongThrow)">SongGu Kang</a>
</div>
