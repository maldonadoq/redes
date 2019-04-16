from flask import Flask
from flask import request
app = Flask(__name__)

@app.route('/hello')
def hello():
    return "Hello, World [Maldonado]!\n"

@app.route('/user', methods=['GET'])
def get_user():
    name = request.args.get('name')
    return "Requested for name = %s\n" % name

@app.route('/login', methods=['POST'])
def login():
    username = request.form.get('username')
    password = request.form.get('password')
    return "Login successful for %s: %s\n" % (username, password)

app.run(port=3000)


"""
	POST /login HTTP/1.1
	Content-Type: application/x-www-form-urlencoded
	Content-Length: 32

	username=percy&password=maldonado
"""

# traceroute 185.164.57.189	- France
# traceroute 186.27.32.34	- Bolivia


