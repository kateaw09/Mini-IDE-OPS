#define slowPrecent 10

void FF(int speed) {
  int base_speed = speed;  // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = 80;      // ความเร็วสูงสุด
  float kp = 1.8;          // KP
  float kd = 10.0;         // KD
  float error = current_degree - angleRead(YAW);
  if (error > 180) error -= 360;
  else if (error < -180) error += 360;
  float derivative = error - previous_error_forward;
  int pd_value = (error * kp) + (derivative * kd);
  if (pd_value > max_speed) pd_value = max_speed;
  else if (pd_value < -max_speed) pd_value = -max_speed;
  int speed_left = base_speed + pd_value;
  int speed_right = base_speed - pd_value;
  motorWrite(speed_left, speed_left, speed_right, speed_right);
  previous_error_forward = error;
}

void FF(int speed, float timer) {
  int min_speed = 10;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
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

void FF(int speed, float timer, char select) {
  int min_speed = 10;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
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
  delay(delayALL);

  if (select == 'L') {
    spinDegree(-90);
  } else if (select == 'R') {
    spinDegree(90);
  } else if (select == 'l') {
    turnDegreeFront(-90);
  } else if (select == 'r') {
    turnDegreeFront(90);
  }
  motorStop();
  delay(delayALL);
}

void FF(int speed, float timer, char select, float turnTimer) {
  int min_speed = 15;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
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
  if (select == 'L') {
    LL(turnTimer);
  } else if (select == 'R') {
    RR(turnTimer);
  }
  delay(delayALL);
}

void BB(int speed) {
  int base_speed = speed;  // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = 80;      // ความเร็วสูงสุด
  float kp = 2.8;          // KP
  float kd = 10.0;         // KD
  float error = current_degree - angleRead(YAW);
  if (error > 180) error -= 360;
  else if (error < -180) error += 360;
  float derivative = error - previous_error_forward;
  int pd_value = (error * kp) + (derivative * kd);
  if (pd_value > max_speed) pd_value = max_speed;
  else if (pd_value < -max_speed) pd_value = -max_speed;
  int speed_left = base_speed - pd_value;
  int speed_right = base_speed + pd_value;
  motorWrite(-speed_left, -speed_left, -speed_right, -speed_right);
  previous_error_forward = error;
}

void BB(int speed, float timer) {
  int min_speed = 15;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
    float error = current_degree - angleRead(YAW);
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    float derivative = error - previous_error;
    int pd_value = (error * kp) + (derivative * kd);
    if (pd_value > max_speed) pd_value = max_speed;
    else if (pd_value < -max_speed) pd_value = -max_speed;
    int speed_left = current_speed - pd_value;
    int speed_right = current_speed + pd_value;
    motorWrite(-speed_left, -speed_left, -speed_right, -speed_right);
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
    previous_error = error;
  }
}

void BB(int speed, float timer, char select) {
  int min_speed = 15;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
    float error = current_degree - angleRead(YAW);
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    float derivative = error - previous_error;
    int pd_value = (error * kp) + (derivative * kd);
    if (pd_value > max_speed) pd_value = max_speed;
    else if (pd_value < -max_speed) pd_value = -max_speed;
    int speed_left = current_speed - pd_value;
    int speed_right = current_speed + pd_value;
    motorWrite(-speed_left, -speed_left, -speed_right, -speed_right);
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
    previous_error = error;
  }

  if (select == 'L') {
    spinDegree(-90);
  }
  if (select == 'R') {
    spinDegree(90);
  }
  if (select == 'l') {
    turnDegreeBack(-90);
  }
  if (select == 'r') {
    turnDegreeBack(90);
  }
}

void BB(int speed, float timer, char select, float turnTimer) {
  int min_speed = 15;     // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = speed;  // ความเร็วสูงสุด
  float kp = 2.8;         // KP
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
    } else current_speed = max_speed;
    float error = current_degree - angleRead(YAW);
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    float derivative = error - previous_error;
    int pd_value = (error * kp) + (derivative * kd);
    if (pd_value > max_speed) pd_value = max_speed;
    else if (pd_value < -max_speed) pd_value = -max_speed;
    int speed_left = current_speed - pd_value;
    int speed_right = current_speed + pd_value;
    motorWrite(-speed_left, -speed_left, -speed_right, -speed_right);
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
    previous_error = error;
  }

  if (select == 'L') {
    LL(turnTimer);
  }
  if (select == 'R') {
    RR(turnTimer);
  }
  delay(delayALL);
}

void SetFront(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[0] <= 20 && line_value[3] <= 20) {
      motorStop();
      break;
    } else {
      if (line_value[0] <= 20) {
        motorWrite(1, -15);
        motorWrite(2, -15);
      } else {
        motorWrite(1, speed);
        motorWrite(2, speed);
      }
      if (line_value[3] <= 20) {
        motorWrite(3, -15);
        motorWrite(4, -15);
      } else {
        motorWrite(3, speed);
        motorWrite(4, speed);
      }
    }
  }
  delay(100);
  current_degree = angleRead(YAW) + 2;
}

void SetFront(int speed, float timer) {
  SetFront(speed);
  delay(10);
  motorTimeDegree(-speed, timer);
  delay(delayALL);
}

void SetFront(int speed, float timer, char select) {
  SetFront(speed, timer);
  if (select == 'L') {
    spinDegree(-90);
  }
  if (select == 'R') {
    spinDegree(90);
  }
  delay(delayALL);
}

void SetFront(int speed, float timer, char select, float turnTimer) {
  SetFront(speed, timer);
  if (select == 'L') {
    LL(turnTimer);
  }
  if (select == 'R') {
    RR(turnTimer);
  }
  delay(delayALL);
}

void SetBack(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[7] <= 20 && line_value[4] <= 20) {
      motorStop();
      break;
    } else {
      if (line_value[4] <= 20) {
        motorWrite(3, 15);
        motorWrite(4, 15);
      } else {
        motorWrite(3, -speed);
        motorWrite(4, -speed);
      }
      if (line_value[7] <= 20) {
        motorWrite(1, 15);
        motorWrite(2, 15);
      } else {
        motorWrite(1, -speed);
        motorWrite(2, -speed);
      }
    }
  }
  delay(100);
  current_degree = angleRead(YAW);
}

void SetBack(int speed, float timer) {
  SetBack(speed);
  delay(10);
  motorTimeDegree(speed, timer);
  delay(delayALL);
}

void SetBack(int speed, float timer, char select) {
  SetBack(speed, timer);
  if (select == 'L') {
    spinDegree(-90);
  }
  if (select == 'R') {
    spinDegree(90);
  }
  delay(delayALL);
}

void SetBack(int speed, float timer, char select, float turnTimer) {
  SetBack(speed, timer);
  if (select == 'L') {
    LL(turnTimer);
  }
  if (select == 'R') {
    RR(turnTimer);
  }
  delay(delayALL);
}

void motorTimeDegree(int speed, float timer) {
  int base_speed = speed;  // ความเร็วเริ่มต้น และความเร็วก่อนหยุด
  int max_speed = 80;      // ความเร็วสูงสุด
  float kp = 3.5;          // KP
  float kd = 0.1;          // KD
  unsigned long timer_forward = millis();
  float previous_error = 0;
  while (1) {
    unsigned long elapsed_time = millis() - timer_forward;
    unsigned long remaining_time = timer - elapsed_time;
    float error = current_degree - angleRead(YAW);
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    float derivative = error - previous_error_forward;
    int pd_value = (error * kp) + (derivative * kd);
    if (pd_value > max_speed) pd_value = max_speed;
    else if (pd_value < -max_speed) pd_value = -max_speed;
    int speed_left = base_speed + pd_value;
    int speed_right = base_speed - pd_value;
    motorWrite(speed_left, speed_left, speed_right, speed_right);
    previous_error_forward = error;
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
  }
}

void FFS(float speed, float timer) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  FF(speed, timer - timerSlow);
  FF(speed - speedSlow, timerSlow);
}

void FFS(float speed, float timer, char direction) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  FF(speed, timer - timerSlow);
  FF(speed - speedSlow, timerSlow, direction);
}

void FFS(float speed, float timer, char direction, float turnTime) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  FF(speed, timer - timerSlow);
  FF(speed - speedSlow, timerSlow, direction, turnTime);
}

void BBS(float speed, float timer) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  BB(speed, timer - timerSlow);
  BB(speed - speedSlow, timerSlow);
}

void BBS(float speed, float timer, char direction) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  BB(speed, timer - timerSlow);
  BB(speed - speedSlow, timerSlow, direction);
}

void BBS(float speed, float timer, char direction, float turnTime) {
  float timerSlow = (slowPrecent/100) * timer, speedSlow = (slowPrecent/100) * speed;
  BB(speed, timer - timerSlow);
  BB(speed - speedSlow, timerSlow, direction, turnTime);
}