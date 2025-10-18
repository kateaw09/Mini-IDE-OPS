void calibrateMinMax() {
  unsigned long timer = millis();
  for (byte i = 0; i < 8; i++) {
    sensor_min[i] = 1023;
    sensor_max[i] = 0;
  }
  motorWrite(1000, 1000, 1000, 1000);
  while (1) {
    for (byte i = 0; i < 8; i++) {
      int sensor_value = adcRead(i);
      if (sensor_value < sensor_min[i]) sensor_min[i] = sensor_value;
      if (sensor_value > sensor_max[i]) sensor_max[i] = sensor_value;
    }
    if (millis() - timer > 2000) {
      motorStop();
      Serial.print("MIN: ");
      for (byte i = 0; i < 8; i++) {
        Serial.print(sensor_min[i]);
        if (i < 7) Serial.print(", ");
        else Serial.println();
      }
      Serial.print("MAX: ");
      for (byte i = 0; i < 8; i++) {
        Serial.print(sensor_max[i]);
        if (i < 7) Serial.print(", ");
        else Serial.println();
      }
      break;
    }
  }
}

void readLine() {
  for (byte i = 0; i < 8; i++) {
    line_value[i] = map(adcRead(i), sensor_min[i], sensor_max[i], 0, 100);
    if (line_value[i] < 0) line_value[i] = 0;
    else if (line_value[i] > 100) line_value[i] = 100;
  }
}

void ShowValue_Sensor() {
  lcdClear();
  while (1) {
    // readLine();
    int num_sensor = 8;
    int sensor_values[num_sensor];          // สร้างตัวแปรแบบ Array เพื่อเก็บค่า Sensor
    for (int i = 0; i < num_sensor; i++) {  // วนลูปอ่านค่า Sensor ทั้งหมด 6 ตัว
      // sensor_values[i] = line_value[i];     // นำค่า Sensor แต่ละตัวเก็บไว้ในตัวแปร
      sensor_values[i] = adcRead(i);  // นำค่า Sensor แต่ละตัวเก็บไว้ในตัวแปร
    }
    lcdDrawChart(sensor_values, num_sensor);  // แสดงกราฟค่าของ Sensor ทั้งหมด 8 ตัว การใช้งาน: lcdDrawChart(ค่าเซ็นเซอร์ [ตัวแปรแบบ Array], จำนวน Sensor)
  }
}

void config() {
  servoWrite(CH_SERVO_FRONT, ServoFront_Lock);
  servoWrite(CH_SERVO_BACK, ServoBack_Lock);
  lcdClear();

  lcdSetTextSize(2);
  lcdPrintText(10, 10, "SERVO VALUE", 0xf800);
  lcdSetTextSize(1);
  lcdPrintText(25, 40, "ServoFront Lock:", 0xffff);
  lcdPrintText(25, 60, "ServoFront Drop:", 0xffff);
  lcdPrintText(25, 80, " ServoBack Lock:", 0xffff);
  lcdPrintText(25, 100, " ServoBack Drop:", 0xffff);
  lcdPrintText(125, 40, "       ", 0xffff);
  lcdPrintText(125, 40, String(ServoFront_Lock), 0xffff);
  lcdPrintText(125, 60, "       ", 0xffff);
  lcdPrintText(125, 60, String(ServoFront_Drop), 0xffff);
  lcdPrintText(125, 80, "       ", 0xffff);
  lcdPrintText(125, 80, String(ServoBack_Lock), 0xffff);
  lcdPrintText(125, 100, "       ", 0xffff);
  lcdPrintText(125, 100, String(ServoBack_Drop), 0xffff);
}

void STOP() {
  motorStop();
  while (!switchRead(A) && !switchRead(B));
  delay(500);
}