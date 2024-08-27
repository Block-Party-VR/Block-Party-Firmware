from Shapes import Mesh
from MatrixMath import *
import pygame

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