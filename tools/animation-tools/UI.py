from itertools import product
from pygame_widgets.button import Button
from pygame_widgets.slider import Slider
from pygame_widgets.dropdown import Dropdown
from pygame_widgets.textbox import TextBox
from Scene import Scene
from Shapes import *
import pygame
from AnimationExporter import *

class ColorPicker:
    def __init__(self, screen, x_pos: int, y_pos: int, width: int, height: int):
        width = max(50, width)
        slider_colors = ((255,0,0),(0,255,0),(0,0,255))
        self.sliders: list[Slider] = [
            Slider(screen, int(x_pos + i*(width/3)), y_pos, int((width-50)/3), height, min=0, max=255, step=1, vertical=True, handleColour=slider_colors[i]) for i in range(3)
            ]
        for slider in self.sliders:
            slider.enable()
    
    def get_color(self) -> tuple[int]:
        return tuple([slider.getValue() for slider in self.sliders])

    def set_color(self, color: tuple[int]):
        for i, slider in enumerate(self.sliders):
            slider.setValue(color[i])

class SceneStore:
    def __init__(self, scene: Scene, fill: FillInterpolation, fade: FrameInterpolation, delay: int):
        self.scene: Scene = scene
        self.fill: FillInterpolation = fill
        self.fade: FrameInterpolation = fade
        self.delay: int = delay
         
class SceneManager:
    def __init__(self, window, color_picker: ColorPicker):     
        self.file_data: str = ""
        self._scenes: list[SceneStore] = [SceneStore(self.new_scene(), FillInterpolation.NO_FILL, FrameInterpolation.FADE, 1000)]
        self._current_scene_idx: int = 0
        self.window = window
        self.color_picker = color_picker
        self._selected_meshes: list[Mesh] = []
    
    def save_frame_to_file(self):
        with open("tools/animation-tools/output.txt", 'w') as file:
            for i, scene in enumerate(self._scenes):
                file.write(scene_to_frame(scene.scene, scene.fill_interpolation, scene.frame_interpolation, scene.delay, i))

    def generate_meshes(self) -> list[Mesh]:
        # generate a list of cubes
        meshes: list[Mesh] = []
        for origin in product([-1, 0, 1],[-1, 0, 1],[-1, 0, 1]):
            cube = Cube()
            cube.scale((0.35, 0.35, 0.35))
            cube.set_position(origin)
            cube.set_face_color((0, 0, 0))
            meshes.append(cube)
        return meshes

    def new_scene(self) -> Scene:
        return Scene(self.generate_meshes(), 90, 5)
        
    def draw(self):
        for mesh in self._selected_meshes:
            mesh.set_face_color(self.color_picker.get_color())
        self.get_current_scene().scene.draw(self.window)
    
    def get_current_scene(self) -> SceneStore:
        return self._scenes[self._current_scene_idx]
    
    def click_mesh(self, coordinates: tuple[int, int]):
        mesh = self.get_current_scene().scene.get_mesh_from_xy(coordinates)
        
        if mesh == None:
            return
        
        if mesh in self._selected_meshes:
            mesh.set_edge_color((0,0,0))
            self._selected_meshes.remove(mesh)
        else:
            mesh.set_edge_color((255,0,0))
            self._selected_meshes.append(mesh)
              
    def deselect_all_mesh(self):
        for mesh in self._selected_meshes:
            mesh.set_edge_color((0,0,0))
        self._selected_meshes = []
    
    def next_scene(self):
        self.deselect_all_mesh()
        current_angles = self.get_current_scene().scene.euler_angles
        if len(self._scenes)-1 == self._current_scene_idx:
            self._scenes.append(SceneStore(self.new_scene(), FillInterpolation.NO_FILL, FrameInterpolation.FADE, 1000))
            
        self._current_scene_idx += 1
        
        self.get_current_scene().scene.euler_angles = [angle for angle in current_angles]
    
    def last_scene(self):
        if self._current_scene_idx > 0:
            current_angles = self.get_current_scene().scene.euler_angles
            self._current_scene_idx -= 1
            self.get_current_scene().scene.euler_angles = [angle for angle in current_angles]
            self.deselect_all_mesh()
    
    def update_scene_options(self, fill: FillInterpolation, fade: FrameInterpolation, delay: int):
        cur_scene: SceneStore = self.get_current_scene()
        cur_scene.fill = fill
        cur_scene.fade = fade
        cur_scene.delay = delay
            
class AnimatorUI:
    def __init__(self, window):
        scr_wdt, scr_hgt = window.get_size()
        
        self.window = window
        self.colorPicker: ColorPicker = ColorPicker(self.window, *self.rel2abs(5, 5, 20, 60))
        self.sceneManager: SceneManager = SceneManager(window, self.colorPicker)
        
        self.save_button: Button = Button(self.window, *self.rel2abs(5, 90, 10, 10), text="Save",onClick=self.sceneManager.save_frame_to_file)
        self.next_frame_button: Button = Button(self.window, *self.rel2abs(75, 90, 25, 5), text="Next Frame",onClick=self._next_scene)
        self.last_frame_button: Button = Button(self.window, *self.rel2abs(50, 90, 25, 5), text="last Frame",onClick=self._last_scene)
        self.trackMouseMotion: bool = False
        
        self.fill_dropdown: Dropdown = Dropdown(self.window, *self.rel2abs(30, 0, 40, 5), name="Fill Type",
                                      choices=[option.value for option in FillInterpolation], onRelease=self.set_update_options_flag)
        
        self.fade_dropdown: Dropdown = Dropdown(self.window, *self.rel2abs(70, 0, 20, 5), name="Fade Type",
                                      choices=[option.value for option in FrameInterpolation], onRelease=self.set_update_options_flag)
        
        self.updateOptions = False
        self._set_dropdown_options()
        
        # TODO: add delay field
        # Make a frame counter as a button but make it not look like a button
        default_color=(150,150,150)
        self.frame_counter_text: Button = Button(self.window, *self.rel2abs(15, 90, 10, 10), text="0", inactiveColour=default_color, hoverColour=default_color, pressedColour=default_color)
    
    def rel2abs(self, x: float, y: float, width: float, height: float) -> tuple[int,int,int,int]:
        scr_wdt, scr_hgt = self.window.get_size()
        x_abs = int(x*scr_wdt/100)
        y_abs = int(y*scr_hgt/100)
        w_abs = int(width*scr_wdt/100)
        h_abs = int(height*scr_hgt/100)
        return (x_abs, y_abs, w_abs, h_abs)
        
    
    def update_interaction(self, game_event):
        if not self.trackMouseMotion:
            pygame.mouse.get_rel()
        
        if game_event.type == pygame.MOUSEBUTTONDOWN:
            if game_event.button == 2: # middle mouse click
                self.trackMouseMotion = True
            elif game_event.button == 1: # left click
                self.sceneManager.click_mesh(pygame.mouse.get_pos())
        elif game_event.type == pygame.MOUSEBUTTONUP:
            if game_event.button == 2: # middle mouse release
                self.trackMouseMotion = False
        elif self.trackMouseMotion and game_event.type == pygame.MOUSEMOTION:
            mouseMovement = pygame.mouse.get_rel()
            current_scene = self.sceneManager.get_current_scene()
            current_scene.euler_angles[0] -= mouseMovement[1]
            current_scene.euler_angles[1] -= mouseMovement[0]
    
    def draw(self):
        if self.updateOptions:
            self.set_scene_options()
        # make the preview window for the color picker
        pygame.draw.rect(self.window, self.colorPicker.get_color(), self.rel2abs(11, 70, 5, 5))
        pygame.draw.rect(self.window, (0,0,0), self.rel2abs(11, 70, 5, 5), 2)
        
        self.sceneManager.draw()
    
    def _next_scene(self):
        self.sceneManager.next_scene()
        self._set_dropdown_options()
        self.frame_counter_text.setText(str(self.sceneManager._current_scene_idx))
        
        
    def _last_scene(self):
        self.sceneManager.last_scene()
        self._set_dropdown_options()
        self.frame_counter_text.setText(str(self.sceneManager._current_scene_idx))
    
    def _set_dropdown_options(self):
        # cursed method to set dropdown options because for some reason pygame_widgets dropdown doesn't allow you to manually set the dropdown option
        scene = self.sceneManager.get_current_scene()
        for choice in self.fill_dropdown._Dropdown__choices:
            if choice.text.find(scene.fill) != -1:
                self.fill_dropdown.chosen = choice
                break
        
        for choice in self.fade_dropdown._Dropdown__choices:
            if choice.text.find(scene.fade) != -1:
                self.fade_dropdown.chosen = choice
                break
    
    def set_update_options_flag(self):
        self.updateOptions = True    
    
    def set_scene_options(self):
        self.updateOptions = False
        self.sceneManager.update_scene_options(self.fill_dropdown.getSelected(), self.fade_dropdown.getSelected(), 1000)