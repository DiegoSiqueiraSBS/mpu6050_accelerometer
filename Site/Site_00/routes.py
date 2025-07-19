from main import app
from flask import render_template

#rotas
@app.route("/")
def homepage():
    return render_template("homepage.html", variavel = 5)

@app.route("/blog")
def blog():
    return "Bem vindo ao blog"