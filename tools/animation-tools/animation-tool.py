import pygame
import pygame_widgets
from Shapes import *
from UI import AnimatorUI

WINDOW_W = 500
WINDOW_H = 500


file_data = ""

pygame.init()
window = pygame.display.set_mode((WINDOW_W, WINDOW_H))

ui = AnimatorUI(window)

clock = pygame.time.Clock()
run = True

while run:
    clock.tick(60)
    window.fill((255, 255, 255))
    
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.QUIT:
            run = False
        # if the event isn't handled above as a global event, let the ui handle it
        else:
            ui.update_interaction(event)
    
    pygame_widgets.update(events)
    
    ui.draw()
    pygame.display.flip()

pygame.quit()