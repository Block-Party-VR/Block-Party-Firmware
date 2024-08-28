from MatrixMath import *
from pygame.math import Vector3

class Mesh():
    def __init__(self, vertices, faces):
        self.__vertices = [Vector3(v) for v in vertices]
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

    def get_average_position(self):
        vertex_sum: Vector3 = Vector3()
        for vertex in self.__vertices:
            vertex_sum += vertex
        return vertex_sum / len(self.__vertices)
    
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

class Cube(Mesh):
    def __init__(self):
        vertices = [(-1,-1,1), (1,-1,1), (1,1,1), (-1,1,1), (-1,-1,-1), (1,-1,-1), (1,1,-1), (-1,1,-1)]
        faces = [(0,1,2,3), (1,5,6,2), (5,4,7,6), (4,0,3,7), (3,2,6,7), (1,0,4,5)]
        super().__init__(vertices, faces)

    def set_position(self, position : tuple):
        super().translate(position)