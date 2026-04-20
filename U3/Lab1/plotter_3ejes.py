import sys
import matplotlib.pyplot as plt
from collections import deque

MAX_PUNTOS = 100

x_data = deque(maxlen=MAX_PUNTOS)
y_data = deque(maxlen=MAX_PUNTOS)
z_data = deque(maxlen=MAX_PUNTOS)

# Configura gráfico interactivo
plt.ion()
fig, ax = plt.subplots()
plt.show()

line_x, = ax.plot([], [], label="X")
line_y, = ax.plot([], [], label="Y")
line_z, = ax.plot([], [], label="Z")

ax.set_ylim(-10, 10)
ax.set_xlim(0, MAX_PUNTOS)
ax.legend()
ax.set_title("Aceleración en tiempo real")
ax.set_xlabel("Tiempo")
ax.set_ylabel("Valor")

for line in sys.stdin:
    try:
        # Parsear CSV
        x, y, z = map(float, line.strip().split(","))

        # Agregar a buffers
        x_data.append(x)
        y_data.append(y)
        z_data.append(z)

        # Actualizar datos del gráfico
        line_x.set_data(range(len(x_data)), x_data)
        line_y.set_data(range(len(y_data)), y_data)
        line_z.set_data(range(len(z_data)), z_data)

        ax.set_xlim(0, MAX_PUNTOS)

        # Redibujar
        plt.draw()
        plt.pause(0.01)

    except ValueError:
        # Ignorar líneas mal formadas
        continue
