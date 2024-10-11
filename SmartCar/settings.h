/*
*
*  IO Lijst
*
*/

// H Brug
#define IN1 2      // Rechter Motor 
#define IN2 3      // Rechter Motor
#define IN3 4      // Linker Motor
#define IN4 7      // Linker Motor
#define ENA 5      // PWM Signaal Links klopt
#define ENB 6      // PWM Signaal Rechts klopt

// Lijn Sensors (P0-P4)
#define LINE1 0    // Meest Links
#define LINE2 1    // Links
#define LINE3 2    // Midden
#define LINE4 3    // Rechts
#define LINE5 4    // Meest Rechts

// Ultrasoon Sensors 
#define TRG1  A0   // Trigger Voor
#define TRG2  A2   // Trigger Rechts
#define TRG3  A1   // Trigger Links
#define ECHO1 8    // Echo Voor
#define ECHO2 11   // Echo Rechts
#define ECHO3 9    // Echo Links 


// Bluetooth Ontvanger
#define BTTX 12    // Bluetooth TX
#define BTRX 13    // Bluetooth RX

// Overig
#define SW1  6     // (P6) MENU Knop
#define SW2  7     // (P7) ENTER Knop
#define LED1 A3    // Richtingaanwijzer Links
#define LED2 10    // Richtingaanwijzer Rechts
#define BUZZ 5     // (P5) Buzzer

/*
*
*  Instellingen
*
*/

// Slave Mode
#define SLAVEOBJDIST 10           // Maximale afstand objectdetectie 
#define SLAVESTRAIGHTSPEED 65     // Snelheid Rechtdoor rijden
#define SLAVEINNERSPEED 90        // Snelheid als binnenste sensors de lijn zien
#define SLAVEOUTERSPEED 100       // Snelheid als buitenste sensors de lijn zien 
//#define SLAVEOUTERSPEEDBACK 60    // Snelheid als buitenste sensors de lijn zien 


