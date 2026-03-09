# RA6M3-GUI-Controller
SCI UART 통신 프로토콜을 이용한 RA6M3 보드 제어용 PC GUI 인터페이스
- PC의 GUI 환경에서 버튼 조작을 통해 Renesas RA6M3 보드의 주변장치를 실시간으로 제어하고 상태를 확인하는 프로젝트

## Key Features
- Real-time Control: GUI 버튼 클릭 시 지연 없는 하드웨어 반응.
- Status Monitoring: 보드의 센서 데이터나 핀 상태를 GUI에 실시간 표시.
- Error Handling: 잘못된 패킷 수신 시 예외 처리 및 재전송 요구 로직 포함.



## GUI Demo
<img width="2296" height="1184" alt="image" src="https://github.com/user-attachments/assets/05f1c763-23ef-4d52-8487-00bf6b0fe311" />


## Tech Stack
- MCU: Renesas RA6M3 (Cortex-M4)
- Development Environment: e2 studio, FSP (Flexible Software Package)
- Language: C (Firmware)
- Interface: SCI UART (Serial Communication Interface)


## System Architecture & Protocol
보드와 PC 간의 데이터 통신 구조

### 1. Hardware Connection
- PC (USB-to-TTL) <-> RA6M3 (SCI Channel 0)
- Baudrate: 115200 bps
- Data bits: 8 / Stop bits: 1 / Parity bits: None

### 2. Communication Protocol (Packet Structure)
| Field | Size (Byte) | Value/Type | Description |
| :--- | :---: | :---: | :--- |
| STX | 1 | `0x02` | Start of Text (프레임 시작) |
| Group Number | 1 | ASCII | Control Unit 결정 |
| CMD Class | 1 | ASCII | 명령 카테고리, Control / Update 결정 |
| CMD | 1 | ASCII | 세부 동작 명령 |
| Data Byte | 1 | ASCII | 뒤따르는 Data 필드의 길이 (N) |
| Data | 0~N | ASCII | Control Unit별 Data 포함 |
| ETX | 1 | `0x03` | End of Text (프레임 종료) |

#### 2-1. Protocol Example
- Scenario: Group '1', Class 'A'의 LED를 켜기('O') 위해 데이터 '1'을 전송할 경우
- Full Packet: `0x02` (STX) + `0x31` ('1') + `0x41` ('A') + `0x4F` ('O') + `0x31` ('1') + `0x31` ('1') + `0x03` (ETX)

### 3. Project Structure
```text
├── GUI/                  # PC 제어용 실행 파일 (.exe)
│   └── RA6M3_Controller.exe
│
├── code/                 # RA6M3 C 소스 코드 (e2 studio Project)
│   ├── .settings/        # 컴파일러 옵션 및 프로젝트 환경 설정 (숨김 폴더)
│   ├── ra/               # FSP 라이브러리 소스 및 드라이버 본체
│   ├── ra_cfg/           # FSP 구성(Configuration) 헤더 파일
│   ├── ra_gen/           # FSP에서 자동 생성한 설정 코드
│   ├── script/           # 링커 스크립트 (.ld) 등 빌드 관련 스크립트
│   ├── src/              # 사용자 작성 메인 로직 및 드라이버 소스 (.c, .h)
│   ├── .project          # e2 studio 프로젝트 정보 파일
│   ├── .cproject         # C/C++ 프로젝트 설정 파일
│   ├── configuration.xml # FSP 그래픽 설정 저장 파일 (가장 중요)
│   └── R7FA6M3AH3CFC.pincfg          # MCU 핀 할당 설정 파일
│
├── docs/                 # 프로젝트 설계 및 설명 문서
│   ├── Hardware_Setup.md
│   ├── Communication_Protocol.md
│   └── Software_Implementation.md
│
├── images/               # 프로젝트 동작 영상
│
└── README.md
