/* Copyright Seeed 2023 */
#ifndef LIB_SEEED_MR24BSD1_SRC_SEEED_MR24BSD1_H_
#define LIB_SEEED_MR24BSD1_SRC_SEEED_MR24BSD1_H_

#define MAX_FRAME_LEN  20    // bytes
#define HEADER         0x55  // Data frame header
#define LENGHT_INDEX   1     // Lenght byte index in frame
#define CMD_INDEX      3
#define ADDR_1_INDEX   4
#define ADDR_2_INDEX   5
#define DATA_INDEX     6
#define ID_LEN         12
#define SOFT_VER_LEN   10
#define HARD_VER_LEN   8
#define PROTO_VER_LEN  8
#define PRINT_RAW      true  // Print raw data bytes
// status
#define NOBODY         0x00
#define STATIC         0x01
#define MOVMENT        0x02


typedef union {
  unsigned char Byte[4];
  float Float;
}Float_Byte;


class Seeed_MR24BSD1{
 private:
        uint8_t device_id[ID_LEN] = {0};
        uint8_t software_ver[SOFT_VER_LEN] = {0};
        uint8_t hardware_ver[HARD_VER_LEN] = {0};
        uint8_t protocol_ver[PROTO_VER_LEN] = {0};
        int status = NOBODY;
        Float_Byte signs_parameter;
        uint8_t threshold = 0;  // 0x01-0x0a
        uint8_t scene_setting = 0;
        uint8_t unoccupied_time = 0;
        uint8_t sleep_func_status = 0;
        uint8_t distance = 0;  // 0x01, 0x02 or 0x03
        uint8_t breathing_rate = 0;
        uint8_t breathing_status = 0;
        uint8_t bed_status = 0;
        uint8_t sleep_state = 0;
        uint8_t duration_of_wakefulness[4] = {0};
        uint8_t light_sleep_lenght[4] = {0};
        uint8_t deep_sleep_lenght[4] = {0};
        uint8_t sleep_quality = 0;
        uint8_t heart_rate = 0;

        void read();
        void process_data();
        void process_03();
        void process_03_01();
        void process_03_03();
        void process_03_04();
        void process_03_05();
        void process_04();
        void process_04_03();
        void process_04_05();
        void process_05();
        void process_05_01();
        void process_05_03();
        void process_05_04();
        void process_05_05();
        void process_05_06();
        uint calculate_time(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

 public:
        uint8_t data_frame_len = MAX_FRAME_LEN;
        uint8_t data_frame[MAX_FRAME_LEN] = {0};
        bool is_new_data = false;

        void begin();
        void begin(uint rx_pin, uint tx_pin);
        void loop(bool print_raw = false);
        void print();
        void print(uint8_t data[]);
        int get_status();  // todo
        float get_signs_parameter();  // todo
        uint8_t get_threshold();  // todo
        uint8_t get_scene_setting();  // todo
        uint8_t get_distance();  // todo
        uint8_t get_breathing_rate();  // todo
        uint8_t get_breathing_status();  // todo
        uint8_t get_bed_status();
        uint8_t get_sleep_state();
        int get_duration_of_wakefulness();
        int get_light_sleep_lenght();
        int get_deep_sleep_lenght();
        uint8_t get_sleep_quality();
        uint8_t get_heart_rate();

        void Bodysign_judgment(byte inf[], float Move_min, float Move_max);
        void Situation_judgment(byte inf[]);
        void Sleep_inf(byte inf[]);
        void SleepTimeCalculate(unsigned char inf1, unsigned char inf2, unsigned char inf3, unsigned char inf4);
        
        unsigned short int us_CalculateCrc16(unsigned char *lpuc_Frame, unsigned short int lus_Len);
};

#endif  // LIB_SEEED_MR24BSD1_SRC_SEEED_MR24BSD1_H_
