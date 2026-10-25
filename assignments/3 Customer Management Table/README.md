# 3 Customer Management Table

과제 PDF의 제목을 사용한 학습용 정리본이다.

- `old/`: 기존 코드를 유지하고 오류 설명만 `WRONG:` 주석으로 추가했다. [리뷰](old/review.md)에서 원인·개념·수정 대응을 확인할 수 있다.
- `corrected/`: 기존 구현을 바탕으로 고친 버전. 새 수정 설명은 `CORRECTED:`로 구분한다.
- 과제 설명 PDF는 이 디렉터리에 보관했다.

```sh
cd "assignments/3 Customer Management Table/corrected"
./run.sh --test
./run.sh --demo
```

먼저 저장소 루트에서 `./assignments/setup.sh`로 환경을 준비한다. 일반 실행은 `./run.sh`이며, 인자가 필요한 프로그램에는 인자를 그대로 전달한다. 자세한 공용 환경·precept 실행법은 [상위 README](../README.md)를 참고한다.
