import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("Arduino_Code\\leituras_mpu6050_06\\90_graus.csv", sep=',')

t = df['ax']
x = df['low-pass']
y = df['high-pass']

plt.figure(figsize=(15,8))

plt.plot(t, label='Ax', color='blue')
plt.plot(y, label='high-pass', color='orange')
plt.plot(x, label='low-pass', color='red')

plt.grid()
plt.title('Gráfico da aceleração em função do tempo')
plt.ylabel('Aceleração (g)')
plt.legend()

plt.tight_layout()
plt.show()