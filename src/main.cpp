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
  pinMode(BT_STATE_PIN, INPUT);
  // pull the enable pin high on boot to enable AT command mode
  pinMode(BT_EN_PIN, OUTPUT);
  digitalWrite(BT_EN_PIN, HIGH);
  // Set the serial baud rate to 38400 which is the default for this module
  Serial.begin(38400);
  // Reset the module to default settings
  Serial.print("AT+ORGL\r\n");
  delay(100);
  // Set the serial baud rate to 38400 which is the default for this module
  Serial.begin(9600);
  // Reset the module to default settings
  Serial.print("AT+ORGL\r\n");
  delay(100);
  // set the baud rate of the bluetooth module to 9600
  Serial.print("AT+UART=9600,0,0\r\n");
  // Serial.begin(9600);
  delay(100);
  // Set the bluetooth's name to blockPartyBT
  Serial.print("AT+NAME=blockPartyBT\r\n");
  delay(100);
  // // set the bluetooth's role to slave
  // Serial.print("AT+ROLE=0\r\n");
  // delay(100);
  // // set the connection mode to slave mode
  // Serial.print("AT+CMODE=0\r\n");
  // delay(100);
  // // remove the password
  // Serial.print("AT+PSWD=\r\n");
  // delay(100);
  // // allow the module to be set to any address
  // Serial.print("AT+ADDR=0\r\n");
  // delay(100);

  digitalWrite(BT_EN_PIN, LOW);

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
uint32_t boardStateTimer{0};
bool boardStateHasChanged{false};
uint32_t boardStateMaxUpdatePeriod{15}; // this is a little faster than 60fps

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