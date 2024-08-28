import pygame
import pygame_widgets
from Shapes import *
from Scene import Scene
from UI import create_ui, SceneManager

WINDOW_W = 500
WINDOW_H = 500


file_data = ""

pygame.init()
window = pygame.display.set_mode((WINDOW_W, WINDOW_H))

sceneManager: SceneManager = create_ui(window, WINDOW_W, WINDOW_H)

clock = pygame.time.Clock()
selected_meshes = []
run = True
trackMouseMotion = False

while run:
    clock.tick(60)
    window.fill((255, 255, 255))
    current_scene: Scene = sceneManager.get_current_scene()
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.QUIT:
            run = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 2: # middle mouse click
                trackMouseMotion = True
            elif event.button == 1: # left click
                sceneManager.click_mesh(pygame.mouse.get_pos())
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 2: # middle mouse release
                trackMouseMotion = False
        elif trackMouseMotion and event.type == pygame.MOUSEMOTION:
            mouseMovement = pygame.mouse.get_rel()
            current_scene.euler_angles[0] -= mouseMovement[1]
            current_scene.euler_angles[1] -= mouseMovement[0]
    pygame_widgets.update(events)


    if not trackMouseMotion:
        pygame.mouse.get_rel()
    
    sceneManager.draw()
    pygame.display.flip()

pygame.quit()