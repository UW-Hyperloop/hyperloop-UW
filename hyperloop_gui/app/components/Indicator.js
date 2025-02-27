"use client";

import styled from 'styled-components';
import { useEffect, useState } from 'react';

const TopSection = styled.div`
  width: 100%;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0 20px;
  margin-bottom: 40px;
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

const Indicators = ({ machineState, startStopToggle }) => {
    const [isRunning, setIsRunning] = useState(machineState === 'running');
  
    const handleMachineToggle = async () => {
      startStopToggle();
    };
  
    useEffect(()=>{
      setIsRunning(machineState === 'running');
    }, [machineState]);
    return (
      <TopSection>
        <StatusIndicator>
          <StatusDot status={machineState} />
          <StatusText>
            {machineState === 'config' && 'Machine is ready'}
            {machineState === 'running' && 'Machine is running'}
            {machineState === 'stopped' && 'Machine is stopped'}
            {machineState === 'error' && 'Machine has error'}
            {machineState === 'estop' && 'Machine Estopped'}
          </StatusText>
        </StatusIndicator>
        <Button 
          isRunning={isRunning} 
          onClick={handleMachineToggle}
        // this is for if we want to disable the button if there is an error
          disabled={machineState === 'error' || machineState === 'estop'}
        //   disabled={isRunning && machineState === 'error'}
        >
          {isRunning ? 'Stop machine' : 'Start machine'}
        </Button>
      </TopSection>
    );
  };

  export default Indicators;