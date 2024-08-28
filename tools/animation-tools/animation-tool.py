import pygame
from itertools import product
import pygame_widgets
from Shapes import *
from Scene import Scene
from CustomWidgets import ColorPicker
from pygame_widgets.button import Button
from AnimationExporter import scene_to_frame
            

# generate a list of cubes
meshes: list[Mesh] = []
for origin in product([-1, 0, 1],[-1, 0, 1],[-1, 0, 1]):
    cube = Cube()
    cube.scale((0.35, 0.35, 0.35))
    cube.set_position(origin)
    cube.set_face_color((0, 0, 0))
    meshes.append(cube)

scene = Scene(meshes, 90, 5)

WINDOW_W = 500
WINDOW_H = 500

def save_frame_to_file(frame_data=""):
    frame_data += scene_to_frame(scene)
    print(frame_data)
    with open("tools/animation-tools/output.txt", 'w') as file:
        file.write(frame_data)

pygame.init()
window = pygame.display.set_mode((WINDOW_W, WINDOW_H))
colorPicker = ColorPicker(window, 20, 20, 100, 300)
save_button = Button(window, 20, WINDOW_H-40, 60, 30, text="Save",onClick=save_frame_to_file)
clock = pygame.time.Clock()
selected_meshes = []
run = True
trackMouseMotion = False

while run:
    clock.tick(60)
    window.fill((255, 255, 255))
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.QUIT:
            run = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 2: # middle mouse click
                trackMouseMotion = True
            elif event.button == 1: # left click
                mousePosition = pygame.mouse.get_pos()
                mesh = scene.get_mesh_from_xy(mousePosition)
                if mesh != None:
                    if mesh in selected_meshes:
                        mesh.set_edge_color((0,0,0))
                        selected_meshes.remove(mesh)
                    else:
                        mesh.set_edge_color((255,0,0))
                        selected_meshes.append(mesh)

                pass
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 2: # middle mouse release
                trackMouseMotion = False
        elif trackMouseMotion and event.type == pygame.MOUSEMOTION:
            mouseMovement = pygame.mouse.get_rel()
            scene.euler_angles[0] -= mouseMovement[1]
            scene.euler_angles[1] -= mouseMovement[0]
    pygame_widgets.update(events)


    if not trackMouseMotion:
        pygame.mouse.get_rel()
    
    for mesh in selected_meshes:
        mesh.set_face_color(colorPicker.get_color())
    scene.draw(window)
    pygame.display.flip()

pygame.quit()