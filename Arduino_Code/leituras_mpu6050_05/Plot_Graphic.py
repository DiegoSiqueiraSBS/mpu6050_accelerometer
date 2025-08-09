import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("Arduino_Code\\leituras_mpu6050_05\\A_B_C_A.csv", sep=',')

t = df['ax']
x = df['newAx']
y = df['lin_ax']
z = df['new_lin_ax']

plt.figure(figsize=(15,8))

plt.plot(t, label='Ax', color='blue')
plt.plot(x, label='newAx', color='red')
plt.plot(y, label='lin_ax', color='orange')
plt.plot(z, label='new_lin_ax', color='green')

plt.grid()
plt.title('Gráfico da aceleração em função do tempo')
plt.xlabel('Tempo (s)')
plt.ylabel('Aceleração (g)')
plt.legend()

plt.tight_layout()
plt.show()