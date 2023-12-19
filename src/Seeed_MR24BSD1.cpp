/* Copyright Tauno Erik 2023 */
#include "Arduino.h"
#include "Seeed_MR24BSD1.h"

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3);  // RX, TX
    #define Serial1 SSerial
#endif

void Seeed_MR24BSD1::begin() {
  Serial1.begin(9600);
}

void Seeed_MR24BSD1::begin(uint rx_pin, uint tx_pin) {
  Serial1.setRX(rx_pin);  // 17
  Serial1.setTX(tx_pin);  // 16
  Serial1.begin(9600);    // 9600
}

/*
  Receive data and process
*/
void Seeed_MR24BSD1::loop(bool print_raw) {
  read();

  if (is_new_data) {
    if (print_raw) {
      print();
    }
    process_data();
    is_new_data = false;
  }
}


void Seeed_MR24BSD1::read() {
  static bool is_receiving = false;
  static uint8_t i = 0;
  static uint8_t len = MAX_FRAME_LEN;

  uint8_t new_byte;

  while (Serial1.available() && !is_new_data) {
    new_byte = Serial1.read();

    if (is_receiving) {
      if (i < len) {
        i++;  // start from 1
        data_frame[i] = new_byte;

        // Andmete suurus
        if (i == LENGHT_INDEX) {
          // Juhul, kui data byte on suurem, kui tohib olla!
          if (new_byte > MAX_FRAME_LEN) {
            len = MAX_FRAME_LEN;
          } else {
            len = new_byte;
          }
        }
      } else {
        is_receiving = false;
        i = 0;
        is_new_data = true;
      }
    } else if (new_byte == HEADER) {  // data frame first byte
      is_receiving = true;
      data_frame[0] = HEADER;
    }
  }
}


/*
Serial Print data frame
(byte inf[])
*/
void Seeed_MR24BSD1::print() {
  uint len = data_frame[LENGHT_INDEX]+1;

  for (uint8_t i = 0; i < len; i++) {
    Serial.print(data_frame[i], HEX);
    Serial.print(' ');
  }

  Serial.println();
}

void Seeed_MR24BSD1::print(uint8_t data[]) {
  int len = data_frame_len + 1;

  for (uint8_t i = 0; i < len; i++) {
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }

  Serial.println();
}

void Seeed_MR24BSD1::process_data() {
  int lenght = data_frame[LENGHT_INDEX];
  int cmd = data_frame[CMD_INDEX];

  switch (cmd) {
    case 0x03:
      // passive reporting
      process_03();
      break;
    case 0x04:
      // proactive reporting
      process_04();
      break;
    case 0x05:
      // sleeping data
      process_05();
      break;
  }
}

void Seeed_MR24BSD1::process_03() {
  // Serial.println("passive reporting");
  int addr_1 = data_frame[ADDR_1_INDEX];
  int addr_2 = data_frame[ADDR_2_INDEX];

  switch (addr_1) {
    case 0x01:
      process_03_01();
      break;
    case 0x03:
      process_03_03();
      break;
    case 0x04:
      process_03_04();
      break;
    case 0x05:
      process_03_05();
      break;
  }
}

void Seeed_MR24BSD1::process_03_01() {
  Serial.print("Device ");
  int addr_2 = data_frame[ADDR_2_INDEX];

  switch (addr_2) {
    case 0x01:
      Serial.println("ID");
      for (int i = 0; i < ID_LEN; i++) {
        device_id[i] = data_frame[DATA_INDEX + i];
      }
      break;
    case 0x03:
      Serial.println("Software version");
      for (int i = 0; i < SOFT_VER_LEN; i++) {
        software_ver[i] = data_frame[DATA_INDEX + i];
      }
      break;
    case 0x04:
      Serial.println("Hardware version");
      for (int i = 0; i < HARD_VER_LEN; i++) {
        hardware_ver[i] = data_frame[DATA_INDEX + i];
      }
      break;
    case 0x05:
      Serial.println("Protocol version");
      for (int i = 0; i < PROTO_VER_LEN; i++) {
        protocol_ver[i] = data_frame[DATA_INDEX + i];
      }
      break;
  }
}

void Seeed_MR24BSD1::process_03_03() {
  Serial.println("Env status ");
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];
  int data_2 = data_frame[DATA_INDEX+1];

  switch (addr_2) {
    case 0x05:
      switch (data_1) {
        case 0x00:
          status = NOBODY;
          break;
        case 0x01:
          switch (data_2) {
            case 0x00:
              status = STATIC;
              break;
            case 0x01:
              status = MOVMENT;
              break;
          }
          break;
      }
      break;
    case 0x06:
      signs_parameter.Byte[0] = data_frame[DATA_INDEX];
      signs_parameter.Byte[1] = data_frame[DATA_INDEX+1];
      signs_parameter.Byte[2] = data_frame[DATA_INDEX+2];
      signs_parameter.Byte[3] = data_frame[DATA_INDEX+3];
      Serial.print("Signs parameter: ");
      Serial.println(signs_parameter.Float);
      break;
  }
}

void Seeed_MR24BSD1::process_03_04() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];

  switch (addr_2) {
    case 0x0c:
      threshold = data_1;
      break;
    case 0x10:
      scene_setting =  data_1;
      break;
    case 0x12:
      unoccupied_time = data_1;
      break;
  }
}

void Seeed_MR24BSD1::process_03_05() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];

  switch (addr_2) {
    case 0x0d:
      sleep_func_status = data_1;
      break;
    case 0x08:
      // ota upgrade
      break;
    case 0x09:
      // upgrade package
      break;
  }
}

void Seeed_MR24BSD1::process_04() {
  Serial.println("proactive reporting");
  int addr_1 = data_frame[ADDR_1_INDEX];

  switch (addr_1) {
    case 0x03:
      process_04_03();
      break;
    case 0x05:
      process_04_05();
      break;
  }
}

void Seeed_MR24BSD1::process_04_03() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];
  int data_2 = data_frame[DATA_INDEX+1];

  switch (addr_2) {
    case 0x05:
      switch (data_1) {
        case 0x00:
          status = NOBODY;
          break;
        case 0x01:
          switch (data_2) {
            case 0x00:
              status = STATIC;
              break;
            case 0x01:
              status = MOVMENT;
              break;
          }
          break;
      }
      break;
    case 0x06:
      signs_parameter.Byte[0] = data_frame[DATA_INDEX];
      signs_parameter.Byte[1] = data_frame[DATA_INDEX+1];
      signs_parameter.Byte[2] = data_frame[DATA_INDEX+2];
      signs_parameter.Byte[3] = data_frame[DATA_INDEX+3];
      Serial.print("Signs parameter: ");
      Serial.println(signs_parameter.Float);
      break;
    case 0x07:
      // approaching
      distance = data_2;
      break;
  }
}

void Seeed_MR24BSD1::process_04_05() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];
  int data_2 = data_frame[DATA_INDEX+1];

  switch (addr_2) {
    case 0x01:
      switch (data_1) {
        case 0x00:
          status = NOBODY;
          break;
        case 0x01:
          switch (data_2) {
            case 0x00:
              status = STATIC;
              break;
            case 0x01:
              status = MOVMENT;
              break;
          }
          break;
      }
      break;
    case 0x02:
      Serial.println("Abnormal reset!");
      break;
    case 0x0a:
      Serial.println("Initialisation successful!");
      break;
  }
}


void Seeed_MR24BSD1::process_05() {
  Serial.println("sleeping data");
  int addr_1 = data_frame[ADDR_1_INDEX];

  switch (addr_1) {
    case 0x01:
      process_05_01();
      break;
    case 0x03:
      process_05_03();
      break;
    case 0x04:
      process_05_04();
      break;
    case 0x05:
      process_05_05();
      break;
    case 0x06:
      process_05_06();
      break;
  }
}

void Seeed_MR24BSD1::process_05_01() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];

  switch (addr_2) {
    case 0x01:
      breathing_rate = data_1;
      break;
    case 0x04:
      breathing_status = data_1;
      break;
  }
}

void Seeed_MR24BSD1::process_05_03() {
  int addr_2 = data_frame[ADDR_2_INDEX];
  int data_1 = data_frame[DATA_INDEX];

  switch (addr_2) {
    case 0x07:
      bed_status = data_1;
      break;
    case 0x08:
      sleep_state = data_1;
      break;
  }
}

void Seeed_MR24BSD1::process_05_04() {
  int addr_2 = data_frame[ADDR_2_INDEX];

  switch (addr_2) {
    case 0x01:
      duration_of_wakefulness[0] = data_frame[DATA_INDEX];
      duration_of_wakefulness[1] = data_frame[DATA_INDEX+1];
      duration_of_wakefulness[2] = data_frame[DATA_INDEX+2];
      duration_of_wakefulness[3] = data_frame[DATA_INDEX+3];
      break;
    case 0x02:
      light_sleep_lenght[0] = data_frame[DATA_INDEX];
      light_sleep_lenght[1] = data_frame[DATA_INDEX+1];
      light_sleep_lenght[2] = data_frame[DATA_INDEX+2];
      light_sleep_lenght[3] = data_frame[DATA_INDEX+3];
      break;
    case 0x03:
      deep_sleep_lenght[0] = data_frame[DATA_INDEX];
      deep_sleep_lenght[1] = data_frame[DATA_INDEX+1];
      deep_sleep_lenght[2] = data_frame[DATA_INDEX+2];
      deep_sleep_lenght[3] = data_frame[DATA_INDEX+3];
      break;
  }
}

void Seeed_MR24BSD1::process_05_05() {
  int addr_2 = data_frame[ADDR_2_INDEX];

  if (addr_2 == 0x01) {
    sleep_quality = data_frame[DATA_INDEX];
  }
}

void Seeed_MR24BSD1::process_05_06() {
  int addr_2 = data_frame[ADDR_2_INDEX];

  if (addr_2 == 0x01) {
    heart_rate = data_frame[DATA_INDEX];
  }
}

uint Seeed_MR24BSD1::calculate_time(uint8_t b1,
                                    uint8_t b2,
                                    uint8_t b3,
                                    uint8_t b4) {
  uint time = 0;
  time = (b1 <<  24)+(b2 << 16)+(b3 << 8)+(b4);
  return time;
}

///////////////////////////////////////////////////
/*************************************************/
///////////////////////////////////////////////////

const unsigned char cuc_CRCHi[256]= {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40
};

const unsigned char  cuc_CRCLo[256]= {
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
  0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
  0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
  0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
  0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
  0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
  0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
  0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
  0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
  0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
  0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
  0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
  0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
  0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
  0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
  0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
  0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
  0x41, 0x81, 0x80, 0x40
};

unsigned short int Seeed_MR24BSD1::us_CalculateCrc16(unsigned char *lpuc_Frame, unsigned short int lus_Len){
  unsigned char luc_CRCHi = 0xFF;
  unsigned char luc_CRCLo = 0xFF;
  int li_Index = 0;
  while (lus_Len--) {
    li_Index = luc_CRCLo ^ * (lpuc_Frame++);
    luc_CRCLo = (unsigned char)(luc_CRCHi ^ cuc_CRCHi[li_Index]);
    luc_CRCHi = cuc_CRCLo[li_Index];
  }
  return (unsigned short int)(luc_CRCLo << 8 | luc_CRCHi);
}

