# Hardware Configuration & Pin Mapping

이 문서는 RA6M3 보드와 외부 주변장치(LED, FND, Speaker, Motors) 간의 물리적 연결 정보를 다룹니다.

## 1. System Block Diagram

```mermaid
graph TD
    %% 중앙 제어부 및 전원
    subgraph System_Core [Main System]
        MCU((RA6M3 MCU))
        USB[USB-C 5V Power]
    end

    %% 전원 공급 흐름 (보드에서 직접 공급)
    USB ==> MCU
    MCU ==>|5V/3.3V Power Line| SPK
    MCU ==>|5V/3.3V Power Line| DC

    %% 입력부 (Sensors)
    subgraph Inputs [Analog Sensors]
        VR[Variable Resistor] ---|ADC 0| MCU
        TS[Thermal Sensor] ---|ADC 1| MCU
        CS[Cds Sensor] ---|ADC 2| MCU
    end

    %% PC 통신
    subgraph PC_Interface [User Interface]
        GUI[PC GUI App] == "UART SCI0" ==> MCU
    end

    %% 출력부 (Actuators & Display)
    subgraph Outputs [Actuators & Display]
        MCU ---|GPIO| LED[LED x4]
        MCU ---|GPIO/Digit| FND[4-Digit FND]
        MCU ---|PWM/Freq| SPK[Speaker]
        MCU ---|PWM/Dir| DC[DC Motor]
    end

    %% 스타일링
    style MCU fill:#f9f,stroke:#333,stroke-width:2px
    style USB fill:#fff,stroke:#333,stroke-dasharray: 5 5
```

## 2. Pin Mapping Table

### 2-1. Communication & Indicators
| 주변 장치 | 기능 | 보드 핀 | 동작 방식 | 비고 |
| :--- | :--- | :---: | :---: | :--- |
| UART (SCI) | PC GUI 통신 | P411(TX), P410(RX) | - | SCI Channel 0 |
| LED (4ea) | 상태 표시 | P006, P008, P009, P010 | Active Low | - |

### 2-2. Display (FND 4-Digit)
| 주변 장치 | 기능 | 보드 핀 | 동작 방식 | 비고 |
| :--- | :--- | :---: | :---: | :--- |
| Digit Select | 자릿수 선택 (1~4) | P305 ~ P308 | Active High | Dynamic Scanning |
| 7-segment | 데이터 출력 | P604 ~ P607, P611 ~ P614 | Active Low | Common Anode 타입 |

### 2-3. Actuators
| 주변 장치 | 기능 | 보드 핀 | 동작 방식 | 비고 |
| :--- | :--- | :---: | :---: | :--- |
| DC Motor | 이동/속도 제어 | P415 (PWM), P900(DIR) | Active High | GPT PWM 제어 |
| Speaker | 소리 발생 | P014 (PWM) | Active High | PWM Frequency 제어 |

### 2-4. Analog Sensors (ADC)
| 주변 장치 | 기능 | ADC 채널 | 입력 범위 | 비고 |
| :--- | :--- | :---: | :---: | :--- |
| Variable Resistor | 저항치 측정 | ADC CH0 | 0 ~ 3.3V | 12-bit Resolution |
| Thermal Sensor | 온도 측정 | ADC CH1 | 0 ~ 3.3V | - |
| Cds Sensor | 주변 밝기 감지 | ADC CH2 | 0 ~ 3.3V | - |

## 3. Power Supply
- MCU Board: USB-C 5V 기반 전원 공급

- Actuators: 보드 내 VCC 전원 라인 공유 및 GND 공통 접지 (Common GND)





