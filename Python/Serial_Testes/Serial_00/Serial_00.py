import serial
import pandas as pd

ser = serial.Serial('COM4', 115200)

df = pd.DataFrame({'t': [], 'x': [], 'y': [], 'z': []})

while True:
    response = ser.readline().decode("utf-8").replace('\r\n', '')

    t, x, y, z = response.split(',')

    df.loc[len(df)] = (t, x, y, z)

    # para adicionar no arquivo, colocar parâmetro mode= 'a'
    df.to_csv("result.csv", sep=";", index=False)

