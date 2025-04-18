"use client";

import { useState, useEffect, useRef } from 'react';
import styled, { css } from 'styled-components';
import MachineOnOff from "./components/MachineOnOff";
import MotorTempGauge from "./components/MotorTemp";

// PageContainer now supports a prop to grey itself out in comms_failure
const PageContainer = styled.div`
  background-color: #1C1C1C;
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: center;
  padding-top: 20px;
  gap: 20px;
  position: relative;

  ${(props) =>
    props.isCommsFailure &&
    css`
      opacity: 0.5;
      pointer-events: none
    `}
`;

// A bigger reset button
const ResetButton = styled.button`
  font-size: 1.5rem;
  padding: 16px 24px;
  margin-top: 16px;
  background-color: #ff6600;
  color: #fff;
  border: none;
  border-radius: 8px;
  cursor: pointer;
`;

const Overlay = styled.div`
  position: absolute;
  inset: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #ffffff;
  font-size: 1.6rem;
  background-color: rgba(0, 0, 0, 0.5);
  pointer-events: auto;
  z-index = 9999;
`;

const GasSensorPanel = styled.div`
  position: absolute;
  top: 20px;
  right: 20px;
  padding: 16px 24px;
  border-radius: 10px;
  font-size: 1.25rem;
  font-weight: bold;
  color: #fff;
  ${(props) =>
    props.gasValue === 0
      ? css`
          background-color: green;
        `
      : css`
          background-color: red;
        `}
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
  const [gasPPM, setGasPPM] = useState(0); // 0 or 1
  const [machineState, setMachineState] = useState('config');
  const [bentoniteState, setBentoniteState] = useState('off');

  const socketRef = useRef(null);

  // Connect to server and listen
  useEffect(() => {
    const socket = new WebSocket('ws://192.168.0.111:8765'); 
    socketRef.current = socket;

    socket.onmessage = function (event) {
      const data = JSON.parse(event.data);
      if (data.state) {
        setMachineState(data.state);
      }
      if (data.bentonite_state) {
        setBentoniteState(data.bentonite_state);
      }
      if (data.motor_temp?.value !== undefined) {
        setMotorTemp(Math.floor(data.motor_temp.value * 100) / 100);
      }
      if (data.gas_sensor?.value !== undefined) {
        setGasPPM(data.gas_sensor.value);
      }
    };

    socket.onclose = function () {
      console.log('WebSocket connection closed');
    };

    return () => {
      // socket.close();
    };
  }, []);

  // Send command to server
  const sendCommandToServer = (command) => {
    if (socketRef.current && socketRef.current.readyState === WebSocket.OPEN) {
      const message = JSON.stringify({ command });
      socketRef.current.send(message);
      console.log("Sent command to server:", message);
    } else {
      console.warn("WebSocket is not open. Cannot send:", command);
    }
  };

  // Start/Stop
  const toggleStartStop = () => {
    if (machineState === 'config' || machineState === 'stop') {
      sendCommandToServer("tbm_start");
      // setMachineState('running');
    } else if (machineState === 'running') {
      setMachineState('stop');
      sendCommandToServer("tbm_stop");
      if (bentoniteState === 'on') {
        setBentoniteState('off');
        sendCommandToServer("TBM_bentonite_stop");
      }
    }
  };

  // If 'stopped', auto-reset to 'config' after 2s
  useEffect(() => {
    if (machineState === 'stop') {
      const timer = setTimeout(() => {
        // setMachineState('config');
      }, 2000);
      return () => clearTimeout(timer);
    }
  }, [machineState]);

  // The "comms failure" overlay logic
  const isCommsFailure = (machineState === 'comms_failure');

  // The bigger "Reset" button
  const handleReset = () => {
    // 1) Send "TBM_reset" command to the server
    sendCommandToServer("tbm_reset");
    // 2) Locally set machineState to config
    setMachineState('config');
  };

  // Gas sensor text
  const gasStatusText = gasPPM === 0 ? 'Gas Status: Safe' : 'Gas Status: Danger';

  return (
    <PageContainer isCommsFailure={isCommsFailure}>
      {isCommsFailure && (
        <Overlay>
          <div>Machine has stale data</div>
          <ResetButton onClick={handleReset}>
            RESET
          </ResetButton>
        </Overlay>
      )}

      <GasSensorPanel gasValue={gasPPM}>
        {gasStatusText}
      </GasSensorPanel>

      <Buttons>
        <MachineOnOff 
          machineState={machineState} 
          startStopToggle={toggleStartStop}
        />
      </Buttons>

      <Row>
        <MotorTempGauge 
          value={motorTemp}
          onChange={setMotorTemp}
          machineState={machineState}
        />
      </Row>
    </PageContainer>
  );
}
