import React from "react";
import styled from "styled-components";
import { useEffect, useState } from "react";

const Container = styled.div`
  background: #1C1C1C;
  padding: 20px;
  width: 100%;
  max-width: 600px;
`;

const GaugeContainer = styled.div`
  position: relative;
  width: 100%;
  margin-bottom: 20px;
`;

const RedBackground = styled.div`
  position: absolute;
  top: 0;
  left: 80%;
  right: ${props => 100 - (props.temperature / 50 * 100)}%;
  bottom: 0;
  background: #FF4F4F;
  opacity: 0.3;
  transition: right 0.1s ease-in-out;
  pointer-events: none;
`;

const GaugeScale = styled.div`
  height: 30px;
  background: #1C1C1C;
  border: 1px solid #ffffff;
  position: relative;
  margin-top: 45px;
`;

const GridLines = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  display: grid;
  grid-template-columns: repeat(50, 1fr);

  div {
    border-right: 1px dashed rgba(255, 255, 255, 0.1);
    height: 100%;

    &:nth-child(5n):not(:nth-child(40)) {
      border-right: 1px solid #ffffff;
    }
  }
`;

const Marker = styled.div`
  position: absolute;
  top: -55px;
  left: ${props => (props.position / 50) * 100}%;
  transform: translateX(-50%);
  transition: left 0.1s ease-in-out;
  display: flex;
  flex-direction: column;
  align-items: center;
  z-index: 2;
  
  &::before {
    content: '${props => props.position}';
    color: ${props => props.isStale ? '#9F9F9F' : props.position > 40 ? '#FF4F4F' : '#00ff00'};
    display: block;
    margin-bottom: 10px;
    font-size: 20px;
    font-weight: bold;
    position: absolute;
    top: 0;
    left: 50%;
    transform: translateX(-50%);
    transition: color 0.1s ease-in-out;
  }
  
  &::after {
    content: '';
    display: block;
    border-left: 8px solid transparent;
    border-right: 8px solid transparent;
    border-top: 25px solid ${props => props.isStale ? '#9F9F9F' : props.position > 40 ? '#FF4F4F' : '#00ff00'};
    position: absolute;
    top: 25px;
    transition: border-top-color 0.1s ease-in-out;
  }
`;

const ScaleMarkers = styled.div`
  position: absolute;
  width: 100%;
  height: 100%;
  top: -15px;
  bottom: 0;
  
  div {
    position: absolute;
    width: 2px;
    background: #00ff00;
    height: calc(100% + 15px);
    
    &::before {
      content: attr(data-value);
      position: absolute;
      top: -20px;
      left: 50%;
      transform: translateX(-50%);
      color: #00ff00;
      font-size: 14px;
    }
    
    &:first-child {
      left: 0;
    }
    
    &:last-child {
      left: 80%;
    }
  }
`;

const ScaleNumbers = styled.div`
  position: relative;
  color: #9F9F9F;
  margin-top: 10px;
  width: 100%;
  height: 20px;

  span {
    position: absolute;
    transform: translateX(-50%);
    font-size: 14px;
    white-space: nowrap;
  }
`;

const Title = styled.div`
  color: white;
  font-size: 16px;
  margin-bottom: 60px;
`;
const StatusMessage = styled.div`
  color: #FFFFFF;
  font-size: 14px;
  margin-top: 5px;
  text-align: center;
`;

const Slider = styled.input`
  width: 100%;
  margin: 20px 0;
  -webkit-appearance: none;
  background: transparent;

  &::-webkit-slider-thumb {
    -webkit-appearance: none;
    height: 16px;
    width: 16px;
    border-radius: 50%;
    background: #00ff00;
    cursor: pointer;
    margin-top: -6px;
  }

  &::-webkit-slider-runnable-track {
    width: 100%;
    height: 4px;
    background: #333;
    border-radius: 2px;
  }

  &:focus {
    outline: none;
  }
`;

const MotorTempGauge = ({ value, onChange, machineState }) => {
  const [isStale, setIsStale] = useState(true);

  useEffect(() => {
    setIsStale(machineState !== "running");
    console.log("machine state: " + machineState);
  }, [machineState]);

  return (
    <Container>
      <Title>Motor temperature (Celsius)</Title>
      <GaugeContainer>
        <GaugeScale>
          <GridLines>
            {[...Array(50)].map((_, i) => (
              <div key={i} />
            ))}
          </GridLines>
          {value > 40 && <RedBackground temperature={value} />}
          <ScaleMarkers>
            <div data-value="0" />
            <div data-value="40" />
          </ScaleMarkers>
          <Marker position={value} isStale={isStale}/>
        </GaugeScale>
        <ScaleNumbers>
          {[0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50].map((num) => (
            <span
              key={num}
              style={{
                left: num === 0 ? '0%' : 
                  num === 50 ? '100%' : 
                  `${(num / 50) * 100}%`,
                transform: num === 50 ? 'translateX(-50%)' : 'translateX(-50%)'
              }}
            >
              {num === 50 ? `${num}` : num}
            </span>
          ))}
        </ScaleNumbers>
      </GaugeContainer>
      <StatusMessage>Status: {isStale?  "Stale" : "Active"}</StatusMessage>
      <Slider
        type="range"
        min="0"
        max="50"
        step="0.1"
        value={value}
        onChange={(e) => onChange(Number(e.target.value))}
      />
      
    </Container>
  );
};

export default MotorTempGauge;