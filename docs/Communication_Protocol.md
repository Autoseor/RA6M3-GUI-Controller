# Communication Protocol Specification

본 문서는 PC GUI Application과 RA6M3 보드 간의 데이터 송수신 규격 및 처리 로직을 정의합니다. 모든 데이터는 ASCII 코드를 기반으로 통신합니다.

---


## 1. Packet Frame Structure

데이터의 시작과 끝을 식별하기 위해 STX/ETX 프레임을 사용하며, 헤더를 통해 장치를 구분합니다.

| Index | Field | Byte | Value (Hex) | Description |
| :---: | :--- | :---: | :---: | :--- |
| 0 | **STX** | 1 | `0x02` | 프레임 시작 (Start of Text) |
| 1 | **Group No.** | 1 | `0x30~0x36` | 장치 그룹 번호 (ASCII '0'~'6') |
| 2 | **CMD Class** | 1 | `0x30~0x39` | 명령 카테고리 (ASCII '0'~'9') |
| 3 | **CMD Detail** | 1 | `0x30~0x39` | 세부 동작 명령 (ASCII '0'~'9') |
| 4 | **Data Byte** | 1 | `0x30~0x39` | 뒤에 올 데이터의 길이 N (ASCII) |
| 5 | **Data** | N | Variable | 실제 제어 값/데이터 (ASCII) |
| max | **ETX** | 1 | `0x03` | 프레임 종료 (End of Text) |

---

## 2. Command Details

각 주변 장치는 `Group(Idx 1)` 번호를 통해 구분됩니다.

### [Group: `0x30`] LED 제어
| CMD Class (Idx 2) | CMD Detail (Idx 3) | Data Byte (Idx 4) | Data (Idx 5~) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0` (`0x30`) | `0` (`0x30`) | `0` (`0x30`) | `0` (`0x30`) | 1번 LED 상태 반전 (Toggle) |
| `0` (`0x30`) | `0` (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | 2번 LED 상태 반전 (Toggle) |

### [Group: `0x31`] FND 제어
| CMD Class (Idx 2) | CMD Detail (Idx 3) | Data Byte (Idx 4) | Data (Idx 5~) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0` (`0x30`) | **`0`** (`0x30`) | `1` (`0x31`) | `0~9` | 10진수 한 자리 출력 |
| `0` (`0x30`) | **`1`** (`0x31`) | `1` (`0x31`) | `0~F` | 16진수 한 자리 출력 |
| `0` (`0x30`) | **`2`** (`0x32`) | `4` (`0x34`) | `0000~1111` | 2진수 4자리 수신 후 16진 변환 출력 |

### [Group: `0x34`] DC 모터 제어
| CMD Class (Idx 2) | CMD Detail (Idx 3) | Data Byte (Idx 4) | Data (Idx 5~) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0` (`0x30`) | **`0`** (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | Motor ON (초기: CCW, High) |
| `0` (`0x30`) | **`1`** (`0x31`) | `0` (`0x30`) | `1` (`0x31`) | Motor OFF |
| `0` (`0x30`) | **`2`** (`0x32`) | `0` (`0x30`) | `1` (`0x31`) | 속도 설정: High (Duty 20% or 80%) |
| `0` (`0x30`) | **`3`** (`0x33`) | `0` (`0x30`) | `1` (`0x31`) | 속도 설정: Low (Duty 60% or 40%) |
| `0` (`0x30`) | **`4`** (`0x34`) | `0` (`0x30`) | `1` (`0x31`) | 방향 설정: CW (시계 방향) |
| `0` (`0x30`) | **`5`** (`0x35`) | `0` (`0x30`) | `1` (`0x31`) | 방향 설정: CCW (반시계 방향) |

### [Group: `0x35`] ADC 센서 제어
| CMD Class (Idx 2) | CMD Detail (Idx 3) | Data Byte (Idx 4) | Data (Idx 5~) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0` (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | `1` (`0x31`) | ADC 데이터 전송 시작 (ON) |
| `0` (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | `0` (`0x30`) | ADC 데이터 전송 중단 (OFF) |

### [Group: `0x36`] DAC 사운드 제어
| CMD Class (Idx 2) | CMD Detail (Idx 3) | Data Byte (Idx 4) | Data (Idx 5~) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0` (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | `1` (`0x31`) | Sound 1 재생 (rawData1) |
| `0` (`0x30`) | `0` (`0x30`) | `1` (`0x31`) | `2` (`0x32`) | Sound 2 재생 (rawData2) |

---

## 3. Advanced Control Logic: DC Motor PWM

RA6M3의 GPT(General Purpose Timer) 레지스터를 직접 제어하여 고속 응답성을 확보했습니다.

### PWM Duty Cycle Mapping Table
모터 드라이버 특성을 보완하기 위해 방향에 따라 Duty 값을 역산하여 일관된 속도를 유지합니다.

| 방향 (DIR) | 속도 (Speed) | GTCCR[0] 설정값 (Duty) | DIR Pin (P901) |
| :---: | :---: | :--- | :---: |
| **CW** | **High** | `Timer_Period * 0.2` (20%) | **HIGH** |
| **CW** | **Low** | `Timer_Period * 0.6` (60%) | **HIGH** |
| **CCW** | **High** | `Timer_Period * 0.8` (80%) | **LOW** |
| **CCW** | **Low** | `Timer_Period * 0.4` (40%) | **LOW** |


### Software Optimization
- Register Direct Access: `R_GPT3->GTCCR[0]`에 직접 Write 하여 FSP API 오버헤드 제거.

- Interrupt Lock: DAC 사운드 재생 시 `IRQ_Disable()`을 호출하여 타이밍 왜곡 방지.
