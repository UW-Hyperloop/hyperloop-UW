"use client";
import styles from "./page.module.css";
import CircularMeter from "./components/CircularMeter";
import QuickStatus from "./components/QuickStatus";
import IntegrityMeter from "./components/IntegrityMeter";
import Status from "./components/status";
import { useState, useEffect } from 'react';
export default function Home() {
  const motors = [
    {name: 'Front', status: 'green', rpm: 1000, temp: 20}, 
    {name: 'Back', status: 'yellow', rpm: 800, temp: 40},
    {name: 'Top', status: 'red', rpm: 750, temp: 30},
    {name: 'Core', status: 'green', rpm: 1200, temp: 50},
  ];
  
  const [machineIntegrity, setMachineIntegrity] = useState(79);
  const [cutterHeadStatus, setCutterHeadStatus] = useState("Running")
  const [motorTemp, setMotorTemp] = useState(81.36);
  const [pumpTemp, setPumpTemp] = useState(100);
  const [data, setData] = useState(25);

  const handleSliderChange = (event) => {
    setData(Number(event.target.value));
  };

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

  return (
    <div className={styles.container}>
      {/* Header Section */}
      <header className={styles.header}>
        <button className={styles.stopButton}>STOP</button>
      </header>

      {/* Temperature Section */}
      <section className={styles.motors}>
        <div className={styles.sectionTitle}>Temperatures</div>
        <div className={styles.statusContainer}>
          <div style={{marginRight: '20px'}}>Status:</div> 
          <Status color={'green'} description={cutterHeadStatus}/>
        </div>

        <div className={styles.temp}>
          <div className={styles.tempContainer}>
            <div className={styles.meterTitle}>Motor Temperature</div>
            <CircularMeter 
              id={0} min={40} max={110} data={motorTemp} partitions={10} units={"°F"} size={150}
              colorRanges={[
                { min: 40, max: 65, color: 'red' },      // Critical low
                { min: 65, max: 75, color: 'yellow' },  // Warning low
                { min: 75, max: 85, color: 'green' },   // Safe range
                { min: 85, max: 90, color: 'yellow' },  // Warning high
                { min: 90, max: 110, color: 'red' },     // Critical high
              ]}
            />
          </div>
          <div className={styles.tempContainer}>
            <div className={styles.meterTitle}>Pump Temperature</div>
            <CircularMeter 
              id={1} min={40} max={200} data={pumpTemp} partitions={10} units={"°F"} size={150}
              colorRanges={[
                { min: 40, max: 65, color: 'red' },      // Critical low
                { min: 65, max: 85, color: 'yellow' },  // Warning low
                { min: 85, max: 105, color: 'green' },   // Safe range
                { min: 105, max: 130, color: 'yellow' },  // Warning high
                { min: 130, max: 200, color: 'red' },     // Critical high
              ]}
            />          

          </div>
        </div>
        
        {/*
        {motors.map((motor, index) => (
          <QuickStatus 
            name={motor.name} 
            key={index} 
            temp={motor.temp} 
            rpm={motor.rpm}
            status={motor.status}
            />
        ))}
        */}
      </section>

      {/* Pump Flow Section */}
      <section className={styles.pump}>
        <div className={styles.sectionTitle}>Pump Flow</div>
        <div className={styles.statusContainer}>
          <div style={{marginRight: '20px'}}>Status:</div> 
          <Status color={'green'} description={cutterHeadStatus}/>
        </div>
        <div className={styles.temp}>
          <div className={styles.tempContainer}>
              <div className={styles.meterTitle}>Water in Rate </div>
              <CircularMeter 
                id={2} min={10} max={40} data={data} partitions={10} units={"Gal/min"} size={150}
                colorRanges={[
                  { min: 10, max: 15, color: 'red' },      // Critical low
                  { min: 15, max: 22, color: 'yellow' },  // Warning low
                  { min: 22, max: 27, color: 'green' },   // Safe range
                  { min: 27, max: 35, color: 'yellow' },  // Warning high
                  { min: 35, max: 40, color: 'red' },    // Critical high
                ]}
              />
          </div>
          <div className={styles.tempContainer}>
              <div className={styles.meterTitle}>Muck out Rate </div>
              <CircularMeter 
                id={3} min={30} max={60} data={43.2} partitions={10} units={"Gal/min"} size={150}
                colorRanges={[
                  { min: 30, max: 35, color: 'red' },      // Critical low
                  { min: 35, max: 41, color: 'yellow' },  // Warning low
                  { min: 41, max: 48, color: 'green' },   // Safe range
                  { min: 48, max: 55, color: 'yellow' },  // Warning high
                  { min: 55, max: 60, color: 'red' },    // Critical high
                ]}
              />
          </div>
        </div>
        
        <div className={styles.sliderContainer}>
          <input
            type="range"
            min={0}
            max={50}
            value={data}
            onChange={handleSliderChange}
            className={styles.slider}
          />
          <div className={styles.sliderValue}>{data}</div>
        </div>
      </section>


      {/* Status Section */}
      <section >
        <div style={{height: '40px'}}></div>
        <div className={styles.statusContainer}>
          <div style={{marginRight: '20px'}}>Bentonite Pump:</div> 
          <Status color={'green'} description={cutterHeadStatus}/>
        </div>
        <div style={{height: '20px'}}></div>
        <div className={styles.statusContainer}>
          <div style={{marginRight: '20px'}}>Communication:</div> 
          <Status color={'green'} description={cutterHeadStatus}/>
        </div>
        <div style={{height: '20px'}}></div>
        <div className={styles.statusContainer}>
          <div style={{marginRight: '20px'}}>Power:</div> 
          <Status color={'green'} description={cutterHeadStatus}/>
        </div>
        <div style={{height: '20px'}}></div>
        <div className={`${styles.statusItem} ${styles.alert}`}>
          <p>Explosive/Flammable Gases</p>
          <p>N/A</p>
        </div>
      </section>
    </div>
  );
}
