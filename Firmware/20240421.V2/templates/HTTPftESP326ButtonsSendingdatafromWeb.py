import os
from flask import Flask, jsonify, request
# , render_template

app = Flask(__name__)

# Use environment variables to get the server's IP/hostname
SERVER_ADDRESS = os.getenv('SERVER_ADDRESS', '0.0.0.0')
SERVER_PORT = os.getenv('SERVER_PORT', '5500')

emergency_level = None
sensorValue = None  # Initialize sensorValue

@app.route('/update', methods=['POST', 'GET'])
def update():
    global sensorValue
    if request.method == 'POST':
        print("TESTING POST REQ .. ")
        sensorValue = request.form.get('value')
        if sensorValue is not None:
            print("POSTING A TEMP VALUE REQUEST:", sensorValue)
            return jsonify({'status': 'success'}), 200  # HTTP 200 OK
        else:
            return jsonify({'status': 'failed', 'error': 'Invalid request'}), 400  # HTTP 400 Bad Request
    elif request.method == 'GET':
        return jsonify({'value': sensorValue})

@app.route('/emergency', methods=['POST', 'GET'])
def emergency():
    global emergency_level
    if request.method == 'POST':
        data = request.json
        emergency_level = data.get('level')
        if emergency_level is not None:
            print("POSTING A NOTIFICATION Emergency level", emergency_level)
        return jsonify({'status': 'success', 'level': emergency_level}), 200
    elif request.method == 'GET':
        print("GETTING Emergency level", emergency_level)
        return jsonify({'level': emergency_level})

# @app.route('/')
# def index():
#     return render_template('index.html', sensorValue=sensorValue, emergency_level=emergency_level)

if __name__ == '__main__':
    app.run(host=SERVER_ADDRESS, port=SERVER_PORT)
