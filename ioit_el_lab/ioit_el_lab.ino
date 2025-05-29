/* ---------------  Blynk template credentials  --------------- */
#define BLYNK_TEMPLATE_ID "TEMPLATE ID"         // <-- copy from blynk.cloud
#define BLYNK_TEMPLATE_NAME "TEMPLATE NAME"
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

/* ---------------  Wi-Fi creds (edit!)  --------------- */
char ssid[] = "ssid";
char pass[] = "passwd";                 // <-- change in real code



/* ---------------  Libraries  --------------- */
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>

void sendGasEvent(int);
/* ---------------  Hardware pins  --------------- */
constexpr uint8_t GAS_PIN = 36;   // MQ-series analog out (ADC1_CH6)
constexpr uint8_t FAN_PIN = 25;   // drives relay / MOSFET
const char* SERVER_URL = "http://192.168.0.105:5000/api/gas";
const long gmtoffset_sec = 19800;
const int daylightoffset_sec =0;
/* ---------------  Parameters  --------------- */
const int GAS_THRESHOLD = 1000;    // tweak after calibration (0-4095 ADC)
BlynkTimer timer;

/* ---------  state flags  -------- */
bool userOverride   = false;      // true while dashboard switch is ON
bool fanState       = false;      // true = fan currently running
bool gasDetect      = false;
bool leakLatched    = false;

/* === callback: user toggles dashboard switch (V2) ============= */
BLYNK_WRITE(V2)
{
  double v = param.asInt();          // 1.0 or 0.0
  userOverride = (v == 1);            // ON ⇒ force fan
}

/* === periodic task: read sensor, decide fan, push to Blynk ==== */
void sampleAndPublish()
{
  Serial.print("hi\n");
  int gasRaw = analogRead(GAS_PIN);         // 0-4095
  Blynk.virtualWrite(V0, gasRaw);           // live chart / gauge

  /* --- decide fan action --- */
  if (userOverride) {                       // user switch ON
    fanState = true;
  } else {
    fanState = false;
    gasDetect = (gasRaw > GAS_THRESHOLD);    // auto logic
  }

  digitalWrite(FAN_PIN, fanState || gasDetect ? LOW : HIGH);
  Blynk.virtualWrite(V1, gasDetect ? "Gas Detected" : "Safe Level");   // status LED/label

  bool leakNow = (gasRaw > GAS_THRESHOLD);
  if(leakNow && !leakLatched){
    sendGasEvent(gasRaw);
    leakLatched = true;
  }
  if (!leakNow) leakLatched = false;   // reset latch
}

/*---------helper----------*/
void sendGasEvent(int gasRaw){
  time_t now= time(nullptr);
  if(now < 10000) return;

  struct tm timeinfo;
  if (!localtime_r(&now, &timeinfo)) return;

  char isoTime[25];  // enough for "YYYY-MM-DDTHH:MM:SSZ"
  strftime(isoTime, sizeof(isoTime), "%Y-%m-%dT%H:%M:%S", &timeinfo);

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type","application/json");

  StaticJsonDocument<128> doc;
  doc["value"] =gasRaw;
  doc["ts"] = isoTime;

  String payload;
  serializeJson(doc,payload);

  int code = http.POST(payload);
  Serial.printf("POST /api/gas -> %d\n",code);

  if(code > 200){
    String resp = http.getString();
    Serial.println(resp);
  }
  http.end();
}

/* ========================  setup  ============================ */
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("STARTING...");
  
  pinMode(GAS_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);

  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Connected to Blynk!");
  timer.setInterval(2000L, sampleAndPublish);      // every 2 s
  configTime(gmtoffset_sec, daylightoffset_sec ,"pool.ntp.org");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for time sync...");
    delay(1000);
  }
  Serial.println("Time synced!");
}

/* ========================  loop  ============================= */
void loop()
{
  Blynk.run();
  timer.run();
}
