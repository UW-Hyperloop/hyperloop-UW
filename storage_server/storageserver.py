import os
import json
import logging
from flask import Flask, request, jsonify
from flask_socketio import SocketIO, emit
from apscheduler.schedulers.background import BackgroundScheduler
from waitress import serve
from datetime import datetime  # Corrected import

app = Flask(__name__)
socketio = SocketIO(app)

# Load configuration
config = json.load(open('server_config.json'))

esp_config_location = os.path.join(config['config_path'], 'initial_config.json')
esp_config = json.load(open(esp_config_location))

# Setup logger
logging.basicConfig(level=logging.INFO)

werkzeug_logger = logging.getLogger('werkzeug') # Silence Werkzeug's logging
werkzeug_logger.setLevel(logging.WARNING) 

# Setup directory structure
os.makedirs(config['storage_path'], exist_ok=True)
os.makedirs(config['config_path'], exist_ok=True)

@app.route('/report', methods=['POST'])
def report():
    """
    Save POST from ESP to storage_path and forward to frontend
    """
    try:
        data = request.json
        timestamp = datetime.now().strftime('%Y%m%d%H%M%S')
        file_path = os.path.join(config['storage_path'], f'report_{timestamp}.json')
        
        # Save data to file
        with open(file_path, 'w') as file:
            json.dump(data, file)
        logging.info(f"Data saved to {file_path}")
        
        # Emit data to WebSocket clients
        socketio.emit('report', data)
        logging.info("Forwarded report to WebSocket clients")

        return jsonify({'status': 'success'}), 200
    except Exception as e:
        logging.error(f"Error in /report: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/config', methods=['GET'])
def get_config():
    """
    Load contents from './configuration/sample_config.json' and return to requester.
    """
    try:
        logging.info(f"Config data from {esp_config_location} forwarded to requester")
        return jsonify(esp_config), 200
    except Exception as e:
        logging.error(f"Error loading config: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

# SocketIO event handlers
@socketio.on('connect')
def handle_connect():
    logging.info("New client connected to WebSocket")

@socketio.on('disconnect')
def handle_disconnect():
    pass

@socketio.on('new_config')
def handle_new_config(data):
    """
    Saves a new configuration to config_path
    """
    global esp_config_location, esp_config
    try:
        timestamp = datetime.now().strftime('%Y%m%d%H%M%S')
        
        esp_config_location = os.path.join(config['config_path'], f'report_{timestamp}.json')
        with open(esp_config_location, 'w') as file:
            json.dump(data, file)
        esp_config = data
        logging.info(f"New configuration saved to {esp_config_location}")
    except Exception as e:
        logging.error(f"Error saving new configuration: {str(e)}. Reverting Config to Initial")
        esp_config_location = os.path.join(config['config_path'], 'initial_config.json')
        esp_config = json.load(open(esp_config_location))
        

if __name__ == "__main__":
    # Run the Flask app with SocketIO using Waitress for production performance
    logging.info(f"Starting server on http://0.0.0.0:{config['server_port']}")
    socketio.run(app, host='0.0.0.0', port=config['server_port'])
