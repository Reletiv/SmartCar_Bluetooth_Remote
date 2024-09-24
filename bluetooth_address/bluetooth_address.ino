#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11);  // RX, TX for HM-10

void setup() {
  // Start Serial communication for debugging (monitor output)
  Serial.begin(9600);    
  
  // Start Bluetooth communication with the HM-10
  BTSerial.begin(9600);  

  // Let the user know we're entering AT command mode
  Serial.println("Enter AT command mode...");

  // Give some time for the HM-10 module to be ready
  delay(1000);  

  // Enter AT command mode
  BTSerial.println("AT");
  delay(1000);
  
  // Request the HM-10 module's Bluetooth address
  BTSerial.println("AT+ADDR?");
  delay(1000);  // Wait for the response
}

void loop() {
  // Check if HM-10 has sent data back (i.e., the address)
  if (BTSerial.available()) {
    while (BTSerial.available()) {
      char c = BTSerial.read();  // Read each character
      Serial.print(c);  // Display the characters on the Serial Monitor
    }
    Serial.println();  // Newline after address is printed
  }
}
