import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("result.csv", sep=';')

t = df['t']
x = df['x']
y = df['y']
z = df['z']

plt.figure(figsize=(10,8))

plt.plot(t,x, label='Aceleração no eixo X', color='blue')
plt.plot(t,y, label='Aceleração no eixo Y', color='red')
plt.plot(t,z, label='Aceleração no eixo Z', color='orange')

plt.grid()
plt.title('Gráfico da aceleração em função do tempo')
plt.xlabel('Tempo (s)')
plt.ylabel('Aceleração (g)')
plt.legend()

plt.tight_layout()
plt.show()