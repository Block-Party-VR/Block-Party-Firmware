// Other peoples libraries
#include <Arduino.h>
#include <BluetoothSerial.h>

// project specific libraries
#include "BluetoothSerial.h"
#include "SerialMessage.h"
#include "BoardLayout.h"
#include "BOARD-DEFINITIONS.h"
#include "Color.h"
#include "ColorManager.h"

// --------------------------------------------------
// ----------------- Types ----------------------
// --------------------------------------------------
enum Commands : uint8_t{
  BoardState = 0,
  PING = 1,
  SetStackColors = 2,
  GoToIdle = 3
};

// --------------------------------------------------
// ----------------- VARIABLES ----------------------
// --------------------------------------------------
uint32_t boardStateTimer{0};
bool boardStateHasChanged{false};
uint32_t boardStateMaxUpdatePeriod{34}; // this is a little slower than 30fps

// BluetoothSerial SerialBT;
// BluetoothSerialMessage serialMessageBT(&SerialBT);
SerialMessage<500> serialMessage(&Serial);
BoardLayout board(BOARD_WIDTH, BOARD_LENGTH, BOARD_HEIGHT, stacks);

// Temporary thing until we can get bluetooth color management working on the quest
ColorManager colorManager(&board);

// --------------------------------------------------
// ----------------- FUNCTIONS ----------------------
// --------------------------------------------------
void SetupBluetoothModule(){
  Serial.begin(38400);
  Serial.print("AT+UART=9600,0,0\r\n"); // set baud rate to 9600
  delay(100);

  Serial.print("AT+NAME=blockPartyBT-v01\r\n"); // set name to blockPartyBT-v01
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

  Serial.print("!0,");
  // SerialBT.print("!0,");

  for(int i = 0; i < (BOARD_WIDTH * BOARD_LENGTH); i++){
    Serial.print(boardState[i]);
    // SerialBT.print(boardState[i]);
    if(i == (BOARD_WIDTH * BOARD_LENGTH) - 1){
      break;
    }
    Serial.print(",");
    // SerialBT.print(",");
  }

  Serial.println(";");
  // SerialBT.println(";");
}

void setStackColor(uint32_t * args, int argsLength){
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

void parseData(uint32_t * args, int argsLength){
  uint32_t command = args[0];
  switch(command){
    case Commands::BoardState:
      printBoardState();
      break;
    case Commands::PING:
      Serial.print("!");
      // SerialBT.print("!");
      Serial.print(Commands::PING);
      // SerialBT.print(Commands::PING);
      Serial.println(";");
      // SerialBT.println(";");
      break;
    case Commands::SetStackColors:
      Serial.println("!2;");
      // SerialBT.println("!2;");
      colorManager.Enable(false);
      setStackColor(args, argsLength);
      break;
    case Commands::GoToIdle:
      Serial.println("!3;");
      // SerialBT.println("!3;");
      colorManager.Enable(true);
      break;
    default:
      Serial.println("INVALID COMMAND");
      // SerialBT.println("INVALID COMMAND");
      break;
  }
}

// --------------------------------------------------
// ----------------- SETUP AND LOOP -----------------
// --------------------------------------------------

void setup() {
  delay(1000);
  SetupBluetoothModule();
  Serial.begin(9600);
  // SerialBT.begin("blockPartyBT");
  Color colors[] = {Color(255, 0, 0), Color(0, 0, 0), Color(0, 0, 0)};
  board.SetStackColors(2, colors);

  boardStateTimer = millis();
  
}


void loop() {
  if(board.BoardStateHasChanged()){
    boardStateHasChanged = true;
  }

  if(millis() - boardStateTimer > boardStateMaxUpdatePeriod && boardStateHasChanged){
    boardStateTimer = millis();
    printBoardState();
    boardStateHasChanged = false;
  }

  // DO serial processing
  serialMessage.Update();
  if(serialMessage.IsNewData()){
    parseData(serialMessage.GetArgs(), serialMessage.GetArgsLength());
    serialMessage.ClearNewData();
  }
  // serialMessageBT.Update();
  // if(serialMessageBT.IsNewData()){
  //   parseData(serialMessageBT.GetArgs(), serialMessageBT.GetArgsLength());
  //   serialMessageBT.ClearNewData();
  // }
  colorManager.Update();
}