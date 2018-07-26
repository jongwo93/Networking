HANGMAN GAME :
----------------------------------------------------------------------------------------------
자바 언어와 소켓 프로그래밍을 활용하여 행맨 게임을 만들었다.
게임 서버를 열면 플레이어가 서버와 연결하여 행맨 게임을 할 수 있다.
스레드를 이용하며 한 서버에서 동시에 최대 3명의 플레이어가 게임을 할 수 있다.
다음 계획은 멀티 플레이어 기능을 만드는 것이다. (서버가 두 명의 플레이어 게임을 컨트롤)

Instructions :

'Javac Server.java' to compile
'Java Server (PortNumber)' ex: Java Server 8080  to specify the argument

Then, we need to obtain IP address (host) of server for anyone who wants to play the game.
For Mac OS devices, 
    - command + s 
    - type network utility
    - get IP address from 'info' tab

For Windows devices,
    - Win + R then cmd and press enter to get command prompt
    - type 'ipconfig'
    - obtain IPv4 Address

'Javac Player.java' to compile
'Java Player (Host) (PortNumber)' ex: Java Player 127.0.0.1 8080 to specify arguments

You will be instructed from there to play the game of Hangman.
Enjoy!

Jongwoo Jang

In progress, multiple players handled concurrently. 
----------------------------------------------------------------------------------------------


SIMPLE ATM :
----------------------------------------------------------------------------------------------
간단한 소켓 프로그래밍 예제.

ATM의 단순 업무 처리를 소켓 프로그래밍을 통해 구현.
C 언어로 작성
타임아웃 기능 구현 - 정해진 시간(60초) 안에 처리 할 수 있는 횟 수: 3
클라이언트가 요청 서버로 패킷 전달
서버가 처리 후 클라이언트로 피드백 패킷 전달
----------------------------------------------------------------------------------------------


