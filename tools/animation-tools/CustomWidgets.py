from pygame_widgets.slider import Slider

class ColorPicker:
    def __init__(self, screen, x_pos: int, y_pos: int, width: int, height: int):
        width = max(50, width)
        self.sliders: list[Slider] = [
            Slider(screen, int(x_pos + i*(width/3)), y_pos, int((width-50)/3), height, min=0, max=255, step=1, vertical=True) for i in range(3)
            ]
        for slider in self.sliders:
            slider.enable()
    
    def get_color(self) -> tuple[int]:
        # return (0,0,0)
        return tuple([slider.getValue() for slider in self.sliders])

    def set_color(self, color: tuple[int]):
        for i, slider in enumerate(self.sliders):
            slider.setValue(color[i])