"use client";

import { useState, useEffect, useRef } from 'react';
import PumpTempMeter from './components/PumpTemp';
import MotorTempGauge from "./components/MotorTemp";
import GasSensor from './components/GasSensor';
import styled from 'styled-components';
import MachineOnOff from "./components/MachineOnOff";
import ErrorMessages from "./components/ErrorMessages";
import BentoniteOnOff from './components/BentoniteOnOff';

const PageContainer = styled.div`
  background-color: #1C1C1C;
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: center;
  padding-top: 20px;
  gap: 20px;
`;

const Row = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;
  width: 100%;
  gap: 60px;
`;

const Buttons = styled.div`
  display: flex;
  flex-direction: row;
  width: 100%;
`;

export default function Page() {
  const [motorTemp, setMotorTemp] = useState(20);
  const [pumpTemp, setPumpTemp] = useState(20);
  const [gasPPM, setGasPPM] = useState(23);
  const [machineState, setMachineState] = useState('config');
  const [bentoniteState, setBentoniteState] = useState('off');
  const [pumpOn, setPumpOn] = useState(false);
  const socketRef = useRef(null);
  
  useEffect(() => {
    // Step 1: Connect to the WebSocket server
    const socket = new WebSocket('ws://localhost:8765'); // Adjust URL if necessary
    socketRef.current = socket;
    // Step 2: Handle incoming WebSocket messages
    socket.onmessage = function (event) {
      console.log(event);
      const data = JSON.parse(event.data); // Parse the JSON data
      if(data.state){
        setMachineState(data.state);
      }
      if(data.bentonite_state){
        setBentoniteState(data.bentonite_state);
      }
      const newMotorTemp = data.motor_temp?.value;
      if(newMotorTemp){
        setMotorTemp(Math.floor(newMotorTemp * 100) / 100);
      }
      const newGasPPM = data.gas_ppm?.value;
      if(newGasPPM){
        setGasPPM(newGasPPM);
      }
    };
 
    // Step 3: Handle WebSocket connection close
    socket.onclose = function () {
      console.log('WebSocket connection closed');
    };
 
    // Step 4: Clean up WebSocket connection on component unmount
    return () => {
      //socket.close();
    };
  }, []);
  
 

  // sends certain commands to server
  const sendCommandToServer = (command) => {
    if (socketRef.current && socketRef.current.readyState === WebSocket.OPEN) {
      const message = JSON.stringify({ command });
      socketRef.current.send(message);
      console.log(" Sent command to server:", message);
    } else {
      console.warn(" WebSocket is not open. Cannot send:", command);
    }
    console.log("sending command: " + command);
  };

  // start stop button handling
  const toggleStartStop = () => {
    if(machineState === 'config'){
      //start machine
      setMachineState('running');
      sendCommandToServer("TBM_start")
    }
    else if(machineState === 'running'){
      setMachineState('stopped');
      sendCommandToServer("TBM_stop");
      if(bentoniteState === 'on'){
        setBentoniteState('off');
        sendCommandToServer("TBM_bentonite_stop");
      }
    }
  };

  const toggleBentonite = () => {
    if(machineState === 'running'){
      if(bentoniteState === 'off'){
        sendCommandToServer("TBM_bentonite_start");
        setBentoniteState('on');
      }
      else {
        sendCommandToServer("TBM_bentonite_stop");
        setBentoniteState('off');
      }
    }
  }

  // mock reset machine 
  useEffect(() => {
    console.log(machineState);
    if(machineState === 'stopped'){
      setTimeout(() => {
        console.log("machine reset");
        setMachineState('config');
      }, 2000);
    }
  }, [machineState]);
  return (
    <PageContainer>
      <Buttons>
        <MachineOnOff machineState={machineState} startStopToggle={toggleStartStop}/>
        <BentoniteOnOff bentoniteState={bentoniteState} machineState={machineState} startStopToggle={toggleBentonite}/>
      </Buttons>
      <Row>
        <MotorTempGauge 
          value={motorTemp} 
          onChange={setMotorTemp} 
          machineState={machineState}
        />
        <GasSensor 
          value={gasPPM} 
          onChange={setGasPPM} 
          machineState={machineState}
        />
        {/**
        colorRanges={[ fahrenheit 
          { min: 40, max: 65, color: 'red' },      // Critical low
          { min: 65, max: 75, color: 'yellow' },  // Warning low
          { min: 75, max: 85, color: 'green' },   // Safe range
          { min: 85, max: 90, color: 'yellow' },  // Warning high
          { min: 90, max: 110, color: 'red' },     // Critical high
        ]} */}
      </Row>    
    </PageContainer>
  );
}