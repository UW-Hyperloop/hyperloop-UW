"use client"; // Marks this component as client-side for Next.js

import styles from './IntegrityMeter.module.css';

//lowest percent for green, lowest percent for yellow, the rest is red

const IntegrityMeter = ({ integrity, green, yellow }) => {
    // Determine the color of the progress bar based on the integrity value
    let barColor = ''; 
    if(integrity >= green){
        barColor = 'green';
    }
    else if (integrity >= yellow) {
        barColor = 'yellow';
    } 
    else {
        barColor = 'red'; 
    }

    return (
        <div className={styles.meterContainer}>
            <div className={styles.meterBackground}>
                <div
                    className={styles.meterForeground}
                    style={{
                        width: `${integrity}%`,
                        backgroundColor: barColor,
                    }}
                >
                
                </div>
                <div className={styles.meterLabel}>{integrity}%</div>
            </div>
        </div>
    );
};

export default IntegrityMeter;
