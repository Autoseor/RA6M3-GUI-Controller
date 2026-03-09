# Software Implementation Logic

본 문서는 RA6M3 펌웨어의 핵심 알고리즘 및 하드웨어 제어 로직의 구현 세부 사항을 기술합니다.

---

## 1. UART Message Processing Flow

PC GUI로부터 수신된 패킷은 `process_uart_message` 함수에서 다음과 같은 단계로 처리됩니다.

1.  Frame Integrity Check: `Index 0(STX)`와 `Index max(ETX)`를 검사하여 데이터 누락 및 프레임 깨짐 방지
2.  Multistage Dispatching:
    - 1단계: `Group No.(Idx 1)`를 통해 제어 대상(LED, Motor, ADC 등) 분류
    - 2단계: `CMD Detail(Idx 3)` 및 `Data(Idx 5)`를 분석하여 세부 동작 수행
3.  Data Conversion: `ASCIItoHEX` 커스텀 함수를 사용하여 ASCII 문자로 수신된 데이터(`0x30~`)를 실제 연산이 가능한 정수형 데이터로 변환

---

## 2. Key Technology Points

### PWM & Motor Control (Register Direct Access)
성능 최적화와 정밀한 제어를 위해 FSP(Flexible Software Package) API 대신 MCU 레지스터에 직접 접근하는 방식을 채택했습니다.

- Direct Register: `R_GPT3->GTCCR[0]` 레지스터를 직접 수정하여 PWM Duty Cycle 즉각 변경
- Direction Inversion Logic: 방향(CW/CCW) 전환 시 Duty 값을 역산(`20%` <-> `80%`) 처리하여 일관된 속도 구현

### FND Dynamic Scanning
4자리 FND를 적은 수의 GPIO로 제어하기 위해 동적 스캐닝(Dynamic Scanning) 기법을 사용합니다.
- 타이머 인터럽트를 이용하여 약 1ms~2ms 주기로 각 자릿수(Digit) 순차적 점등
- 잔상 효과(Persistence of Vision)를 활용하여 4자리가 동시에 켜져 있는 것처럼 보이도록 설계

### DAC Sound Output & System Stability
DAC를 통한 고품질 사운드 재생 시 CPU 자원 점유율이 높아짐에 따른 시스템 불안정을 방지하기 위해 다음 전략을 사용합니다.

- Interrupt Locking: 사운드 데이터를 DAC로 전송하는 루프 동안 `IRQ_Disable()`을 호출하여 UART 및 스위치 인터럽트를 일시적으로 차단, 오디오 샘플링 타이밍의 왜곡 방지
- Critical Section: 재생이 끝난 후 즉시 `IRQ_Enable()`과 `R_SCI_UART_Open()`을 통해 통신 시스템 복구

---

## 3. Peripheral Control Summary

| Peripheral | Control Method | Key Driver/Module |
| :--- | :--- | :--- |
| LED | GPIO Output | `R_IOPORT_PinWrite` |
| FND | Multiplexing | GPIO + Timer Interrupt |
| DC Motor | PWM (GPT) | `R_GPT3->GTCCR[0]` (Direct) |
| Sensor | 12-bit ADC | `R_ADC_Read` (Scanning Mode) |

| Speaker | 12-bit DAC | `R_DAC_Write` (Software Delay) |
