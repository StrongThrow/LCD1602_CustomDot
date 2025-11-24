# 🎨 STM32 I2C LCD Custom Graphics Library

<div align="center">
  <img src="https://img.shields.io/badge/STM32-F411-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white">
  <img src="https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white">
  <img src="https://img.shields.io/badge/Library-HAL-FCC624?style=for-the-badge&logo=stm32&logoColor=black">
</div>

<br>

**1602 텍스트 LCD를 그래픽 디스플레이처럼 사용해보세요.**

이 라이브러리는 **Dynamic CGRAM Allocation Algorithm** 기반으로 동작하여,  
일반적인 HD44780 LCD에서 **픽셀 단위 제어, 스프라이트 표현, 애니메이션 구현**을 가능하게 합니다.

---

## 📺 Demo

![LCD Execution Result](./assets/lcd_demo.gif)  
*STM32F411 + 1602 LCD 구현 예시*

---

## ⚙️ How It Works

이 라이브러리는 일반 텍스트 LCD를 **가상의 80×16 픽셀 공간**으로 확장해 표현합니다.

### 🔹 1. Pixel Buffering (`PIXEL_MAP`)
모든 그리기 명령(`lcd_set_dot()`)은 LCD에 바로 적용되지 않고  
**80 × 16 비트 크기의 버퍼에 먼저 기록됩니다.**

→ 이 단계에서는 실제 LCD가 변경되지 않습니다.

---

### 🔹 2. Cell Scanning (5×8 Character Cell Parsing)

1602 LCD는 **가로 16칸 × 세로 2줄 → 총 32칸**으로 구성됩니다.

각 칸은 다음과 같은 **5×8 픽셀 블록**입니다.

┌───5px───┐
│ █ █ █ █ │ ← 8 rows (최대 8비트 표현)
└─────────┘


버퍼를 스캔하여 **픽셀이 포함된 칸만 활성화 셀로 분류합니다.**

---

### 🔹 3. Dynamic CGRAM Allocation

HD44780은 **최대 8개의 사용자 정의 문자(CGRAM)** 를 저장할 수 있습니다.

라이브러리는 활성화된 셀을 순서대로 검사하고:

| 조건 | 처리 |
|------|------|
| 아직 CGRAM 여유공간 있음 → | Custom Character로 변환 & LCD에 등록 |
| 이미 8칸 사용됨 → | 해당 셀은 렌더링 생략 |

---

### 🔹 4. Smart Rendering

LCD 전체를 다시 쓰지 않고,  
**변경된 셀만 업데이트**하여 성능을 최적화합니다.

---

## ✨ 주요 기능

- 🟩 **픽셀 단위 그리기**
- ⚙️ **동적 CGRAM 메모리 관리**
- 🚀 **효율적인 I2C 업데이트**
- 🎨 **애니메이션 가능**
- 🔡 텍스트 함수 (`lcd_send_string()`), 커서 이동과 함께 사용 가능

---

## 🛠 요구 사항

| 항목 | 사양 |
|------|------|
| MCU | STM32F411 (다른 HAL 기반 STM32에서도 동작 가능) |
| LCD | 1602 HD44780 + PCF8574 I2C Backpack |
| IDE | STM32CubeIDE |
| Driver | STM32 HAL |

---

## 🚀 사용 방법

### 1️⃣ 라이브러리 추가

프로젝트에 다음 파일을 추가하세요:
Core/Src/my_lcd_i2c.c
Core/Inc/my_lcd_i2c.h


---

### 2️⃣ 예제 코드

~~~c
#include "my_lcd_i2c.h"

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();

  // LCD 초기화
  lcd_init(&hi2c1);

  while (1)
  {
    // 애니메이션: 왼→오 이동
    for(int i = 0; i < 68; i++)
    {
      // 1. 버퍼에 점 찍기
      for(int x = 0; x < 12; x++){
        for(int y = 6; y < 10; y++){
          lcd_set_dot(y, i + x);
        }
      }

      // 2. LCD 렌더링
      lcd_print_custom();

      HAL_Delay(100);

      // 3. 다음 프레임을 위해 초기화
      lcd_clear_custom();
    }
  }
}
~~~

---

## 📚 API Reference

| 함수 | 설명 |
|------|------|
| `lcd_init()` | LCD 및 I2C 초기화 |
| `lcd_set_dot(y, x)` | 픽셀 생성 |
| `lcd_clear_dot(y, x)` | 픽셀 삭제 |
| `lcd_print_custom()` | 버퍼 기반 그래픽 렌더링 |
| `lcd_clear_custom()` | 버퍼 초기화 |
| `lcd_send_string()` | 텍스트 출력 |
| `lcd_set_cursor()` | 커서 이동 |

---

## ⚠ 기술적 제한

> 이는 LCD의 **하드웨어 구조(CGRAM)**에 기반한 필수 제약입니다.

- 🔹 **동시에 최대 8개의 Custom Character만 표현 가능**
- 🔹 픽셀 간 **간격으로 인해 연결선이 완벽히 연속적으로 보이지 않을 수 있음**
- 🔹 해상도: **80px × 16px**

---

<br>
<div align="center">
  Author: <a href="https://github.com/StrongThrow">SongGu Kang</a>
</div>


