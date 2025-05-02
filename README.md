# Vision_Subject_MingyuOh
## 📙목차
- 🧑‍💻 [실행 파일 경로](#-실행-파일-경로)
- 📝 [Logger concept](#-로그-컨셉)

## 🧑‍💻 실행 파일 경로
실행 파일은 "EXE.zip"에 존재합니다.
- 압축 파일 내부
  1. Include(lib 및 dll 헤더 파일)
  2. Images(개발 시 사용했던 테스트 이미지)
  3. x64(실행 파일 경로)
     3-1. Release
       3-1-1. Setting 폴더 -> Setting.ini - 폴더 내의 설정 파일(이미지 파일 경로, 커널 크기, 결과 이미지 저장 경로)
             (* 실행 파일 실행 시, 이미지 파일 경로는 수정되어야 합니다.)
       3-1-2. Opencv.dll, Custom.dll, ImageObjectLib.lib - 라이브러리
       3-1-3. result.csv - 성능 비교 테스트 리포트
       3-1-4. image_blur.log - 로그 파일
     3-2. Debug
       3-2-#. Release와 동일합니다.

## 📝 Logger concept
> - Data structure:   
> &emsp;버퍼는 std::pair<automic bool, LogData>형 std::vector로 구현   
> &emsp;버퍼의 Key 자료형을 원자성을 가진 automic bool을 사용하여 Thread-Safe 하도록 구현   
> &emsp;버퍼에 접근하는 인덱스를 쓰기/읽기 인덱스(read index/write index)로 분리하여 인덱스 변수 동시 접근 방지   
> &emsp;Consumer thread를 생성하여 로그를 파일에 쓰도록 처리   

> - Logic:   
> &emsp;싱글톤 변수로 선언하여 전역 단일 객체로 사용하며 원형 큐로 사용   
> &emsp;Producer-Consumer 패턴 사용하여 로그 데이터 처리   
> &emsp;메인 스레드 또는 외부 스레드에서 로그를 Producer 함수로 버퍼에 추가   
> &emsp;Consumer thread는 대기하며 읽을 인덱스를 감시하고 있다가 데이터가 추가되면 데이터 처리 후 다음 인덱스에서 대기   
