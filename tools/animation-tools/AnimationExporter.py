from Scene import Scene
from enum import StrEnum
from pygame.math import Vector3
from Shapes import Mesh

class FillInterpolation(StrEnum):
    NO_FILL = "NO_FILL"
    CLOSEST_COLOR = "CLOSEST_COLOR"
    LINEAR_WEIGHTED_DISTANCE = "LINEAR_WEIGHTED_DISTANCE"
    SQUARE_WEIGHTED_DISTANCE = "SQUARE_WEIGHTED_DISTANCE"

class FrameInterpolation(StrEnum):
    SNAP = "SNAP"
    FADE = "FADE"

class Cell:
    def __init__(self, position: Vector3, color: Vector3):
        self.position: Vector3 = position
        self.color: Vector3 = color
    
    def to_string(self):
        message = f"CreateCell({self.position.x},{self.position.y},{self.position.z}"
        message += f",V3D<uint8_t>({self.color.x},{self.color.y},{self.color.z})),"
        return message

class AnimationFrame:
    def __init__(self, cells: list[Cell], fill_interpolation: FillInterpolation, frame_interpolation: FrameInterpolation, delay: int):
        self.cells = cells
        self.fill = fill_interpolation
        self.frame_interp = frame_interpolation
        self.delay = delay
    
    def to_string(self, frame_number: int):
        cell_str = ""
        for cell in self.cells:
            if cell.color.x != 0 or cell.color.y != 0 or cell.color.z != 0:
                cell_str += cell.to_string() + "\n\t"
        cell_str = cell_str[:-3]
        message = f"""
AnimationFrame frame{frame_number}{{
    .frame = {{
        {cell_str}
    }},
    .fillInterpolation = FillInterpolation::{self.fill},
    .frameInterpolation = FrameInterpolation::{self.frame_interp},
    .delay = std::chrono::milliseconds({self.delay})
}}; 
        """
        return message
        

#TODO: Impliment this
def generate_animation_frame_struct(animation_types_path: str):
    with open(animation_types_path, 'r') as AnimationTypes:
        with open("AnimationTypes.py", 'w') as output:
            pass

def mesh_to_cell(mesh: Mesh) -> Cell:
    if mesh.face_color[0] != 0 or mesh.face_color[1] != 0 or mesh.face_color[2] != 0:
        pass
    pos = (mesh.get_average_position() + Vector3(1,1,1)) / 2
    # need to swap z and y for the coordinate system
    z = pos.y
    pos.y = pos.z
    pos.z = z
    testVector = Vector3(0.1,0.5,0.9)
    
    cell = Cell(pos, Vector3(mesh.face_color))
    return cell

def scene_to_frame(scene: Scene) -> str:
    cells = [mesh_to_cell(cube) for cube in scene.meshes]
    frame = AnimationFrame(cells, FillInterpolation.NO_FILL, FrameInterpolation.FADE, 1000)
    return frame.to_string(0)