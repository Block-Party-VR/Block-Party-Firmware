// Other peoples libraries
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <FreeRTOS.h>
#include <Adafruit_NeoPixel.h>
#include <chrono>
#include <array>

// Static Defines
#include "PINOUT.h"
#include "BOARD-DEFINITIONS.h"
#include "COMMANDS.h"

// project specific libraries
#include "BluetoothSerial.h"
#include "SerialMessage.h"
#include "GlobalPrint.h"
#include "CommandHandler.h"

#include "BoardManager.h"
#include "BoardDriver.h"
#include "BoardTypes.h"

#include "Animator.h"
#include "TestFrames.h"
#include "Animation.h"

// --------------------------------------------------
// ----------------- VARIABLES ----------------------
// --------------------------------------------------
TaskHandle_t updateCommunicaitonTask;
TaskHandle_t updateBoardTask;

// WARNING! This array size should always be equal to the number of entries in it!!
std::array<std::vector<AnimationFrame>*, 2> animations = {
  &RisingCubes::rising,
  &RotatingCubes::rotating,
};

CommandHandler commandHandler{};

// BluetoothSerial SerialBT;
// BluetoothSerialMessage serialMessageBT(&SerialBT);
SerialMessage<SERIAL_CHAR_LENGTH, SERIAL_ARG_LENGTH> serialMessage(&Serial);

Adafruit_NeoPixel pixelController{BOARD_HEIGHT*2, STACK1_LED_PIN, NEO_GRB + NEO_KHZ800};

Animator<BOARD_DIMENSIONS> animator{};
BoardDriver<BOARD_WIDTH*BOARD_LENGTH> boardDriver{stacks, pixelController};
BoardManager<BOARD_DIMENSIONS> boardManager{boardDriver, animator};
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
  GlobalPrint::Print("!0,");
  String boardString;
  boardManager.Board2StackString(boardString);
  GlobalPrint::Print(boardString);
  GlobalPrint::Println(";");
}

void SetStackColor(uint32_t * args, uint32_t argsLength){
  uint32_t stackNum = args[0];
  uint32_t X_COORD{stackNum};
  while(X_COORD > BOARD_DIMENSIONS.x - 1){
    X_COORD -= BOARD_DIMENSIONS.x;
  }
  uint32_t Y_COORD{(stackNum - X_COORD) / BOARD_DIMENSIONS.y};
  Serial.println("StackNum: " + String(stackNum));
  Serial.println("X: " + String(X_COORD) + " Y: " + String(Y_COORD));
  uint32_t numColors = (argsLength - 1) / 3;

  // nothing to do if no colors were given
  if(numColors == 0){
    return;
  }
  
  Serial.println("Num Colors: " + String(numColors));
  V3D<uint32_t> colors[numColors];
  
  for(int i = 0; i < numColors; i++){
    uint32_t red = args[1 + (i * 3)];
    uint32_t green = args[2 + (i * 3)];
    uint32_t blue = args[3 + (i * 3)];
    colors[i] = V3D<uint32_t>{red, green, blue};
    Serial.println("Color: " + String(red) + "," + String(green) + "," + String(blue));
  }
  boardManager.SetColumnColors(V3D<uint32_t>{X_COORD, Y_COORD, BOARD_TYPES::PLANE_NORMAL::Z}, colors, numColors);
}

// command handling functions
CommandHandler::CommandStatus BoardStateCommandHandler(uint32_t * /*args*/, uint32_t /*argsLength*/){
  printBoardState();
  return CommandHandler::CommandStatus::SUCCESS;
}

CommandHandler::CommandStatus PingCommandHandler(uint32_t * /*args*/, uint32_t /*argsLength*/){
  GlobalPrint::Println("!" + String(Commands::PING) + ";");
  return CommandHandler::CommandStatus::SUCCESS;
}

CommandHandler::CommandStatus SetColorCommandHandler(uint32_t * args, uint32_t argsLength){
  GlobalPrint::Println("!2;");
  animator.isEnabled = false;
  V3D<uint32_t> black{};
  boardManager.FillColor(black);
  SetStackColor(args, argsLength);
  return CommandHandler::CommandStatus::SUCCESS;
}

CommandHandler::CommandStatus GoToIdleCommandHandler(uint32_t * /*args*/, uint32_t /*argsLength*/){
  GlobalPrint::Println("!3;");
  animator.isEnabled = true;
  return CommandHandler::CommandStatus::SUCCESS;
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
      // We reinterpret cast the args to a uint32_t pointer because we know that the args will always be positive
      commandHandler.ProcessCommand(reinterpret_cast<uint32_t *>(serialMessage.GetArgs()), serialMessage.GetPopulatedArgs());
      serialMessage.ClearNewData();
    }
    // serialMessageBT.Update();
    // if(serialMessageBT.IsNewData()){
    //   commandHandler.ProcessCommand(reinterpret_cast<uint32_t *>(serialMessage.GetArgs()), serialMessage.GetPopulatedArgs());
    //   serialMessage.ClearNewData();
    // }
    vTaskDelay(3);
  }
  Serial.println("UpdateCommunication task has ended unexpectedly!");

}

void UpdateBoard(void * params){
  Serial.println("Spawning UpdateBoard task");
  auto updateTickRate{std::chrono::milliseconds(8)};
  auto boardStateTimer{std::chrono::milliseconds(0)};
  auto boardStateMaxUpdatePeriod{std::chrono::milliseconds(34)}; // this is a little slower than 30fps
  unsigned long accurateTimer{millis()};
  auto changeAnimationTimer{std::chrono::milliseconds(0)};
  uint8_t currentAnimation{0};

  for(;;){
    auto actualTimePassed{std::chrono::milliseconds(millis() - accurateTimer)};
    accurateTimer = millis();

    if(boardStateTimer >= boardStateMaxUpdatePeriod && boardManager.HasBoardChanged()){
      printBoardState();
      boardManager.ClearBoardChanged();
      boardStateTimer = std::chrono::milliseconds(0);
    }

    if(changeAnimationTimer >= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1))){
      changeAnimationTimer = std::chrono::milliseconds(0);
      currentAnimation++;
      if(currentAnimation >= animations.size()){
        currentAnimation = 0;
      }
      animator.StartAnimation(animations[currentAnimation]);
    }

    animator.RunAnimation(actualTimePassed);
    boardManager.Update();

    boardStateTimer += actualTimePassed;
    changeAnimationTimer += actualTimePassed;
    vTaskDelay(updateTickRate.count());
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

  // Register all of our commands with the command handler
  commandHandler.RegisterCommand(Commands::BoardState, BoardStateCommandHandler);
  commandHandler.RegisterCommand(Commands::PING, PingCommandHandler);
  commandHandler.RegisterCommand(Commands::SetStackColors, SetColorCommandHandler);
  commandHandler.RegisterCommand(Commands::GoToIdle, GoToIdleCommandHandler);

  Serial.println("Configuring communication methods");
  serialMessage.Init(9600);
  // SerialBT.begin("blockPartyBT");
  xTaskCreate(UpdateCommunication, "UpdateCommunication", 10000, NULL, 0, &updateCommunicaitonTask);

  Serial.println("Beginning Board Initializaiton");
  boardManager.Init();
  animator.SetLoop(true);
  animator.StartAnimation(animations[0]);
  xTaskCreate(UpdateBoard, "UpdateBoard", 10000, NULL, 0, &updateBoardTask);

  Serial.println("Setup Complete");
}


void loop() {
  // delete the loop task because we don't use it
  vTaskDelete(NULL);
}