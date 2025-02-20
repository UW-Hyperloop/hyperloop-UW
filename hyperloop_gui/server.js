/**
 * This is a test server, won't be used in production
 * Set up to test if GUI oculd recieve websocket messgages
 */


const WebSocket = require('ws');

// Create a WebSocket server on port 5000
const wss = new WebSocket.Server({ port: 5000 });
let overAllState = 'Running'
let motorTemp = 25;
let motor_active = true;
let circuitTemp = 25;
let circuit_active = true;
let flow_rate = 25;
let flow_active = true;

wss.on('connection', (ws) => {
  console.log('Client connected');

  // Send a test message every 3 seconds to the connected client
  const sendTestData = setInterval(() => {
    motorTemp = motorTemp + Math.random() * 3 - 1.5;
    circuitTemp = circuitTemp + Math.random() * 3 - 1.5
    flow_rate = flow_rate + Math.random() * 2 - 1
    const testData = JSON.stringify({
      state: overAllState,
      motor_temp: {active: motor_active, value: motorTemp},
      circuit_temp: {active: circuit_active, value: circuitTemp},
      flow: {active: flow_active, value: flow_rate},
      timestamp: new Date().toISOString()
    });

    ws.send(testData); // Send data to the client
  }, 600);

  ws.on('close', () => {
    console.log('Client disconnected');
    clearInterval(sendTestData); // Stop sending data if the client disconnects
  });
});

console.log('WebSocket server running on ws://localhost:5000');
