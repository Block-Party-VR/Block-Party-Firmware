import math
import pygame
from itertools import product
from pygame_widgets.slider import Slider
import pygame_widgets

def project(vector, w, h, fov, distance):
    factor = math.atan(fov / 2 * math.pi / 180) / (distance + vector.z)
    x = vector.x * factor * w + w / 2
    y = -vector.y * factor * w + h / 2
    return pygame.math.Vector3(x, y, vector.z)

def rotate_vertices(vertices, angle, axis):
    return [v.rotate(angle, axis) for v in vertices]
def scale_vertices(vertices, s):
    return [pygame.math.Vector3(v[0]*s[0], v[1]*s[1], v[2]*s[2]) for v in vertices]
def translate_vertices(vertices, t):
    return [v + pygame.math.Vector3(t) for v in vertices]
def project_vertices(vertices, w, h, fov, distance):
    return [project(v, w, h, fov, distance) for v in vertices]

class Mesh():
    def __init__(self, vertices, faces):
        self.__vertices = [pygame.math.Vector3(v) for v in vertices]
        self.__faces = faces
        self.face_color = (0, 0, 0)
        self.edge_color = (0,0,0)

    def rotate(self, angle, axis):
        self.__vertices = rotate_vertices(self.__vertices, angle, axis)
    def scale(self, s):
        self.__vertices = scale_vertices(self.__vertices, s)
    def translate(self, t):
        self.__vertices = translate_vertices(self.__vertices, t)

    def calculate_average_z(self, vertices):
        return [(i, sum([vertices[j].z for j in f]) / len(f)) for i, f in enumerate(self.__faces)]

    def get_face(self, index):
        return self.__faces[index]
    def get_vertices(self):
        return self.__vertices

    def create_polygon(self, face, vertices):
        return [(vertices[i].x, vertices[i].y) for i in [*face, face[0]]]
    
    def set_face_color(self, color : tuple[int]):
        self.face_color = color

    def set_edge_color(self, color : tuple[int]):
        self.edge_color = color
       
class Scene:
    def __init__(self, meshes, fov, distance):
        self.meshes: list[Mesh] = meshes
        self.fov = fov
        self.distance = distance 
        self.euler_angles = [0, 0, 0]

    def transform_vertices(self, vertices, width, height):
        transformed_vertices = vertices
        axis_list = [(1, 0, 0), (0, 1, 0), (0, 0, 1)]
        for angle, axis in reversed(list(zip(list(self.euler_angles), axis_list))):
            transformed_vertices = rotate_vertices(transformed_vertices, angle, axis)
        transformed_vertices = project_vertices(transformed_vertices, width, height, self.fov, self.distance)
        return transformed_vertices

    def point_in_polygon(self, point, polygon):
        """ Determine if the point (x, y) is inside the polygon """
        x, y = point
        n = len(polygon)
        inside = False
        p1x, p1y = polygon[0]
        for i in range(n + 1):
            p2x, p2y = polygon[i % n]
            if y > min(p1y, p2y):
                if y <= max(p1y, p2y):
                    if x <= max(p1x, p2x):
                        if p1y != p2y:
                            xints = (y - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
                        if p1x == p2x or x <= xints:
                            inside = not inside
            p1x, p1y = p2x, p2y
        return inside

    def get_mesh_from_xy(self, pos: tuple[int]) -> Mesh:
        x, y = pos
        closest_mesh = None
        closest_z = float('inf')

        for mesh in self.meshes:
            transformed_vertices = self.transform_vertices(mesh.get_vertices(), *pygame.display.get_surface().get_size())
            avg_z = mesh.calculate_average_z(transformed_vertices)
            for i, avg_z_value in avg_z:
                polygon = mesh.create_polygon(mesh.get_face(i), transformed_vertices)
                if self.point_in_polygon((x, y), polygon):
                    if avg_z_value < closest_z:
                        closest_z = avg_z_value
                        closest_mesh = mesh

        return closest_mesh

    def draw(self, surface):
        
        polygons = []
        for mesh in self.meshes:
            transformed_vertices = self.transform_vertices(mesh.get_vertices(), *surface.get_size())
            avg_z = mesh.calculate_average_z(transformed_vertices)
            for z in avg_z:
            #for z in sorted(avg_z, key=lambda x: x[1], reverse=True):
                pointlist = mesh.create_polygon(mesh.get_face(z[0]), transformed_vertices)
                polygons.append((pointlist, z[1], mesh.face_color, mesh.edge_color))
                #pygame.draw.polygon(surface, (128, 128, 192), pointlist)
                #pygame.draw.polygon(surface, (0, 0, 0), pointlist, 3)

        for poly in sorted(polygons, key=lambda x: x[1], reverse=True):
            pygame.draw.polygon(surface, poly[2], poly[0])
            pygame.draw.polygon(surface, poly[3], poly[0], 3)
            


class Cube(Mesh):
    def __init__(self):
        vertices = [(-1,-1,1), (1,-1,1), (1,1,1), (-1,1,1), (-1,-1,-1), (1,-1,-1), (1,1,-1), (-1,1,-1)]
        faces = [(0,1,2,3), (1,5,6,2), (5,4,7,6), (4,0,3,7), (3,2,6,7), (1,0,4,5)]
        super().__init__(vertices, faces)

    def set_position(self, position : tuple):
        super().translate(position)

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

meshes: list[Cube] = []
for origin in product([-1, 0, 1],[-1, 0, 1],[-1, 0, 1]):
    cube = Cube()
    cube.scale((0.35, 0.35, 0.35))
    cube.set_position(origin)
    cube.set_face_color((0, 0, 0))
    meshes.append(cube)

scene = Scene(meshes, 90, 5)


pygame.init()
window = pygame.display.set_mode((500, 500))
colorPicker = ColorPicker(window, 20, 20, 100, 300)
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