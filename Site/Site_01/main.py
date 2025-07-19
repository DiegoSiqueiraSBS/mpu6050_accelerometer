from flask import Flask

import pandas as pd

df = pd.read_csv("result.csv")

app = Flask(__name__)

from routes import *

if __name__ == "__main__":
    app.run()