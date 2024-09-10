# Setup
In order to run this tool follow these steps:

1. ensure you're using python 3.12+. You can verify this by running 
```
python --version
``` 
in a terminal.

2. Inside of a terminal in the 
```
tools/animation-tools
``` 
folder, run: 
```
python -m venv venv
```
 to create your virtual environment.

3. Now activate the virtual environment. In powershell run 
```
venv\Scripts\activate
```
You may need to fix permission issues if this is your first time using a virtual environment. For git bash just run: 
```
source venv/Scripts/activate
```
4. Run 
```
pip install -r requirements.txt
```

# How to Use
The animation tool was made to allow anyone to easily create animaitons without much practice. You can create a sequence of frames in the studio, export them and then copy them over to the firmware

## Creating a Frame
When you first run the program the window will look like this:

![Startup](images\open-animator.png)

### Rotating the view
Press and hold the middle mouse button and move around the mouse to rotate the view.

### Changing Cube Colors
Click on a cube to select it. Selected cubes will be highlighted in red. All selected cubes will be set to the current color picker color. You can deselect a block by clicking on it again.

The color picker consists of 3 sliders on the left of the screen. A preview of the color is in a small box underneath the sliders. Move the sliders around to change colors.

### Fill Options
Fill options tell the board how to color cubes which are set to black in the animator. There are four options:

#### No Fill
Cubes that are set to black will be off

#### Closest Color
Cubes that are set to black will become the same color as their closest colored neighbor

#### Linear
Cubes that are set to black will become a distance weighted average of all of the colored cubes

#### Square
Same as linear, but the distance weighting is squared. This means a cube that's twice as far away will have only 1/4 the effect.

### Fade options
The fade options tell the board how to transition between frames

#### Snap
The colors will remain constant until the delay time for the current frame has passed. At that point, the frame will immediatly switch to the next frame.

#### Fade
The colors will linearly transition from one frame to the next. This often results in very interesting colors and patterns emerging.

### Creating more than one frame
An animation isn't really an animation with only one frame. You can add more frames to your animation with the "Next Frame" button. This will create a new frame for you to edit. You'll also notice the frame counter above the next frame button will go up. That shows you the current frame number that you're editing. You can view previous frames but clicking on the last frame button.

### Exporting your Animation
Now that you're done creating your lovely animation you can export it! Click the save button and C++ code will automatically be generated which describes all of the frames you just created. The code is saved in output.txt in the animation-tools folder. How to use that code is beyond the scope of this README. Reach out to Quinn for more instruction.

# Future features
Eventually I would like to add the ability to send the animations directly to the board without having to mess with the firmware at all. This will take a pretty big rework though.