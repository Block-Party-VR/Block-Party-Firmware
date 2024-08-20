// Other peoples libraries
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <FreeRTOS.h>

// Static Defines
#include "PINOUT.h"
#include "BOARD-DEFINITIONS.h"
#include "COMMANDS.h"

// project specific libraries
#include "BluetoothSerial.h"
#include "SerialMessage.h"
#include "BoardLayout.h"
#include "Color.h"
#include "ColorManager.h"
#include "GlobalPrint.h"

// --------------------------------------------------
// ----------------- VARIABLES ----------------------
// --------------------------------------------------
TaskHandle_t updateCommunicaitonTask;
TaskHandle_t updateBoardTask;

uint32_t boardStateTimer{0};
bool boardStateHasChanged{false};
uint32_t boardStateMaxUpdatePeriod{34}; // this is a little slower than 30fps

// BluetoothSerial SerialBT;
// BluetoothSerialMessage serialMessageBT(&SerialBT);
SerialMessage<500, 10> serialMessage(&Serial);
BoardLayout board(BOARD_WIDTH, BOARD_LENGTH, BOARD_HEIGHT, stacks);

// Temporary thing until we can get bluetooth color management working on the quest
ColorManager colorManager(&board);

// --------------------------------------------------
// ----------------- FUNCTIONS ----------------------
// --------------------------------------------------
/**
 * @brief Send programming commands to the serial to bluetooth adapter so 
 * it is set up as expected for the VR headset
 * @post the serial baud rate will be set to 9600
 */
void SetupBluetoothModule(){
  Serial.begin(38400);
  Serial.print("AT+UART=9600,0,0\r\n"); // set baud rate to 9600
  delay(100);

  Serial.print("AT+NAME=blockPartyBT-v01\r\n"); // set name to blockPartyBT-v0.1
  delay(100);

  Serial.print("AT+PSWD=1234\r\n"); // set password to 1234
  delay(100);

  Serial.print("AT+ROLE=0\r\n"); // set to slave
  delay(100);

  // exit at mode and go into pairing mode
  Serial.print("AT+INIT\r\n");
  Serial.begin(9600);
  delay(100);
}


void printBoardState(){
  // create a buffer to hold the board state
  uint16_t boardState[BOARD_WIDTH * BOARD_LENGTH];
  // read in the board state
  board.GetBoardState(boardState);

  GlobalPrint::Print("!0,");

  for(int i = 0; i < (BOARD_WIDTH * BOARD_LENGTH); i++){
    GlobalPrint::Print(String(boardState[i]));
    if(i == (BOARD_WIDTH * BOARD_LENGTH) - 1){
      break;
    }
    GlobalPrint::Print(",");
  }

  GlobalPrint::Println(";");
}

void SetStackColor(uint32_t * args, int argsLength){
  uint32_t stackNum = args[1];
  uint32_t numColors = (argsLength - 2) / 3;
  Color colors[numColors];
  
  for(int i = 0; i < numColors; i++){
    int red = args[2 + (i * 3)];
    int green = args[3 + (i * 3)];
    int blue = args[4 + (i * 3)];
    colors[i] = Color(red, green, blue);
  }

  board.SetStackColors(stackNum, colors);
}

void parseData(Message<500, 10> &message){
  int32_t * args{message.GetArgs()};
  uint32_t argsLength{message.GetArgsLength()};
  uint32_t command = args[0];
  switch(command){
    case Commands::BoardState:
      printBoardState();
      break;
    case Commands::PING:
      GlobalPrint::Println("!" + String(Commands::PING) + ";");
      break;
    case Commands::SetStackColors:
      GlobalPrint::Println("!2;");
      colorManager.Enable(false);
      SetStackColor(reinterpret_cast<uint32_t *>(args), argsLength);
      break;
    case Commands::GoToIdle:
      GlobalPrint::Println("!3;");
      colorManager.Enable(true);
      break;
    default:
      GlobalPrint::Println("INVALID COMMAND");
      break;
  }

  // now that we have run the command we can clear the data for the next command.
  serialMessage.ClearNewData();
}

// --------------------------------------------------
// ----------------- FREERTOS TASKS -----------------
// --------------------------------------------------
void UpdateCommunication(void * params){
  Serial.println("Spawning UpdateCommunication task");
  for(;;){
    // DO serial processing
    serialMessage.Update();
    if(serialMessage.IsNewData()){
      parseData(serialMessage);
    }
    // serialMessageBT.Update();
    // if(serialMessageBT.IsNewData()){
    //   parseData(serialMessageBT.GetArgs(), serialMessageBT.GetArgsLength());
    //   serialMessageBT.ClearNewData();
    // }
    vTaskDelay(3);
  }
  Serial.println("UpdateCommunication task has ended unexpectedly!");

}

void UpdateBoard(void * params){
  Serial.println("Spawning UpdateBoard task");
  for(;;){
    if(board.BoardStateHasChanged()){
      boardStateHasChanged = true;
    }

    if(millis() - boardStateTimer > boardStateMaxUpdatePeriod && boardStateHasChanged){
      boardStateTimer = millis();
      printBoardState();
      boardStateHasChanged = false;
    }

    colorManager.Update();

    vTaskDelay(5);
  }
  Serial.println("UpdateBoard task has ended unexpectedly!");
}


// --------------------------------------------------
// ----------------- SETUP AND LOOP -----------------
// --------------------------------------------------

void setup() {
  // delay a little bit to get the serial monitor a chance to capture the next log messages.
  delay(1000);

  Serial.begin(9600);
  Serial.println("Beginning Setup");
  Serial.println("Configuring Bluetooth Adapter");
  SetupBluetoothModule();
  Serial.begin(9600);

  Serial.println("Configuring communication methods");
  serialMessage.Init(9600);
  // SerialBT.begin("blockPartyBT");
  xTaskCreate(UpdateCommunication, "UpdateCommunication", 10000, NULL, 0, &updateCommunicaitonTask);

  Serial.println("Beginning Board Initializaiton");
  xTaskCreate(UpdateBoard, "UpdateBoard", 10000, NULL, 0, &updateBoardTask);
  Color colors[] = {Color(255, 0, 0), Color(0, 0, 0), Color(0, 0, 0)};
  board.SetStackColors(2, colors);
  boardStateTimer = millis();

  Serial.println("Setup Complete");
}


void loop() {
  // delete the loop task because we don't use it
  vTaskDelete(NULL);
}