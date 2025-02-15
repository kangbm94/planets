import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

def draw_sphere():
    glColor3f(1, 1, 0)  # Yellow sphere
    glutSolidSphere(2, 50, 50)  # Reduce radius for better visibility

# Initialize GLUT (Required for glutSolidSphere)
glutInit()

# Initialize Pygame
pygame.init()
display = (800, 600)
pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

# Enable Depth Testing & Lighting
glEnable(GL_DEPTH_TEST)
glEnable(GL_LIGHTING)
glEnable(GL_LIGHT0)
glEnable(GL_COLOR_MATERIAL)
glShadeModel(GL_SMOOTH)

# Set up Perspective Projection
gluPerspective(45, (display[0] / display[1]), 1, 100)
glTranslatef(0, 0, -5)  # Move camera closer

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    draw_sphere()
    pygame.display.flip()
    pygame.time.wait(10)

