from AnimationExporter import scene_to_frame
from CustomWidgets import ColorPicker
from itertools import product
from pygame_widgets.button import Button
from Scene import Scene
from Shapes import *
import pygame

class SceneManager:
    def __init__(self, window, color_picker: ColorPicker):     
        self.file_data: str = ""
        self._scenes: list[Scene] = [self.new_scene()]
        self._current_scene_idx: int = 0
        self.window = window
        self.color_picker = color_picker
        self._selected_meshes: list[Mesh] = []
    
    def save_frame_to_file(self):
        with open("tools/animation-tools/output.txt", 'w') as file:
            for i, scene in enumerate(self._scenes):
                file.write(scene_to_frame(scene, i))

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
        self.get_current_scene().draw(self.window)
    
    def get_current_scene(self) -> Scene:
        return self._scenes[self._current_scene_idx]
    
    def click_mesh(self, coordinates: tuple[int, int]):
        mesh = self.get_current_scene().get_mesh_from_xy(coordinates)
        
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
        current_angles = self.get_current_scene().euler_angles
        if len(self._scenes)-1 == self._current_scene_idx:
            self._scenes.append(self.new_scene())
            
        self._current_scene_idx += 1
        
        self.get_current_scene().euler_angles = [angle for angle in current_angles]
    
    def previous_scene(self):
        if self._current_scene_idx > 0:
            current_angles = self.get_current_scene().euler_angles
            self._current_scene_idx -= 1
            self.get_current_scene().euler_angles = [angle for angle in current_angles]
            self.deselect_all_mesh()

class AnimatorUI:
    def __init__(self, window):
        scr_wdt, scr_hgt = window.get_size()
        
        self.window = window
        colorPicker: ColorPicker = ColorPicker(self.window, 20, 20, 100, 300)
        self.sceneManager: SceneManager = SceneManager(window, colorPicker)
        
        
        self.save_button: Button = Button(self.window, 20, scr_hgt-40, 60, 30, text="Save",onClick=self.sceneManager.save_frame_to_file)
        self.next_frame_button: Button = Button(self.window, scr_wdt-120, scr_hgt-40, 120, 30, text="Next Frame",onClick=self.sceneManager.next_scene)
        self.last_frame_button: Button = Button(self.window, scr_wdt-240, scr_hgt-40, 120, 30, text="last Frame",onClick=self.sceneManager.previous_scene)
        self.trackMouseMotion: bool = False
    
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
        self.sceneManager.draw()