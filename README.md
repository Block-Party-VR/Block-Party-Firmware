# Block Party

Stack blocks in real life. See them stack in VR.
Play with blocks in VR. See them change in real life.
If you’re feeling brave, enter the doll house.

Virtual reality can be a powerful tool to connect the physical world and the digital world and connect people in different locations.
However, many VR experiences today are limited to the digital world.
Many multi-user VR experiences place all users into the same digital world and people in the VR headset are cut off from the physical world.

Block Party uses an asymmetric experience to connect a person using a VR headset with a person without a headset.
The person without a headset interacts with physical blocks on a physical board while the person in the VR headset interacts with digital blocks on a digital board.
When the physical user places a physical block on the physical board, the VR user sees a block in the corresponding location on the digital board.
When the physical user removes a block from the board, the corresponding digital block disappears for the VR user.
When the VR user hovers over or picks up a digital cube, the corresponding physical cube changes color to show the physical user what the VR user is doing.

## Setup
### Repository Setup
When the repository is first cloned you will need to initialize all of its submodules. 

In the terminal type: `git submodule update --init --recursive` which will initalize all of the submodules.

### Board Assemble
#### Parts List
- Foam core (scavenged)
- (4) Thor labs boxes (scavenged)
- (1) ESP32
- (1) breadboard
- copper tape
- (9) magnets
- (9) 10k resistors
board documentation can be found int he `documentation` directory.

### Cube Assembly
#### Parts List
- 3D printed case and cover (stl files are in the documentation folder)
- WS2812B LED (1 strip of 2 LEDs)
- (1) 10k resistor
- copper tape
- (2) magnets
- Kapton tape
- black electrical tape to mark the bottom
wiring diagram for the cube is in the `documentation` directory.

### Hardware Required
- Meta Quest headset. The included Unity project uses SDKs for Android so other Android-based headsets may work, but have not been tested.
- The physical board and physical blocks
- Two USB power sources to run the physical board. One for the LEDs and one for the sensors detecting where the blocks are.

### Software Dependencies
#### Unity
- [`the Singularity`](https://github.com/VRatMIT/TheSingularity-Unity)
- Meta Presence Platform: hand tracking, pokable item, interaction, hand-poke interaction, synthetic hands, grab interaction, hand-poke limiter, pass-through
- Unity package: [DOTween](https://assetstore.unity.com/packages/tools/animation/dotween-hotween-v2-27676)

#### Board
- Adafruit’s Neopixel library
- Arduino libraries

### Bluetooth Module
On v0.1 of the board, a seperate bluetooth module (HC-05) is being used for bluetooth. This module needs to be programmed when first plugged in. To program the module, disconnect it from power and hold the "EN" button on the module. (The button should be the only button on the HC-05 module). While still holding down the button, reconnect the module to the ESP32. Let go of the button and then press the ESP32's reset button. Wait 5 seconds while still holding down the "EN" button on the module, then release the button, power cycle the module, and you're done.

## Run
- Power up the board and pair it with the headset over bluetooth.
- Build and load the Unity project to the headset.
- The first time the Unity project starts on the headset, you will receive a request for Bluetooth permissions. Grant the permission and restart the app. The restart is required after permissions are granted due to current limitations on `the Singularity`.
- After restarting the app, the app should automatically connect to the physical board and respond to blocks being placed on the physical board.
- Enjoy the connection between the digital and physical world.

## Unity interactions

### Interacting with the digital blocks
Once the cube has been placed on the board a cube in VR will appear.
The VR user can poke and pickup the digital blocks which will be indicated in real life by changing the colors of the blocks. Poking will change the block color to blue and picking it up will change it to red.

### Passthrough mode
The user can activate passthrough mode inside the app to see the physical board and the digital board at the same time.

### Dollhouse mode
When the dollhouse mode is activated, the digital blocks are increased in size and the VR user is moved to the center square of the board.
Note that a physical cube on the center square of the board will not appear in the digital world.

## Board Command Docs
The board contains an ESP32 that uses bluetooth serial to send the state of the board and receive commands.

### Command Name: `BoardState`
Command Number: `0`

Format Example: `!0,X1,X2,X3,X4,X5,X6,X7,X8,X9;`

Description: This command is sent when the board state changes and describes how the board is populated.

| X1 X2 X3 |

| X4 X5 X6 |

| X7 X8 X9 |

0 corresponds with the command type. The command number for BoardState is 0.
X1 corresponds with board spot X1, and so on. The value in each of the spots indicates how many cubes are in that spot.
IE: If X1 where 3, then there would be 3 cubes in the top left spot.

If you would like to get the board state, you can send !0; to the ESP32 and it will respond with the current board state. Otherwise, the ESP32 will send the board state whenever it changes.

### Command Name `Ping`
Command Number: `1`

Format Example: `!1;`

Description: This command is sent to the ESP32 to check if it is still connected. The ESP32 will respond with `!1;` if it is still connected.

### Command Name: `SetStackColor`
Command Number: `2`

Format Example: `!2,StackNumber,Red1,Green1,Blue1,Red2,Green2,Blue2,...REDX,GREENX,BLUEX;`

Description Set the colors for one of the stacks.
- 2 corresponds with the command type. The command number for SetStackColor is 2.
- StackNumber corresponds with the stack number you want to set the colors for. See `BoardState` description for stack numbers.
- Red1, Green1, Blue1 correspond with the color of the first cube in the stack and so on. These values can be between 0-255.
You can add as many colors as you want, but they wont display if the corresponding cube doesn't physically exist.

### Command Name: `GoToIdle`
Command Number: `3`

Format Example `!3;`

Description: This command will tell the ESP32 to re-enable its idle animation. It will respond with a `!3;` when it recieves this command.

## Shout-Outs
- Thank you to all the organizers at Reality Hack and the Hardware track.
- Lucas De Bonet for creating `the Singularity` which allowed us to connect the board to Quest headsets.
- Mentor Mark Steelman for showing us the value (and complications) of runtime list creation and showing us how to make an in-editor button for debugging.
- Fellow Reality Hack hacker Sean Feeser for help with debugging.
- Juan Gil for helping us 3D print.