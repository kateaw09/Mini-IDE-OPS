void DropF() {
  // SetFront(25);
  servoWrite(CH_SERVO_FRONT, ServoFront_Drop);
  delay(200);
  servoWrite(CH_SERVO_FRONT, ServoFront_Lock);
}

void DropB() {
  // SetBack(25);
  servoWrite(CH_SERVO_BACK, ServoBack_Drop);
  delay(200);
  servoWrite(CH_SERVO_BACK, ServoBack_Lock);
}

void FF_DropF(int speed, float timer) {
  int min_speed = 10;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 0.5;         // KP
  float kd = 10.0;        // KD
  int ramp_up = 50;       // หุ่นยนต์จะเริ่มวิ่งจากความเร็วต่ำสุดไปที่ความเร็วสูงสุดภายในเวลาที่กำหนด
  int ramp_down = 0;      // หุ่นยนต์จะวิ่งจากความเร็วสูงสุดไปที่ความเร็วต่ำสุดภายในเวลาที่กำหนดก่อนที่จะหยุด
  int current_speed = min_speed;
  unsigned long timer_forward = millis();
  float previous_error = 0;
  while (1) {
    unsigned long elapsed_time = millis() - timer_forward;
    unsigned long remaining_time = timer - elapsed_time;
    if (elapsed_time <= ramp_up) {
      current_speed = min_speed + (float)elapsed_time / ramp_up * (max_speed - min_speed);
    } else if (remaining_time <= ramp_down) {
      current_speed = min_speed + (float)remaining_time / ramp_down * (max_speed - min_speed);
      if (current_speed < min_speed) current_speed = min_speed;
    } else {
      current_speed = max_speed;
      servoWrite(CH_SERVO_FRONT, ServoFront_Drop);
    }
    float error = current_degree - angleRead(YAW);
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    float derivative = error - previous_error;
    int pd_value = (error * kp) + (derivative * kd);
    if (pd_value > max_speed) pd_value = max_speed;
    else if (pd_value < -max_speed) pd_value = -max_speed;
    int speed_left = current_speed + pd_value;
    int speed_right = current_speed - pd_value;
    motorWrite(speed_left, speed_left, speed_right, speed_right);
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
    previous_error = error;
  }
}