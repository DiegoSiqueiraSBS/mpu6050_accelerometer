from main import app, df
from flask import render_template



x = df['x']

#rotas
@app.route("/")
def homepage():
    return render_template("homepage.html", variavel = x)
