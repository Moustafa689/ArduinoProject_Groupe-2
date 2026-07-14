// 1. Include the Libraries
#include <IRremote.hpp>
#include <IskakINO_LiquidCrystal_I2C.h>
#include <DHT.h>

// 2. Define all the Pins
int IR_PIN = 2;    
int ECHO_PIN = 3;  
int TRIG_PIN = 4;
int WATER_PIN = A8; 
int LIGHT_PIN = A12;  // Photoresistor Pin
int RAIN_PIN = A10;   // Raindrop Sensor Pin
int SOIL_PIN = A1;    // NEW: Soil Moisture Sensor Pin
int GREEN_LED = 10;
int RED_LED = 11;
int BUZZER = 12;      // AC Passive Buzzer Pin
int DHT_PIN = 13;
int FAN_RELAY = 53;   // Fan Relay Signal Pin

// 3. Setup the LCD and DHT sensors
LiquidCrystal_I2C lcd(16, 2); 
DHT dht(DHT_PIN, DHT11);

// 4. Variables for our Passcode System
String correctPassword = "#123";
String enteredPassword = "";

// 5. System Modes
// Mode 0 = Locked & Armed, Mode 1 = Gate Open (10s), Mode 2 = Post-Open Armed, Mode 3 = Alarm!
int systemMode = 0; 

// 6. Timers
unsigned long timerStart = 0;
unsigned long screenTimer = 0;
unsigned long blinkTimer = 0;
unsigned long fanTimer = 0; 
unsigned long serialTimer = 0; 
unsigned long alarmScreenTimer = 0; 

// Timers to manually oscillate the AC buzzer without crashing the IR Remote timer
unsigned long buzzerMicros = 0;
bool buzzerPinState = false;
bool shouldSound = false; 

// Screen Rotation State: 0 = Climate, 1 = Water Level, 2 = Light & Rain, 3 = Soil Moisture
int screenPage = 0; 
bool alarmToggleState = false; 

// 7. Threshold Settings & Noise Filtering
int intruderDistance = 5; 
int consecutiveTriggers = 0; 

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(WATER_PIN, INPUT); 
  pinMode(LIGHT_PIN, INPUT); 
  pinMode(RAIN_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT); // NEW: Set soil pin as Input
  pinMode(FAN_RELAY, OUTPUT); 
  
  IrReceiver.begin(IR_PIN);
  dht.begin();
  
  lcd.begin();         
  lcd.backlight();
  
  // BOOT WARM-UP & CALIBRATION PERIOD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Booting...");
  
  for (int i = 3; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Arming in: "); lcd.print(i); lcd.print("s ");
    getDistance(); 
    delay(1000);
  }
  
  lockGreenhouse();
}

void loop() {
  checkRemote();
  unsigned long currentTime = millis();

  // ------------------------------------------------------------------
  // Global Serial Monitor Telemetry (Runs every 2 seconds)
  // ------------------------------------------------------------------
  if (currentTime - serialTimer >= 2000) {
    serialTimer = currentTime;
    
    float currentTemp = getTemperature();
    float currentHum = getHumidity();
    int waterLevel = getWaterLevel();
    int lightPercentage = getLightLevel();
    int rainPercentage = getRainLevel();
    int soilPercentage = getSoilMoisture(); // NEW: Fetch soil telemetry
    
    Serial.println("--- GREENHOUSE SYSTEM STATUS ---");
    Serial.print("Active Mode: "); Serial.println(systemMode);
    
    if (!isnan(currentTemp) && !isnan(currentHum)) {
      Serial.print("Temperature: "); Serial.print(currentTemp, 1); Serial.println(" C");
      Serial.print("Humidity   : "); Serial.print(currentHum, 1); Serial.println(" %");
    } else {
      Serial.println("Temperature/Humidity: Error reading DHT sensor!");
    }
    
    Serial.print("Water Level: "); Serial.print(waterLevel);
    if (waterLevel == 0)      Serial.println(" (DRY)");
    else if (waterLevel == 1) Serial.println(" (LOW)");
    else if (waterLevel == 2) Serial.println(" (MED)");
    else if (waterLevel == 3) Serial.println(" (HIGH)");
    
    Serial.print("Light Level: "); Serial.print(lightPercentage); Serial.println(" %");
    Serial.print("Rain Fall  : "); Serial.print(rainPercentage); Serial.println(" %");
    Serial.print("Soil Moist : "); Serial.print(soilPercentage); Serial.println(" %"); // NEW: Print to Serial
    Serial.println("--------------------------------\n");
  }

  // ------------------------------------------------------------------
  // Automatic Fan Temperature Control (Checks every 2 seconds)
  // ------------------------------------------------------------------
  if (currentTime - fanTimer >= 2000) {
    fanTimer = currentTime;
    float currentTemp = getTemperature();
    
    if (!isnan(currentTemp)) {
      if (currentTemp > 30) {
        digitalWrite(FAN_RELAY, LOW);   
      } else {
        digitalWrite(FAN_RELAY, HIGH);  
      }
    }
  }

  // Security System Logic
  if (systemMode == 0) {
    float distance = getDistance();
    if (distance > 0.5 && distance < intruderDistance) { 
      consecutiveTriggers++;
      if (consecutiveTriggers >= 3) {
        triggerAlarm();
      }
    } else {
      consecutiveTriggers = 0; 
    }
  } 
  else if (systemMode == 1) { 
    digitalWrite(GREEN_LED, HIGH);
    updateScreen(currentTime);

    if (currentTime - timerStart >= 10000) {
      systemMode = 2; 
      digitalWrite(GREEN_LED, LOW); 
    }
  } 
  else if (systemMode == 2) { 
    updateScreen(currentTime); 

    float distance = getDistance();
    if (distance > 0.5 && distance < intruderDistance) { 
      consecutiveTriggers++;
      if (consecutiveTriggers >= 3) {
        triggerAlarm();
      }
    } else {
      consecutiveTriggers = 0;
    }
  } 
  else if (systemMode == 3) { 
    if (currentTime - blinkTimer >= 250) {
      blinkTimer = currentTime; 
      alarmToggleState = !alarmToggleState; 
      
      digitalWrite(RED_LED, alarmToggleState);
      shouldSound = alarmToggleState; 
    }

    if (currentTime - alarmScreenTimer >= 300) {
      alarmScreenTimer = currentTime;
      float distance = getDistance();
      
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      if(distance == 999) {
        lcd.print("Out of Range   ");
      } else {
        lcd.print(distance, 1);
        lcd.print(" cm    "); 
      }
    }

    if (shouldSound == true) {
      unsigned long currentMicros = micros();
      if (currentMicros - buzzerMicros >= 500) { 
        buzzerMicros = currentMicros;
        buzzerPinState = !buzzerPinState;
        digitalWrite(BUZZER, buzzerPinState);
      }
    } else {
      digitalWrite(BUZZER, LOW); 
    }
  }
}

// ==========================================
//          CORE SYSTEM FUNCTIONS
// ==========================================

void checkRemote() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (systemMode == 0 || systemMode == 3) {
      switch (code) {
        case 0xE916FF00: addNumber('1'); break;
        case 0xE619FF00: addNumber('2'); break;
        case 0xF20DFF00: addNumber('3'); break;
        case 0xF30CFF00: addNumber('4'); break;
        case 0xE718FF00: addNumber('5'); break;
        case 0xA15EFF00: addNumber('6'); break;
        case 0xF708FF00: addNumber('7'); break;
        case 0xE31CFF00: addNumber('8'); break;
        case 0xA55AFF00: addNumber('9'); break;
        case 0xAD52FF00: addNumber('0'); break;
        case 0xB54AFF00: addNumber('#'); break;
        
        case 0xBD42FF00: 
          enteredPassword = "";
          lcd.setCursor(6, 1);
          lcd.print("          "); 
          break;
          
        case 0xBF40FF00: 
          verifyPassword();
          break;
      }
    }
    IrReceiver.resume(); 
  }
}

void addNumber(char num) {
  enteredPassword += num;
  lcd.setCursor(6, 1); 
  lcd.print(enteredPassword);
}

void verifyPassword() {
  if (enteredPassword == correctPassword) {
    enteredPassword = "";
    shouldSound = false; 
    consecutiveTriggers = 0;
    
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW); 
    
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Code Matched!");
    lcd.setCursor(0, 1); 
    lcd.print("Opening Gate...");
    delay(1500); 
    lcd.clear();
    
    timerStart = millis(); 
    systemMode = 1; 
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Denied!");
    delay(1000);
    
    enteredPassword = "";
    lcd.clear();
    if (systemMode == 3) {
      lcd.setCursor(0, 1);
      lcd.print("Code: ");
    } else {
      lockGreenhouse();
    }
  }
}

void lockGreenhouse() {
  systemMode = 0;
  enteredPassword = "";
  shouldSound = false;
  consecutiveTriggers = 0;
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(FAN_RELAY, LOW); 
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Gate Code:"); 
}

void triggerAlarm() {
  systemMode = 3; 
  enteredPassword = ""; 
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Code: "); 
}

void updateScreen(unsigned long currentTime) {
  // Page Swapper Strategy: Cycles through 4 screen pages every 5 seconds
  if (currentTime - screenTimer >= 5000) {
    screenTimer = currentTime;
    screenPage++;
    if(screenPage > 3) screenPage = 0; // MODIFIED: Wraps at page 3 instead of page 2
    lcd.clear();
  }

  if (screenPage == 0) {
    float temp = getTemperature();
    float hum = getHumidity();
    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.print(" C   ");
    
    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.print(hum, 1);
    lcd.print(" %   ");
  } 
  else if (screenPage == 1) {
    int waterLevel = getWaterLevel();
    
    lcd.setCursor(0, 0);
    lcd.print("Water Reservoir");
    lcd.setCursor(0, 1);
    lcd.print("Level: ");
    
    if (waterLevel == 0)      lcd.print("DRY   ");
    else if (waterLevel == 1) lcd.print("LOW   ");
    else if (waterLevel == 2) lcd.print("MED   ");
    else if (waterLevel == 3) lcd.print("HIGH  ");
  } 
  else if (screenPage == 2) {
    int lightPercentage = getLightLevel();
    int rainPercentage = getRainLevel();
    
    lcd.setCursor(0, 0);
    lcd.print("Light: ");
    lcd.print(lightPercentage);
    lcd.print("%    ");
    
    lcd.setCursor(0, 1);
    lcd.print("Rain:  ");
    lcd.print(rainPercentage);
    lcd.print("%    ");
  }
  else if (screenPage == 3) {
    // ---- NEW PAGE 3: SOIL MOISTURE TELEMETRY ----
    int soilPercentage = getSoilMoisture();
    
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture");
    lcd.setCursor(0, 1);
    lcd.print("Level: ");
    lcd.print(soilPercentage);
    lcd.print("%    ");
  }
}

// ==========================================
//          HARDWARE SENSOR FUNCTIONS
// ==========================================

float getTemperature() { return dht.readTemperature(); }
float getHumidity() { return dht.readHumidity(); }

int getWaterLevel() {
  int rawValue = analogRead(WATER_PIN); 
  if (rawValue < 50)   return 0; 
  if (rawValue < 300)  return 1; 
  if (rawValue < 600)  return 2; 
  return 3; 
}

int getLightLevel() {
  int rawLight = analogRead(LIGHT_PIN);
  return map(rawLight, 0, 1023, 0, 100);
}

int getRainLevel() {
  int rawRain = analogRead(RAIN_PIN);
  int percentage = map(rawRain, 1023, 0, 0, 100);
  if(percentage < 0) percentage = 0;
  return percentage;
}

// NEW: Soil Moisture Logic Block
int getSoilMoisture() {
  int rawSoil = analogRead(SOIL_PIN);
  
  // CALIBRATION NOTE: 
  // 1023 represents total dryness in the air. 
  // 250 is an approximation of your sensor fully dipped in a cup of water.
  int percentage = map(rawSoil, 1023, 250, 0, 100);
  
  return constrain(percentage, 0, 100); 
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return 999; 
  
  return duration * 0.034 / 2;  
}