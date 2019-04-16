from flask import Flask
app = Flask(__name__)

@app.route('/hello')
def hello():
    return "Hello, World [Maldonado]!\n"

app.run(port=3000)