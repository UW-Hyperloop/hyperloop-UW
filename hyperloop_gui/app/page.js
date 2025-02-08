"use client";

import { useState, useEffect } from 'react';
import WaterPumpGauge from "./components/WaterPump";
import MotorTempGauge from "./components/MotorTemp";
import styled from 'styled-components';
import WaterFlowGauge from "./components/WaterFlow";
import StatusIndicators from "./components/Indicator";
import ErrorMessages from "./components/ErrorMessages";

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

  useEffect(() => {
    if (motorTemp < 10 || motorTemp > 50 || waterFlowInRate < 20 || waterFlowInRate > 120) {
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
        <WaterPumpGauge 
          value={waterPumpTemp} 
          onChange={setWaterPumpTemp} 
        />
        <MotorTempGauge 
          value={motorTemp} 
          onChange={setMotorTemp} 
          machineState={machineState}
        />
      </RowOne>
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
      <ErrorMessages 
        motorTemp={motorTemp}
        waterFlowRate={waterFlowInRate}
      />
    </PageContainer>
  );
}