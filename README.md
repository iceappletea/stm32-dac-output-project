# STM32 Dual DAC Waveform Generator with Bluetooth & LCD Interface

## STM32F103RCT6 雙 DAC 波形產生器（含藍牙與 LCD 顯示）

本專案使用 STM32F103RCT6 內建的雙 DAC（PA4、PA5）模擬產生兩路正弦波，並透過 DMA + TIM2 控制波形輸出頻率。使用者可透過編碼器、按鍵或 HC-05 藍牙模組進行參數調整，LCD 顯示模組則提供即時視覺回饋。

**HC-05 藍牙模組的操作功能與實體旋轉編碼器與按鍵完全等效，系統設計上允許僅使用藍牙模組進行控制，或保留實體輸入進行備援。兩者可擇一使用，亦可同時接入。**

This project utilizes the dual DAC output feature of STM32F103RCT6 to generate two analog sine waves. Waveform parameters (frequency and phase) can be controlled via encoder, push button, or HC-05 Bluetooth module. The ST7789 LCD displays frequency, phase, and waveform in real-time. The Bluetooth module can fully substitute the hardware input system.

---

## ✅ Features | 功能總覽

- **雙 DAC 輸出**：PA4 輸出原始正弦波，PA5 輸出相位可調波形。
- **DMA 傳輸**：使用 DMA2_Channel4 以 32-bit 雙通道資料推送至 DAC。
- **TIM2 控制頻率**：以 TIM2 Update 作為 DAC 的 TRGO 觸發。
- **旋轉編碼器控制**：可用來調整頻率或相位。
- **實體按鍵切換模式**：PC1 控制頻率 / 相位切換。
- **HC-05 藍牙控制**：接受如 `arr+5`, `p-1` 等命令進行遠端設定。
- **LCD 顯示介面**：即時顯示當前波形資訊。

---

## 📂 Code Modules | 程式模組

| 檔案名稱 | 功能說明 |
|----------|----------|
| `DAC.c` | 初始化雙 DAC 與 DMA，波形資料來源為 `DualSine12bit[]`。 |
| `Encoder.c` | EXTI2 中斷偵測旋轉方向，控制頻率或相位。 |
| `Key.c` | 控制 PC1 按鍵切換模式。 |
| `HC05.c` | 處理來自 HC-05 的藍牙命令字串（USART3）。 |
| `ALL.h` | 定義全域變數：`wave_offset`, `TIM_Period`, `current_mode` 等。 |
| `lcd.c` | ST7789 顯示模組，顯示文字與圖形波形畫面。 |

---

## 🔌 Peripheral Configuration | 硬體接腳與功能配置

| 外設 | 功能 | 使用腳位 | 備註 |
|------|------|----------|------|
| DAC1 | 原始正弦波輸出 | PA4 | DMA 低 16 位元 |
| DAC2 | 相位偏移波形輸出 | PA5 | DMA 高 16 位元 |
| TIM2 | 更新 DAC 輸出頻率 | 無實體引腳 | 使用 TRGO Update |
| DMA2_Ch4 | 雙通道波形傳輸 | 無實體引腳 | 寫入 `DAC_DHR12RD_ADDRESS` |
| GPIOC.1 | 模式切換按鍵 | PC1 | 控制 FREQ / PHASE |
| GPIOC.2 | 編碼器 A 相 | PC2 (EXTI2) | 配合 GPIO_EXTILineConfig |
| GPIOC.3 | 編碼器 B 相 | PC3 | 用於方向判斷 |
| USART3 | HC-05 藍牙模組 | TX: PB10, RX: PB11 | 接收字串指令 |
| LCD | 圖像顯示 | GPIOA[0~3,6,7] | 詳細腳位見下方定義 |

---

## 📌 LCD 腳位定義（ST7789）

```c
#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)

#define LCD_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define LCD_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define LCD_RST_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define LCD_RST_Set() GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define LCD_DC_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_3)
#define LCD_DC_Set() GPIO_SetBits(GPIOA,GPIO_Pin_3)

#define LCD_CS_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define LCD_CS_Set() GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define LCD_BLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define LCD_BLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)
```

---

## 🔧 HC-05 Remote Command Guide | 藍牙指令與實例說明

### 📡 Frequency Control | 頻率控制

頻率計算公式：

```
f = 72_000_000 / (256 × TIM_Period)
```

例：當 `TIM_Period = 703`，則：

```
f = 72_000_000 / (256 × 703) = 72_000_000 / 179968 ≈ 400.007112 Hz
```

- `arr-5`：減少 TIM_Period → 頻率上升，例如變成 698 ≈ 402.3 Hz
- `arr+1`：增加 TIM_Period → 頻率下降，例如變成 704 ≈ 399.44 Hz

**這裡與編碼器正轉／反轉功能一致。**

---

### 🔄 Phase Control | 相位控制

相位計算公式：

```
Phase (°) = wave_offset × (360 / 256) = wave_offset × 1.40625
```

- `p+1`：相位偏移 +1 → +1.4°

---

### ⚙️ Mode Switching | 模式切換

| 指令 | 說明 |
|------|------|
| `mode freq` | 切換為「頻率調整模式」 |
| `mode phase` | 切換為「相位調整模式」 |

**這裡與按鍵功能一致（PC1）。**

---

## 📚 References | 參考資料

### 📘 DAC 引腳固定與裝置選型

- 根據《STM32F103xC/D/E datasheet》中的 Alternate Function Mapping 表列：
  - PA4 → DAC_OUT1（無法 AFIO 重映射）
  - PA5 → DAC_OUT2（固定腳位）
- STM32F103C8T6 無 DAC 模組，需選擇具備 DAC 的 RCT6 / RET6 等晶片。

### 🧱 雙通道 DAC 架構與 DMA 寫入說明

- STM32 的 DAC 為一個模組，內含兩個通道：
  - DAC_Channel_1 → PA4
  - DAC_Channel_2 → PA5
- 可使用同步模式：
  ```c
  DualSine12bit[i] = (Sine12bit[idx] << 16) | Sine12bit[i];
  ```
  - 上 16-bit → DAC2
  - 下 16-bit → DAC1
- 資料會寫入 `DAC_DHR12RD` 暫存器（32-bit），定義於 Reference Manual 第 270 頁：
  - Bit 27~16：DAC2輸出第二通道資料
  - Bit 11~0：DAC1輸出第一通道資料

### ⏱️ TIM2 與 TRGO 設定

- DAC 的觸發來源來自 TSEL[2:0] 設定，定義於 Reference Manual 第 254 頁：
  - 可選TIM2_TRGO/TIM4_TRGO/TIM5_TRGO/TIM6_TRGO/TIM7_TRGO/TIM8_TRGO
- 選擇由 TIM2 的 Update Event 作為同步更新 DAC 的頻率控制源。

### 🧩 為何使用 `|` 而非 `+`

- `|` 可以避免進位錯誤
- 明確指出：上 16-bit 為 DAC2、下 16-bit 為 DAC1

- **ST7789 驅動**：  
  使用 [中景園電子 1.3LCD 技術資料] 所提供的驅動程式碼。
  
- **DAC 輸出**：  
  參考野火教材《STM32庫開發實戰指南》（標準庫原始碼）內範例：  
  `1-書籍配套例程\38-DAC—输出正弦波`

- **HC-05 藍牙控制**：  
  參考 CSDN 用戶「玄奕子」分享的 HC-05 控制範例（USART 指令接收處理）

---

## 🖼️ Screenshots

