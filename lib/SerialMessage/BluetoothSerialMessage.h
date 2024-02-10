#pragma once
#include "SerialMessage.h"
#include <BluetoothSerial.h>

class BluetoothSerialMessage : public SerialMessage{
    public:
        /**
         * @brief Construct a new Bluetooth Serial Message object
         */
        BluetoothSerialMessage(BluetoothSerial *serial);

        /**
         * @brief Initialize the BluetoothSerialMessage object
         */
        void Init(unsigned int baud_rate = 115200) override;

        /**
         * @brief prints the args array to the serial monitor
        */
        void PrintArgs() override;
    
    private:
        /**
         * @brief reads the serial data and stores it in the data array
        */
        void readSerial() override;

        BluetoothSerial *serial;


};