# asio-server

C++ ASIO 기반 분산 게임 서버 포트폴리오.

MMORPG 구조를 단순화하여, **존(Zone) 분산**과 **존 간 이동**을 중심으로
실시간 게임 서버의 핵심 설계를 코드로 구현하는 것을 목표로 합니다.

---

## 목표

- 멀티 존(Zone 1~4) 분산 처리
- 존 간 이동(핸드오프) 및 존 경계에서의 액터 시야 동기화(AOI)
- 몬스터 배치 및 AI(FSM) 구동
- 락-프리 구조(스레드 어피니티)를 활용한 실시간 패킷 처리

클라이언트는 별도 저장소에서 **C# MonoGame**으로 구현 예정입니다.

---

## 서버 토폴로지

```
Client
  └─ GatewayServer   진입점, 토큰 검증, 핸드오프
       ├─ LoginServer  로그인, 대기열, JWT 발급
       └─ WorldServer  캐릭터, 글로벌 상태, 존 라우팅
            ├─ ZoneServer  Zone 1 (실시간 처리)
            ├─ ZoneServer  Zone 2
            ├─ ZoneServer  Zone 3
            └─ ZoneServer  Zone 4
                 └─ PostgreSQL (DB)
```

---

## 프로젝트 구조

```
asio-server/
├─ core/             공용 정적 라이브러리 (네트워크/패킷/스레드 기반 코드)
│   ├─ Session.h     소켓 1개 = 클라이언트 1개
│   └─ Listener.h    accept 루프, 세션 생성
├─ zone_server/      존 서버 실행 파일 (core 참조)
│   └─ main.cpp
├─ 3rd/
│   └─ asio/include/ standalone ASIO 헤더
├─ .gitignore
└─ README.md
```

`core`는 정적 라이브러리(.lib)로 빌드되어 모든 서버가 공유합니다.
서버별 실행 파일(zone/world/login/gateway)은 `core`를 참조만 합니다.

---

## 기술 스택

| 항목 | 내용 |
| --- | --- |
| 언어 | C++20 |
| 네트워크 | standalone ASIO 1.38.0 (Boost 비의존) |
| DB | PostgreSQL + libpqxx (예정) |
| 빌드 | Visual Studio 2026, Windows x64 |
| 클라이언트 | C# MonoGame (별도 저장소, 예정) |

---

## 빌드 방법

### 1. 저장소 클론

```bash
git clone https://github.com/QunaPark/asio-server.git
```

### 2. ASIO 준비

`3rd/asio/include/`에 standalone ASIO 헤더가 포함되어 있습니다.
(미포함 시: [ASIO 1.38.0](https://github.com/chriskohlhoff/asio) 릴리스에서
`asio/include/` 폴더를 받아 위 경로에 복사)

### 3. 빌드

`asio-server.sln`을 Visual Studio 2026으로 열고 빌드(F7).

빌드 산출물:
- `bin/x64/Debug/core.lib`
- `bin/x64/Debug/zone_server.exe`

### 프로젝트 설정 (참고)

- C++ 언어 표준: ISO C++20
- 추가 포함 디렉터리: `$(SolutionDir)3rd\asio\include`
- 전처리기 정의: `ASIO_STANDALONE`, `_WIN32_WINNT=0x0A00`

---

## 핵심 설계

### 스레드 어피니티 (락-프리)

```
player_id % thread_count = 담당 스레드 고정
→ 같은 플레이어 패킷은 항상 같은 스레드에서 처리
→ PlayerState 접근 시 락 불필요
```

### ASIO 역할 분리

```
io_context 스레드  네트워크 수신 전담
WorkerThread       게임 로직 전담
DB ThreadPool      DB 작업 전담 (블로킹 격리)
```

---

## 로드맵

| 단계 | 내용 | 상태 |
| --- | --- | --- |
| 0 | 프로젝트 셋업 (core + zone_server, ASIO 연동) | 완료 |
| 1 | ASIO echo 서버 (Session / Listener) | 진행 중 |
| 2 | PacketHeader 기반 패킷 단위 송수신 | 예정 |
| 3 | WorkerThread + player_id 라우팅 (스레드 어피니티) | 예정 |
| 4 | Actor / Player / Monster, 이동 동기화 | 예정 |
| 5 | AOI(GridMap) 기반 시야 동기화 | 예정 |
| 6 | 몬스터 AI (FSM) | 예정 |
| 7 | 존 4개 확장 + 존 이동(핸드오프) | 예정 |
| 8 | 존 경계 시야 (인접 존 액터 노출) | 예정 |
| 9 | WorldServer + DB(libpqxx) 연동 | 예정 |
| 10 | C# MonoGame 클라이언트 | 예정 |

---

## 라이선스

학습 및 포트폴리오 용도.
ASIO는 Boost Software License를 따릅니다.