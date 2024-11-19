"use client"
import styles from "./page.module.css";
import CircularMeter from "../components/CircularMeter";
import { useState } from "react";
const Motors = () => {
  const [data, setData] = useState(0);


  const handleSliderChange = (event) => {
    setData(Number(event.target.value));
  }; 
  return (
    <div style={{marginLeft: '20%'}}>
        <h1 className={styles.title}>Motors</h1>
        <CircularMeter
          id={0} min={40} max={100} data={data} partitions={10} units={"°C"} size={150}
          colorRanges={[
            {min: 40, max: 70, color: 'red' },      
            {min: 70, max: 100, color: 'yellow'},
          ]}
        />

        <div className={styles.sliderContainer}>
          <input
            type="range"
            min={40}
            max={200}
            value={data}
            onChange={handleSliderChange}
            className={styles.slider}
          />
          <div className={styles.sliderValue}>{data}</div>
        </div>
        

    
      
    </div>
  );
}

export default Motors;