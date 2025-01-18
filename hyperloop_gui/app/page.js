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
  const [motorTemp, setMotorTemp] = useState(9.5);
  const [waterFlowInRate, setWaterFlowInRate] = useState(96);
  const [waterFlowOutRate, setWaterFlowOutRate] = useState(96);
  const [waterPumpTemp, setWaterPumpTemp] = useState(9.5);
  const [machineState, setMachineState] = useState('ready');

  useEffect(() => {
    const checkMachineStatus = async () => {
      if (motorTemp > 40 || waterFlowInRate > 115) {
        setMachineState('error');
      } else {
        setMachineState('ready');
      }
    };

    const timer = setTimeout(checkMachineStatus, 1600);
    return () => clearTimeout(timer);
  }, [motorTemp, waterFlowInRate]);

  return (
    <PageContainer>
      <StatusIndicators machineState={machineState} />
      <RowOne>
        <WaterPumpGauge 
          value={waterPumpTemp} 
          onChange={setWaterPumpTemp} 
        />
        <MotorTempGauge 
          value={motorTemp} 
          onChange={setMotorTemp} 
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