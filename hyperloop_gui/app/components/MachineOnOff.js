"use client";

import styled from 'styled-components';
import { useEffect, useState } from 'react';

const Container = styled.div`
  margin-right: auto;
  width: 450px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 20px;
  margin-bottom: 20px;
`;

const StatusIndicator = styled.div`
  display: flex;
  align-items: center;
  gap: 10px;
`;

const StatusDot = styled.span`
  width: 12px;
  height: 12px;
  border-radius: 50%;
  background-color: ${props => {
    switch (props.status) {
      case 'config': return '#FFDC2E';
      case 'running': return '#41F21D';
      case 'stopped': return '#797979';
      case 'error': return '#FF4F4F';
      case 'estop': return '#FF4F4F';
      case 'power_failure': return '#FF4F4F';
      case 'comms_failure': return '#FF4F4F';
      default: return '#797979';
    }
  }};
`;

const StatusText = styled.span`
  color: #F3F3F3;
  font-family: 'Helvetica Neue', sans-serif;
  font-size: 20px;
`;

const Button = styled.button`
  background-color: ${props => props.isRunning ? '#FF4F4F' : '#41F21D'};
  color: #000000;
  border: 2px solid transparent;
  padding: 10px 20px;
  border-radius: 4px;
  font-family: 'Inter', sans-serif;
  font-size: 15px;
  cursor: pointer;
  transition: all 0.2s ease-in-out;
  box-sizing: border-box;
  width: 200;
  height: 40px;

  &:hover {
    background-color: transparent;
    border: 2px solid ${props => props.isRunning ? '#FF4F4F' : '#41F21D'};
    color: ${props => props.isRunning ? '#FF4F4F' : '#41F21D'};
  }

  &:disabled {
    background-color: #797979;
    border-color: #797979;
    cursor: not-allowed;
    &:hover {
      background-color: #797979;
      color: #000000;
    }
  }
`;

const MachineOnOff = ({ machineState, startStopToggle }) => {
    const [isRunning, setIsRunning] = useState(machineState === 'running');
  
    const handleMachineToggle = async () => {
      startStopToggle();
    };
  
    useEffect(()=>{
      setIsRunning(machineState === 'running');
    }, [machineState]);
    return (
      <Container>
        <StatusIndicator>
          <StatusDot status={machineState} />
          <StatusText>
            {machineState === 'config' && 'Machine is ready'}
            {machineState === 'running' && 'Machine is active'}
            {machineState === 'stop' && 'Machine is stopped'}
            {machineState === 'error' && 'Machine has error'}
            {machineState === 'estop' && 'Machine Estopped'}
            {machineState === 'power_failure' && 'Machine power failure'}
            {machineState === 'comms_failure' && 'Machine comms failure'}
          </StatusText>
        </StatusIndicator>
        <Button 
          isRunning={isRunning} 
          onClick={handleMachineToggle}
        // this is for if we want to disable the button if there is an error
          disabled={machineState !== 'config' && machineState !== 'running' && machineState !== 'stop'}
        //   disabled={isRunning && machineState === 'error'}
        >
          {isRunning ? 'Stop machine' : 'Start machine'}
        </Button>
      </Container>
    );
  };

  export default MachineOnOff;