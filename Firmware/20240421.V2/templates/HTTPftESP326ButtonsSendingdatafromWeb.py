from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import time

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})

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


@app.route('/')
def index():
    return render_template('index.html', sensorValue=sensorValue, emergency_level=emergency_level)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5500)
