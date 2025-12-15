#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

//========WiFi 설정====================

char STASSID[] = "codelab";        // 공유기 SSID
char STAPSK[] = "20380800";       // 공유기 비밀번호

WiFiServer server(80);

//============ PCAM1_REV_CH6M1_FWD_CH5 모터 채널 매핑 (Yahboom 4륜)==================
//==============================
// 모터 매핑 (Yahboom 원본 그대로)
// M1_FWD_CH~M4_REV_CH 채널 사용
//==============================

#define M1_FWD_CH 8  // Right Rear Forward
#define M1_REV_CH 9  // Right Rear Reverse
#define M2_FWD_CH 10 // Right Front Forward
#define M2_REV_CH 11 // Right Front Reverse
#define M3_FWD_CH 12 // Left Front Forward  <-- 주의: M3_FWD_CH(Fwd), M3_REV_CH(Rev)
#define M3_REV_CH 13 // Left Front Reverse
#define M4_FWD_CH 14 // Left Rear Forward   <-- 주의: M4_REV_CH(Fwd), M4_FWD_CH(Rev)
#define M4_REV_CH 15 // Left Rear Reverse

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);



void Clear_All_PWM()
{
  for (int i = 0; i < 16; i++)
  {
    pwm.setPWM(i, 0, 0);
  }
}

// 전진
void advance(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, Speed); // Right front wheel Forward 
  pwm.setPWM(M2_REV_CH, 0, 0);
  pwm.setPWM(M1_FWD_CH,  0, 0); // Right rear wheel Forward 
  pwm.setPWM(M1_REV_CH,  0, Speed);

  pwm.setPWM(M3_FWD_CH, 0, Speed); // Left front wheel Forward 
  pwm.setPWM(M3_REV_CH, 0, 0);
  pwm.setPWM(M4_REV_CH, 0, 0); // Left rear wheel Forward 
  pwm.setPWM(M4_FWD_CH, 0, Speed);
}

// 정지
void brake()
{
  pwm.setPWM(M1_FWD_CH,  0, 0);
  pwm.setPWM(M1_REV_CH,  0, 0);
  pwm.setPWM(M2_FWD_CH, 0, 0);
  pwm.setPWM(M2_REV_CH, 0, 0);
  pwm.setPWM(M3_REV_CH, 0, 0);
  pwm.setPWM(M3_FWD_CH, 0, 0);
  pwm.setPWM(M4_FWD_CH, 0, 0);
  pwm.setPWM(M4_REV_CH, 0, 0);
}

// 좌회전 (오른쪽 바퀴만 전진)
void left(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, Speed);     // Right front wheel Forward
  pwm.setPWM(M2_REV_CH, 0, 0);
  pwm.setPWM(M1_FWD_CH,  0, Speed);     // Right rear wheel Forward
  pwm.setPWM(M1_REV_CH,  0, 0);      

  pwm.setPWM(M3_FWD_CH, 0, 0);         // Left wheels Stop
  pwm.setPWM(M3_REV_CH, 0, 0);
  pwm.setPWM(M4_REV_CH, 0, 0);
  pwm.setPWM(M4_FWD_CH, 0, 0);
}

// 우회전 (왼쪽 바퀴만 전진)
void right(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, 0);        // Right wheels Stop
  pwm.setPWM(M2_REV_CH, 0, 0);
  pwm.setPWM(M1_FWD_CH,  0, 0);
  pwm.setPWM(M1_REV_CH,  0, 0);

  pwm.setPWM(M3_FWD_CH, 0, Speed);    // Left front wheel Forward
  pwm.setPWM(M3_REV_CH, 0, 0);
  pwm.setPWM(M4_REV_CH, 0, Speed);    // Left rear wheel Forward
  pwm.setPWM(M4_FWD_CH, 0, 0);
}

// 제자리 좌회전 (왼쪽 뒤로, 오른쪽 앞으로)
void spin_left(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, Speed);      // Right side Forward
  pwm.setPWM(M2_REV_CH, 0, 0);
  pwm.setPWM(M1_FWD_CH,  0, Speed);      // Right rear Forward
  pwm.setPWM(M1_REV_CH,  0, 0);

  pwm.setPWM(M3_FWD_CH, 0, 0);
  pwm.setPWM(M3_REV_CH, 0, Speed);      // Left front Back
  pwm.setPWM(M4_REV_CH, 0, 0);
  pwm.setPWM(M4_FWD_CH, 0, Speed);      // Left rear Back
}

// 제자리 우회전 (왼쪽 앞으로, 오른쪽 뒤로)
void spin_right(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, 0);
  pwm.setPWM(M2_REV_CH, 0, Speed);    // Right front Back
  pwm.setPWM(M1_FWD_CH,  0, 0);
  pwm.setPWM(M1_REV_CH,  0, Speed);    // Right rear Back

  pwm.setPWM(M3_FWD_CH, 0, Speed);    // Left front Forward
  pwm.setPWM(M3_REV_CH, 0, 0);
  pwm.setPWM(M4_REV_CH, 0, Speed);    // Left rear Forward
  pwm.setPWM(M4_FWD_CH, 0, 0);
}

// 후진
void back(int Speed)
{
  Speed = map(Speed, 0, 255, 0, 4095);
  pwm.setPWM(M2_FWD_CH, 0, 0);
  pwm.setPWM(M2_REV_CH, 0, Speed); // Right front wheel Reverse
  pwm.setPWM(M1_FWD_CH,  0, 0);
  pwm.setPWM(M1_REV_CH,  0, Speed); // Right rear wheel Reverse

  pwm.setPWM(M3_FWD_CH, 0, 0);
  pwm.setPWM(M3_REV_CH, 0, Speed);  // Left front wheel Reverse
  pwm.setPWM(M4_REV_CH, 0, 0);
  pwm.setPWM(M4_FWD_CH, 0, Speed); // Left rear wheel Reverse
}

//==============================
// 주행 상태 관리 (enum 값만 사용)
//==============================
enum {
  DIR_STOP = 0,
  DIR_FWD,
  DIR_BACK,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_SPINL,
  DIR_SPINR
};

uintM1_FWD_CH_t currentDir   = DIR_STOP;
int     currentSpeed = 0;          // 0~255 기준

const int MAX_SPEED       = 150;   // 최속 (255보다 작게, 배터리 보호)
const int STEP_SPEED      = 10;    // 가감속 단계
const int STEP_DELAY      = 30;    // 가감속 사이 딜레이(ms)
const int DIR_BRAKE_DELAY = 80;    // 방향 전환 시 브레이크 후 딜레이(ms)

//==============================
// 방향/속도 즉시 반영 함수
//==============================
void driveDirOnce(uintM1_FWD_CH_t dir, int spd)
{
  switch (dir) {
    case DIR_FWD:   advance(spd);   break;
    case DIR_BACK:  back(spd);      break;
    case DIR_LEFT:  left(spd);      break;
    case DIR_RIGHT: right(spd);     break;
    case DIR_SPINL: spin_left(spd); break;
    case DIR_SPINR: spin_right(spd);break;
    case DIR_STOP:
    default:        brake();        break;
  }
}

//==============================
// 부드러운 감속
//==============================
void smoothStop()
{
  if (currentDir == DIR_STOP || currentSpeed == 0) {
    brake();
    currentSpeed = 0;
    return;
  }

  for (int s = currentSpeed; s >= 0; s -= STEP_SPEED) {
    driveDirOnce(currentDir, s);
    delay(STEP_DELAY);
  }
  brake();
  currentDir   = DIR_STOP;
  currentSpeed = 0;
}

//==============================
// 안전한 방향 전환
//==============================
void safeChangeDir(uintM1_FWD_CH_t newDir, int targetSpeed)
{
  if (newDir == DIR_STOP) {
    smoothStop();
    return;
  }

  if (targetSpeed > MAX_SPEED) targetSpeed = MAX_SPEED;
  if (targetSpeed < 0)         targetSpeed = 0;

  // 같은 방향이면 가감속만
  if (currentDir == newDir) {
    int step = (targetSpeed > currentSpeed) ? STEP_SPEED : -STEP_SPEED;
    if (step == 0) return;

    for (int s = currentSpeed; 
         (step > 0) ? (s <= targetSpeed) : (s >= targetSpeed); 
         s += step) {
      driveDirOnce(newDir, s);
      delay(STEP_DELAY);
    }
    currentSpeed = targetSpeed;
    return;
  }

  // 다른 방향이면: 부드럽게 정지 후 변경
  smoothStop();
  delay(DIR_BRAKE_DELAY);

  for (int s = 0; s <= targetSpeed; s += STEP_SPEED) {
    driveDirOnce(newDir, s);
    delay(STEP_DELAY);
  }
  currentDir   = newDir;
  currentSpeed = targetSpeed;
}

//==============================
// 웹 UI
//==============================
void sendUI(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-M1_FWD_CH");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name='viewport' content='width=device-width'>");
  client.println("<title>WSAD RC CAR CONTROL</title>");
  client.println("<style>");
  client.println("body{text-align:center;font-family:sans-serif;margin-top:40px;}");
  client.println("button{font-size:22px;padding:M2_FWD_CHpx 20px;margin:M1_FWD_CHpx;}");
  client.println("</style>");
  client.println("<script>");
  client.println("let pressed = {};");  // 키 상태 기록

  client.println("function sendCmd(c){");
  client.println("  fetch('/cmd?c='+c).catch(e=>console.log(e));");
  client.println("}");

  // keydown: 처음 눌렸을 때만 명령 전송
  client.println("document.addEventListener('keydown', function(e){");
  client.println("  const k = e.key.toLowerCase();");
  client.println("  if (['w','a','s','d','q','e'].includes(k)) {");
  client.println("    if (!pressed[k]) {");
  client.println("      pressed[k] = true;");
  client.println("      let cmd = 'X';");
  client.println("      if (k=='w') cmd='W';");
  client.println("      if (k=='s') cmd='S';");
  client.println("      if (k=='a') cmd='A';");
  client.println("      if (k=='d') cmd='D';");
  client.println("      if (k=='q') cmd='Q';");
  client.println("      if (k=='e') cmd='E';");
  client.println("      sendCmd(cmd);");
  client.println("    }");
  client.println("  } else if (e.key === ' ') {");
  client.println("    sendCmd('X');");
  client.println("  }");
  client.println("});");

  // keyup: 방향키 떼면 항상 STOP
  client.println("document.addEventListener('keyup', function(e){");
  client.println("  const k = e.key.toLowerCase();");
  client.println("  if (['w','a','s','d','q','e'].includes(k)) {");
  client.println("    pressed[k] = false;");
  client.println("    sendCmd('X');");
  client.println("  }");
  client.println("});");

  client.println("</script>");
  client.println("</head><body>");
  client.println("<h2>WSAD RC CAR CONTROL</h2>");
  client.println("<p>키를 누르고 있는 동안만 움직이고, 떼면 부드럽게 정지합니다.</p>");
  client.println("<p>W/S/A/D : 전/후/좌/우, Q/E : 제자리 회전, SPACE : 긴급 정지</p>");
  client.println("<div>");
  client.println("<button onclick=\"sendCmd('W')\">Forward (W)</button><br>");
  client.println("<button onclick=\"sendCmd('A')\">Left (A)</button>");
  client.println("<button onclick=\"sendCmd('X')\">STOP (SPACE)</button>");
  client.println("<button onclick=\"sendCmd('D')\">Right (D)</button><br>");
  client.println("<button onclick=\"sendCmd('S')\">Backward (S)</button><br>");
  client.println("<button onclick=\"sendCmd('Q')\">Spin Left (Q)</button>");
  client.println("<button onclick=\"sendCmd('E')\">Spin Right (E)</button>");
  client.println("</div>");
  client.println("</body></html>");
}

//==============================
// Setup
//==============================
void setup() {
  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(60);  // Yahboom 기본값
  Clear_All_PWM();
  brake();

  WiFi.begin(STASSID, STAPSK);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

//==============================
// Loop (웹서버 루프를 더 안정적으로 수정)
//==============================
void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New client");

  String requestLine = "";
  unsigned long startTime = millis();

  // 첫 줄(요청 라인)만 '\n' 나올 때까지 읽기
  while (client.connected() && (millis() - startTime < 2000)) {
    if (client.available()) {
      char c = client.read();
      if (c == '\r') {
        // 무시
      } else if (c == '\n') {
        break; // 첫 줄 끝
      } else {
        requestLine += c;
      }
    }
  }

  Serial.print("Request: ");
  Serial.println(requestLine);

  // 나머지 헤더 버리기
  while (client.available()) client.read();

  // 명령 처리: GET /cmd?c=...
  if (requestLine.startsWith("GET /cmd?")) {
    int idx = requestLine.indexOf("c=");
    char c = requestLine.charAt(idx + 2);

    switch (c) {
      case 'W': safeChangeDir(DIR_FWD,   MAX_SPEED); break;
      case 'S': safeChangeDir(DIR_BACK,  MAX_SPEED); break;
      case 'A': safeChangeDir(DIR_LEFT,  MAX_SPEED); break;
      case 'D': safeChangeDir(DIR_RIGHT, MAX_SPEED); break;
      case 'Q': safeChangeDir(DIR_SPINL, MAX_SPEED); break;
      case 'E': safeChangeDir(DIR_SPINR, MAX_SPEED); break;
      case 'X': safeChangeDir(DIR_STOP,  0);         break;
      default:  break;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("OK");
    client.stop();
    Serial.println("cmd response sent");
    return;
  }

  // 그 외 요청 (/, /favicon.ico 등) : UI 전송
  sendUI(client);
  client.stop();
  Serial.println("UI sent");
}
