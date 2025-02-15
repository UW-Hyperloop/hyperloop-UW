"use client";

import { useState, useEffect } from 'react';
import WaterPumpGauge from "./components/WaterPump";
import MotorTempGauge from "./components/MotorTemp";
import styled from 'styled-components';
import WaterFlowGauge from "./components/WaterFlow";
import StatusIndicators from "./components/Indicator";
import ErrorMessages from "./components/ErrorMessages";
import FlowMeter from './components/FlowMeter';

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

const RowOne = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;
  width: 100%;
  gap: 100px;
`;

export default function Page() {
  const [motorTemp, setMotorTemp] = useState(20);
  const [waterFlowInRate, setWaterFlowInRate] = useState(96);
  const [waterFlowOutRate, setWaterFlowOutRate] = useState(96);
  const [waterPumpTemp, setWaterPumpTemp] = useState(20);
  const [machineState, setMachineState] = useState('config');

  /*
  useEffect(() => {
    // Step 1: Connect to the WebSocket server
    const socket = new WebSocket('ws://localhost:5000'); // Adjust URL if necessary
 
    // Step 2: Handle incoming WebSocket messages
    socket.onmessage = function (event) {
      console.log(event);
      const data = JSON.parse(event.data); // Parse the JSON data
      const newMotorTemp = data.motor_temp.value;
      if(newMotorTemp){
        setMotorTemp(Math.floor(newMotorTemp * 100) / 100);
      }
      const newCircuitTemp = data.circuit_temp.value;
      if(newCircuitTemp){
        setCircuitTemp(Math.floor(newCircuitTemp * 100) / 100);
      }
      const newFlow = data.flow.value;
      if(newFlow){
        setData(Math.floor(newFlow * 100) / 100);
      }
    };
 
    // Step 3: Handle WebSocket connection close
    socket.onclose = function () {
      console.log('WebSocket connection closed');
    };
 
    // Step 4: Clean up WebSocket connection on component unmount
    return () => {
      socket.close();
    };
  }, []);
  */
 
  useEffect(() => {
    if (motorTemp < 10 || motorTemp > 50 || waterFlowInRate < 40 || waterFlowInRate > 200) {
      setMachineState('error');
    }
  }, [motorTemp, waterFlowInRate]);

  // sends certain commands to server
  const sendCommandToServer = (command) => {
    // TODO: format and send command to server
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
      alert("stopping and resetting machine")
    }
  };

  // mock reset machine 
  useEffect(() => {
    console.log(machineState);
    if(machineState === 'stopped'){
      setTimeout(() => {
        console.log("machine reset");
        setMachineState('config');
      }, 3000);
    }
  }, [machineState]);
  return (
    <PageContainer>
      <StatusIndicators machineState={machineState} startStopToggle={toggleStartStop}/>
      <RowOne>
        <FlowMeter 
          title={"Pump Flow Rate"}
          value={waterFlowInRate} 
          onChange={setWaterFlowInRate}
          machineState={machineState}
          max={250}
        />
        <MotorTempGauge 
          value={motorTemp} 
          onChange={setMotorTemp} 
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
      </RowOne>
      {/**
      <WaterFlowGauge 
        direction="IN" 
        value={waterFlowInRate} 
        onChange={setWaterFlowInRate}
      />
      <WaterFlowGauge 
        direction="OUT" 
        value={waterFlowOutRate} 
        onChange={setWaterFlowOutRate}
      />
       */}
      
      <ErrorMessages 
        motorTemp={motorTemp}
        waterFlowRate={waterFlowInRate}
      />
    </PageContainer>
  );
}