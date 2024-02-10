#include "BluetoothSerialMessage.h"

BluetoothSerialMessage::BluetoothSerialMessage(BluetoothSerial *serial){
    this->serial = serial;
}

void BluetoothSerialMessage::Init(unsigned int baud_rate){
    // Don't need to do anything here, just let the user init bluetooth serial
}

void BluetoothSerialMessage::PrintArgs(){
    serial->print("Current number of args: ");
    serial->println(populated_args);
    for (int i = 0; i < populated_args; i++) {
        serial->print(args[i]);
        serial->print(" ");
    }
    serial->println();
}

void BluetoothSerialMessage::readSerial(){
    boolean recvInProgress = false;
    byte ndx = 0;
    char c;

    // read the incoming serial data:
    while (serial->available() > 0 && data_recieved == false) {
        // get the neext character in the serial buffer
        c = serial->read();
        Serial.print(c);

        // only execute this if the startMarker has been received
        if (recvInProgress == true) {
            // if the incoming character is not the endMarker...
            if (c != endMarker) {
                // add it to the data array
                data[ndx] = c;
                ndx++; // increment the data array index
                // if the index is greater than the maximum data array size,
                // keep overwriting the last element until the endMarker is received.
                if (ndx >= num_chars) {
                    ndx = num_chars - 1;
                }
            }
            // if the incoming character is the endMarker clean up and set the flags
            else {
                data[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                data_recieved = true;
            }
        }
        // if the incoming character is the startMarker, set the recvInProgress flag
        else if (c == startMarker) {
            recvInProgress = true;
        }
    }
}