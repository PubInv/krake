from flask import Flask, request, jsonify, render_template
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# Global variables to store sensor value and emergency level
emergency_level = None
sensorValue = None

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
        # Handle case where sensorValue is not yet set
        if sensorValue is None:
            return jsonify({'value': 'No data available'}), 200
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
        else:
            return jsonify({'status': 'failed', 'error': 'Invalid request'}), 400  # Handle invalid request
    elif request.method == 'GET':
        # Handle case where emergency_level is not yet set
        if emergency_level is None:
            return jsonify({'level': 'No data available'}), 200
        print("GETTING Emergency level", emergency_level)
        return jsonify({'level': emergency_level})


@app.route("/foo") 
@app.route("/index") 
@app.route("/") 
def hello(): 
    message = "Hello, World"
    return render_template('index.html', message=message) 
    # return render_template(message=message) 



if __name__ == '__main__':
    # app.run(host='192.168.1.9', port=5500, debug=True)  # Bind to all interfaces, debug mode enabled
    app.run(host='192.168.1.137', port=5500, debug=True)  # Bind to all interfaces, debug mode enabled

