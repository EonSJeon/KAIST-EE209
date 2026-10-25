# EE209 Assignments

`instructions.md`에 따라 과제 PDF에서 제목을 읽어 폴더 이름을 정리하고, 기존 코드 리뷰와 수정본을 분리했다. 변경·설치·빌드·실행 산출물은 모두 `assignments` 안에 둔다. `precepts`는 읽기만 한다.

| 번호 | 프로젝트 | 원본 리뷰 | 수정본 실행 |
|---|---|---|---|
| 1 | A Word Counting Program | [review](<1 A Word Counting Program/old/review.md>) | `"./assignments/1 A Word Counting Program/corrected/run.sh"` |
| 2 | String Manipulation | [review](<2 String Manipulation/old/review.md>) | `"./assignments/2 String Manipulation/corrected/run.sh" -f EE209` |
| 3 | Customer Management Table | [review](<3 Customer Management Table/old/review.md>) | `"./assignments/3 Customer Management Table/corrected/run.sh"` |
| 4 | Assembly Language Programming | [review](<4 Assembly Language Programming/old/review.md>) | `"./assignments/4 Assembly Language Programming/corrected/run.sh"` |
| 5 | A Unix Shell | [review](<5 A Unix Shell/old/review.md>) | `"./assignments/5 A Unix Shell/corrected/run.sh"` |

## 원본과 수정본

각 프로젝트의 `old`에는 기존 코드를 그대로 두고 **`WRONG:` 주석만 추가**했다. 원래 주석·실행문·공백·개행은 보존했다. 기존 바이너리와 중간 산출물도 old에 보관하지만 현재 환경에서는 corrected를 새로 빌드해 실행한다. 별도의 `originals` 디렉터리는 없다.

`old/review.md`는 오류의 원인, 관련 개념, 바꾸면 좋은 습관, 수정본과의 대응, 검증 범위를 한국어로 설명한다. `corrected`의 새 수정 설명은 **`CORRECTED:`**로 시작한다. 기존에 맞는 부분은 맞다고 설명하고, 기능 오류와 방어적/스타일 개선을 구분했다. 특히 과제 1의 DFA는 정상 입력 검사에서 맞아 유지했다.

PDF는 각 프로젝트 최상위에 두었다. 과제 5의 Supplement도 함께 보관했다. 누락된 과제 3의 공개 헤더와 테스트 클라이언트는 PDF 기준으로 새로 만들었으며 원래 제공된 파일이라고 표시하지 않았다. 수정본 `readme`에는 이번 Codex 도움을 명시했다. 서명된 윤리 서약서는 생성하지 않았다.

## 환경 설정

저장소 루트에서:

```sh
./assignments/setup.sh
. ./assignments/.venv/bin/activate
```

이미 이 작업 환경에는 `.venv`와 의존성을 설치해 두었다. 다른 컴퓨터에서는 setup을 먼저 실행한다. Python 3.10 이상, C 컴파일러 `cc`, IA-32 어셈블러로 쓸 `clang`, `make`가 필요하다. macOS에서는 Command Line Tools의 도구를 사용한다.

Python venv 자체가 C 컴파일러를 설치하거나 C ABI를 바꾸는 것은 아니다. venv 안에는 PDF 검사와 IA-32 실행용 패키지를 고정 버전으로 설치하고, `ee209`/`gcc209` 실행 도구를 연결한다. `gcc209`는 현재 시스템 C 컴파일러에 공통 경고/표준 옵션을 붙이는 편의 래퍼이며 과거 학교의 동일 이름 프로그램과 같은 바이너리는 아니다.

```sh
# 수정본 전체를 빌드하고 자동 검사 (대화형 입력 없이 끝남)
./assignments/run_all.sh

# precepts의 55개 실행 예제도 함께 빌드·실행
./assignments/run_all.sh --with-precepts

# 활성화 후 같은 기능
ee209 all
ee209 assignment 4 --test
```

`run_all.sh`는 검사 실패 시 0이 아닌 상태로 종료한다. 각 `corrected/run.sh`는 경로에 공백이 있거나 다른 작업 디렉터리에서 호출해도 동작한다.

## 과제별 실행

각 `run.sh`는 기본적으로 컴파일 후 프로그램을 실행한다. 프로그램 인자는 그대로 전달한다. 과제 3은 main이 없는 라이브러리이므로 두 구현에 링크한 테스트 클라이언트를 실행한다.

```sh
printf 'abc/*comment*/def\n' | "./assignments/1 A Word Counting Program/corrected/run.sh"
printf 'ababa\n' | "./assignments/2 String Manipulation/corrected/run.sh" -r aba X
"./assignments/3 Customer Management Table/corrected/run.sh"
printf '2\n0\n^\np\nq\n' | "./assignments/4 Assembly Language Programming/corrected/run.sh"
"./assignments/5 A Unix Shell/corrected/run.sh"
```

모든 과제는 다음 옵션을 공통 지원한다.

| 옵션 | 동작 |
|---|---|
| `--build` | 빌드하고 실제 실행 명령 출력 |
| `--test` | 해당 과제 회귀 검사 |
| `--demo` | 짧은 입력 예제로 실행 (셸 데모는 임시 HOME으로 격리) |
| `-- args...` | 예약 옵션과 겹치는 프로그램 인자를 그대로 전달 |

과제 1은 전처리/컴파일/어셈블/링크를 나누어 `.i`, `.s`, `.o`, 실행 파일을 만든다. 과제 5의 makefile은 모듈별 object와 header dependency를 관리한다. 각 과제의 생성 파일은 `corrected/.build` 안에만 둔다.

## Apple Silicon의 IA-32 실행

과제 4와 precept 7·8의 `.s`는 32비트 x86 ELF/GNU 문법이다. arm64 macOS에서 그대로 네이티브 링크/실행할 수 없어, Clang의 `-target i386-unknown-linux-gnu`로 object를 만든 뒤 Unicorn으로 **실제 IA-32 명령어**를 실행한다. 계산기를 C로 바꿔 결과만 흉내 내지 않는다.

[tools/ia32.py](tools/ia32.py)는 이 수업 예제에 필요한 ELF relocation과 소수의 libc 호출만 지원하는 실행기다. Linux 전체, 임의의 i386 바이너리, 수업 채점 환경을 재현하지 않는다. 지원하지 않는 심볼/relocation은 오류로 보고한다. libc 호출 후 caller-saved ECX/EDX를 실제로 훼손해 레지스터 보존에 대한 잘못된 가정도 드러낸다. 기본 명령어 한도는 5천만 개이고 stack memory는 8MiB다.

macOS의 제한된 샌드박스는 Unicorn JIT를 중단시킬 수 있다. 이 경우 일반 터미널에서 스크립트를 실행하면 된다. 이 작업에서는 JIT 실행을 허용한 환경에서 어셈블리 테스트를 마쳤다.

## precepts 실행 (원본 수정 없음)

```sh
./assignments/precepts.sh --list
./assignments/precepts.sh --build-all
./assignments/precepts.sh --test
printf '2\n5\n' | ./assignments/precepts.sh 07-ia32-assembly-basics/power-signed/power.s
./assignments/precepts.sh 04-arrays-pointers-and-functions/command-line/printargv.c hello EE209
```

precept 소스는 `../precepts/src`에서 읽으며, 모든 바이너리·로그·실행 작업 파일은 `assignments/.build/precepts`에 둔다. 여러 소스로 된 intmath/DynArray와 lexer의 의존성도 실행기가 연결한다. `testexec.c`에 필요한 `./hello`, fd 예제의 `tempfile`, 복사 예제의 입력은 그 안의 작업 디렉터리에 준비한다.

원래 `precepts/.../lexing/dfa.c`에는 main만 있고 호출하는 `lexLine`/print/free helper 정의가 없다. `assignments/tools/precept_lexer.c/h`를 외부에서 연결해 컴파일/실행 가능하게 했다. 이는 복구한 원본 구현이 아니라, 공백으로 구분된 영문 단어/십진 숫자를 처리한다고 가정해 보완한 교육용 구현이다. precept 원문에는 손대지 않았다.

precept는 원래의 경고와 수업용 버그를 유지한다. 빌드 경고는 각 프로그램의 `build.log`에 남긴다. `testsignal`, `testsignalignore`, `testforkwaitexec`는 원래 무한 루프 예제이므로 `--test`에서 잠시 관찰한 뒤 해당 테스트 프로세스 그룹만 종료한다. 실행 가능 여부 확인과 모든 교육 예제의 알고리즘 정확성 보장은 별개다.

## 검증과 보존 확인

- 수정본은 `-std=c99 -Wall -Wextra -Wpedantic -Werror`로 빌드한다.
- 문자열의 모든 바이트 비교, 주석 경계 임의 입력, DB 확장/삭제/재사용과 할당 실패 rollback, 계산기 signed 경계와 잘못된 토큰을 검사한다.
- 셸은 200KB 파이프와 낮은 FD 한도 반복 실행, HOME 시작 파일, 리다이렉션, EOF/사용자 오류 복구, 실제 SIGINT/SIGQUIT와 5초 창을 검사한다.
- AddressSanitizer/UndefinedBehaviorSanitizer 검사는 네이티브 C에 적용한다. IA-32 검증은 동일한 어셈블리 실행기에서 수행하며, sanitizer가 IA-32 object를 계측했다는 뜻은 아니다.

```sh
EE209_SANITIZE=1 ./assignments/run_all.sh
./assignments/.venv/bin/python ./assignments/tools/verify_integrity.py
```

`verify_integrity.py`는 old 코드의 추가된 `WRONG:` 주석 줄만 제거한 바이트가 작업 시작 시 원본 SHA-256과 일치하는지 검사한다. PDF/기존 산출물도 검사한다. precepts의 전체 파일 경로·내용·권한도 시작 시 스냅샷과 비교한다. 기록은 [tools/integrity.json](tools/integrity.json)에 있으며, 보존 검사 자체는 파일을 쓰지 않는다.

최신 자동 검사 결과는 `.build/assignment*-results.json`, precept 결과는 `.build/precepts/results.json`, DB 클라이언트의 측정 출력은 `.build/customer-results.txt`에 남는다. 공식 sample 실행 파일은 보관본에 없어 과제 PDF와 자체 테스트를 기준으로 검증했다.
