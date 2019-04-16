from flask import Flask
from flask import request
app = Flask(__name__)

@app.route('/hello')
def hello():
    return "Hello, World [Maldonado]!\n"

@app.route('/user')
def get_user():
    name = request.args.get('name')
    return "Requested for name = %s\n" % name

app.run(port=3000)