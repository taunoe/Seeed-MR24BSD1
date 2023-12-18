/* Copyright Seeed 2023 */
#ifndef LIB_SEEED_MR24BSD1_SEEED_MR24BSD1_H_
#define LIB_SEEED_MR24BSD1_SEEED_MR24BSD1_H_

#define MAX_DATA_LEN   12    // bytes
#define HEADER         0x55  // Data frame header
#define LENGHT_INDEX    1     // Lenght byte index in frame
#define CMD_INDEX       3
#define ADDR_1_INDEX    4
#define ADDR_2_INDEX    5
#define DATA_INDEX      6
#define ID_LEN         12
#define SOFT_VER_LEN   10
#define HARD_VER_LEN    8
#define PROTO_VER_LEN   8
#define PRINT_RAW      true  // Print raw data bytes
// Function codes:
#define READ           0x01
#define WRITE          0x02
#define PASSIVE_REPORT 0x03
#define ACTIVE_REPORT  0x04


#define REPORT_RADAR 0x03       //Report radar information
#define REPORT_OTHER 0x05       //Report other information

#define HEARTBEAT 0x01          //Heartbeat Pack
#define ABNOEMAL 0x02           //Abnormal Reset
#define ENVIRONMENT 0x05        //Environment
#define BODYSIGN 0x06           //Physical parameters
#define CLOSE_AWAY 0x07         //Approach away

#define CA_BE 0x01              //Approach away head frame
#define CA_CLOSE 0x02           //Someone approaches
#define CA_AWAY 0x03            //Some people stay away
#define SOMEBODY_BE 0x01        //Motion state header frame
#define SOMEBODY_MOVE 0x01      //Somebody move
#define SOMEBODY_STOP 0x00      //Somebody stop
#define NOBODY 0x00             //No one here

#define SLEEP_INF 0x05          //Sleep radar data header frames
#define BREATH 0x01             //Breathing parameters
#define SCENARIO 0x03           //Scenario evaluation
#define SLEEP_TIME 0x04         //Duration parameters
#define SLEEP_QUALITY 0x05      //Sleep quality

#define BREATH_RATE 0x01        //Breathing rate
#define CHECK_SIGN 0x04         //Detection signal

#define BREATH_HOLD 0x01        //Breath-holding abnormalities
#define BREATH_NULL 0x02        //None
#define BREATH_NORMAL 0x03      //Normal breathing
#define BREATH_MOVE 0x04        //Movement abnormalities
#define BREATH_RAPID 0x05       //Acute respiratory abnormalities

#define CLOSE_AWAY_BED 0x07     //Getting in and out of bed judgment
#define SLEEP_STATE 0x08        //Sleep state judgment

#define AWAY_BED 0x00           //Leaving the bed
#define CLOSE_BED 0x01          //Get into bed

#define AWAKE 0x00              //sleep state:Awake
#define LIGHT_SLEEP 0x01        //sleep state:Light sleep
#define DEEP_SLEEP 0x02         //sleep state:deep sleep
#define SLEEP_NULL 0x03         //no sleep state

#define AWAKE_TIME 0x01         //Awake time
#define LIGHT_SLEEP_TIME 0x02   //Light sleep time
#define DEEP_SLEEP_TIME 0x03    //Deep sleep time

#define SLEEP_SCORE 0x01        //Sleep quality score

class Seeed_MR24BSD1{
 private:
        uint8_t device_id[ID_LEN] = {0};
        uint8_t software_ver[SOFT_VER_LEN] = {0};
        uint8_t hardware_ver[HARD_VER_LEN] = {0};
        uint8_t protocol_ver[PROTO_VER_LEN] = {0};

        void read();
        void process_data();
        void process_03();
        void process_03_01();
        void process_04();
        void process_05();

 public:
        uint8_t data_frame_len = MAX_DATA_LEN;
        uint8_t data_frame[MAX_DATA_LEN] = {0};
        bool is_new_data = false;

        void begin();
        void begin(uint rx_pin, uint tx_pin);
        void loop(bool print_raw = false);
        void print();
        void print(uint8_t data[]);

        void Bodysign_judgment(byte inf[], float Move_min, float Move_max);
        void Situation_judgment(byte inf[]);
        void Sleep_inf(byte inf[]);
        void SleepTimeCalculate(unsigned char inf1, unsigned char inf2, unsigned char inf3, unsigned char inf4);
        
        unsigned short int us_CalculateCrc16(unsigned char *lpuc_Frame, unsigned short int lus_Len);
};

#endif  // LIB_SEEED_MR24BSD1_SEEED_MR24BSD1_H_
