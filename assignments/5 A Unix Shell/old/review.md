# 과제 5 원본 리뷰

## 판단과 요구사항

PDF 제목은 **A Unix Shell**이다. 마지막 과제는 작은 문자열 오류 하나보다 **토큰 소유권, argv 계약, 파일 디스크립터 수명, 부모/자식 역할, 시그널 안전성**이 동시에 엉킨 문제가 컸다. 기존 `DynArray`, `Token`, `lexLine`, `synLine`, `singleLine`, `fork/execvp` 흐름은 유지하고 책임별 파일로 나눴다. PDF 보충 문서는 소스 파일 수준의 모듈화도 필수로 요구한다.

## 치명적인 오류

| 위치 | 반례 / 영향 | 수정 개념 |
|---|---|---|
| `unitCommandExtractor` | 단일 인자에도 `(개수-1)`만 할당하고 마지막 NULL을 넣지 않는다. `execvp`가 배열 밖을 읽는다. | argc+1 슬롯, NULL 종료 |
| 공백만 있는 줄 | 토큰 0개인데 `DynArray_get(...,0)`으로 assert 종료한다. | 빈 입력도 정상 상태 |
| 메인 입력 반복 | `fgets`의 EOF를 무시해 Ctrl-D 뒤 이전 명령을 반복한다. | EOF와 읽기 오류를 분리 |
| 파이프 FD | 부모/자식에 불필요한 쓰기 끝이 남아 소비자가 EOF를 못 받고 기다린다. | 각 프로세스의 FD 소유권과 참조 수 |
| exec 실패 | `abort` 및 그룹 SIGINT로 셸까지 영향을 준다. | 자식만 오류 보고 후 `_exit`, 정확한 PID wait |
| 시그널 종료 | 이미 해제한 전역 토큰을 다시 free할 수 있고, 핸들러에서 free/printf/exit를 쓴다. | `sig_atomic_t`, async-signal-safe 처리 |

## 렉서·파서 오류

`END_QUOTE`의 공백 분기에서 `iValueIndex`를 초기화하지 않는다. 예를 들어 `"a" b`에서 두 번째 단어에 앞 단어 문자가 섞일 수 있다. 인접한 `"a""b"`, 빈 문자열 `""`, 따옴표 안의 `|`도 별도로 검증해야 한다. 수정본은 닫힌 따옴표에서 `IN_WORD`로 돌아가 기존 단어를 이어 붙인다. `isspace`에는 unsigned char를 전달한다.

원본은 `|`만 특수 토큰으로 처리하고 `<`, `>` 리다이렉션이 없다. 수정본은 가산점 기능도 구현했다. 파서가 먼저 빈 명령, 연속/끝 파이프, 빠진 파일명, 중복 리다이렉션, 파이프 오른쪽 입력/왼쪽 출력 리다이렉션을 거절한다. builtin은 어느 파이프 위치에서도, 파일 리다이렉션과도 함께 쓸 수 없다. 검증이 끝나기 전에 출력 파일을 열거나 프로세스를 만들지 않는다.

## 초기화·자원·오류 처리

- `.ishrc`는 현재 디렉터리 대신 `HOME/.ishrc`에서 읽고 `% `와 원문을 출력해야 한다. 없거나 읽을 수 없는 파일은 허용한다.
- `makeAddToken`의 배열 추가 실패, 단일 명령 성공 경로, 재할당한 `pgmArgs`와 pipes 등에서 메모리가 누수된다. 수정본은 한 줄의 토큰/실행 계획을 지역 변수로 소유하고 한 정리 경로에서 해제한다.
- `setenv`, `unsetenv`, `chdir`, `pipe`, `fork`, `dup2`, `open`, `execvp`, `waitpid`의 오류를 사용자 입력이 유발할 수 있다. assert/abort 대신 argv[0] 접두사와 errno 원인을 stderr로 보낸다.
- 출력 리다이렉션은 새 파일 생성·기존 내용 삭제·0600 권한을 적용한다. 부모 표준 입출력은 바꾸지 않는다.
- 자식의 SIGINT/SIGQUIT/SIGALRM은 기본 동작으로 복원한다. 부모는 SIGINT를 무시하고 `signal()`/`alarm()`으로 5초 SIGQUIT 창을 관리한다. 두 번째 SIGQUIT는 블로킹 입력/대기를 깨워 정상 흐름에서 정리한다.

## 이 코드에서 바꾸면 좋은 습관

각 malloc에 free를 붙이는 방식만으로는 부족하다. 객체의 소유자가 누구인지 먼저 정한다. argv와 파일 디스크립터는 C API의 계약을 종이에 적어 놓고 만든다. 파이프는 모든 단계를 시작한 다음 기다리고, 각 프로세스가 어느 끝을 닫는지 표로 확인한다. 사용자 오류와 프로그래머 오류를 분리한다. 신호 핸들러에서 일반 프로그램 코드를 호출하지 않는다.

## corrected 파일 대응

| 파일 | 기존 코드와의 관계 |
|---|---|
| `ish.c` | main, singleLine, 시작 파일/입력 처리 |
| `token.c/h` | 기존 makeToken/freeToken과 소유권 |
| `lexer.c/h` | 기존 DFA lexLine |
| `parser.c/h` | synLine, commandType, 안전한 argv/파이프 계획 |
| `execute.c/h` | 기존 fork/exec·builtin·파이프 실행, 리다이렉션 |
| `signals.c/h` | 기존 SIGQUIT/SIGALRM 처리의 안전한 분리 |
| `dynarray.c/h` | 제공받은 구현을 그대로 사용 |
| `makefile` | 모듈별 object/dependency와 전체 빌드 |

## 검증

빈 줄/공백/EOF/마지막 개행 없음, 빈/인접 따옴표, 3단 파이프, 200KB 생산 파이프, FD 한도 64에서 160회 파이프 반복, 리다이렉션 위치·권한·truncate, builtin 인자·환경·cd, HOME의 시작 파일·exit, 긴 입력 복구, 실행 실패 후 계속 실행을 확인한다. SIGINT 무시·자식 기본 동작, 두 번 SIGQUIT, 5초 경과 후 창 재설정, 자식 대기 중 두 번 SIGQUIT도 실제 프로세스로 검사한다.

```sh
./run.sh --test
./run.sh
```

명령은 `corrected`에서 실행한다. 과제 범위에 없는 단일 따옴표 해석, 변수 확장, 백그라운드 작업, glob, heredoc은 추가하지 않았다. 공식 `sampleish`는 보관본에 없어 PDF와 자체 테스트를 기준으로 검증했다.
