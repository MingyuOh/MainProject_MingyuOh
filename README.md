# Vision_Subject_MingyuOh
## 📙목차
- 📺 [실행 파일 경로](#-실행-파일-경로)
- 📝 [Logger concept](#-로그-컨셉)


## 📺 실행 파일 경로
실행 파일은 "EXE.zip"에 존재합니다.
- 압축 파일 내부
1. Include(lib 및 dll 헤더 파일)
2. Images(개발 시 사용했던 테스트 이미지)
3. x64(실행 파일 경로)
   <br><br>3-1. Release
   <br><br><br>3-1-1. Setting 폴더 -> Setting.ini - 폴더 내의 설정 파일(이미지 파일 경로, 커널 크기, 결과 이미지 저장 경로)
   <br><br>(* 실행 파일 실행 시, 이미지 파일 경로는 수정되어야 합니다.)
   3-1-2. Opencv.dll, Custom.dll, ImageObjectLib.lib - 라이브러리
   3-1-3. result.csv - 성능 비교 테스트 리포트
   3-1-4. image_blur.log - 로그 파일
3-2. Debug
   3-2-#. Release와 동일합니다.


## 📝 Logger concept
> - 데이터 구조:    
> &emsp;버퍼는 std::vector<std::pair<std::atomic<bool>, LogData>> 형태로 구현되어 있으며,
> &emsp;버퍼의 Key는 자료형 std::atomic<bool>을 사용하여 원자성을 보장하고, 스레드 안전하게 구성되어 있습니다. 
> &emsp;버퍼 접근 시에는 읽기 인덱스(Read Index)와 쓰기 인덱스(Write Index)를 분리하여, 인덱스에 대한 동시 접근을 방지합니다.
> &emsp;Consumer 스레드를 별도로 생성하여 로그 데이터를 파일로 기록합니다.


> - 동작 방식:
> &emsp;해당 로그 시스템은 싱글톤 객체로 구현되어 전역에서 단일 인스턴스를 사용하며, 원형 큐(Ring Buffer) 형태로 동작합니다.
> &emsp;Producer-Consumer 패턴을 기반으로 로그 데이터를 처리합니다.
> &emsp;메인 스레드 또는 외부 스레드에서 로그 데이터를 Producer 함수를 통해 버퍼에 추가합니다.
> &emsp;Consumer 스레드는 대기 상태로 있다가, 읽기 인덱스에 데이터가 추가되면 이를 감지하여 로그를 처리하고, 다음 인덱스로 이동하여 다시 대기합니다.
 
