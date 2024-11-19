"use client";
import { useEffect, useState } from 'react';
import styles from './CircularMeter.module.css';

const CircularMeter = ({ id, min, max, data, units, partitions, size, colorRanges}) => {
    // Calculate the proportion of the meter based on the current data
    const proportion = ((data - min) / (max - min));
    //console.log(min + " " + max + " " + data)


    // Arc is 240 degrees (2/3 of the full circle)
    // Generate labels
    const totalLabels = partitions + 1; // Including min and max
    const labels = [];

    for (let i = 0; i < totalLabels; i++) {
        const labelProportion = i / (totalLabels - 1); // totalLabels - 1 intervals
        const value = min + labelProportion * (max - min);
        const angle = 150 + 240 * labelProportion; // from -120 to +120 degrees
        labels.push({ value, angle });
    }

    // Calculate the angle for the needle
    const needleAngle = -120 + 240* proportion; // angle in degrees, lines start at noon
    const needleAngleRad = (needleAngle - 90) * (Math.PI / 180); // Adjust for SVG coordinate system

    // Needle coordinates
    const needleLength = 50; // length of the needle from the center
    const needleInnerLength = 20; // start point of the needle from the center
    const needleX1 = 50 + needleInnerLength * Math.cos(needleAngleRad);
    const needleY1 = 50 + needleInnerLength * Math.sin(needleAngleRad);
    const needleX2 = 50 + needleLength * Math.cos(needleAngleRad);
    const needleY2 = 50 + needleLength * Math.sin(needleAngleRad);

    function polarToCartesian(centerX, centerY, radius, angleInDegrees) {
        const angleInRadians = (angleInDegrees - 90) * Math.PI / 180.0;
    
        return {
            x: centerX + radius * Math.cos(angleInRadians),
            y: centerY + radius * Math.sin(angleInRadians),
        };
    }
    
    // Function to describe an arc path
    function describeArc(x, y, radius, startAngle, endAngle) {
        const start = polarToCartesian(x, y, radius, endAngle);
        const end = polarToCartesian(x, y, radius, startAngle);
    
        const largeArcFlag = endAngle - startAngle <= 180 ? '0' : '1';
    
        const d = [
            'M', start.x, start.y,
            'A', radius, radius, 0, largeArcFlag, 0, end.x, end.y,
        ].join(' ');
    
        return d;
    }
    
    // Generate linear gradients and arcs
    const gradientDefs = colorRanges.map((range, index) => {
        let begin = min;
        if(index > 0){
            begin = (range.max - range.min) / 3 + range.min
        }

        const mid = (range.max - range.min) * 2/3 + range.min;

        let stop = max;
        let nextColor = range.color;
        const nextRange = colorRanges[index + 1];
        if(nextRange){
            stop = (nextRange.max - nextRange.min) / 3 + nextRange.min
            nextColor = nextRange.color;
        }

        
        // Calculate start and end points for gradient
        const startProportion = (begin - min) / (max - min);
        const midProportion = (mid - begin) / (stop - begin);
        const endProportion = (stop - min) / (max - min);
        
        console.log(index + ": " + startProportion + " " + midProportion + " " + endProportion);
        const startAngle = -120 + 240 * startProportion;
        const endAngle = -120 + 240 * endProportion;
        console.log(index + ": " + startAngle + " " + endAngle);
        const start = polarToCartesian(50, 50, 45, startAngle); // Start of arc
        const end = polarToCartesian(50, 50, 45, endAngle); // End of arc
        //const info = {index: index, start: begin, mid: mid, end: stop, color: range.color, nextColor: nextColor, midProp: midProportion};
        //console.log(info);
        return (
            <linearGradient id={`grad${id}${index}`} key={index} gradientUnits="userSpaceOnUse" x1={start.x} y1={start.y} x2={end.x} y2={end.y}>
                <stop offset="0%" stopColor={range.color} />
                <stop offset={midProportion} stopColor={range.color}/>
                <stop offset="100%" stopColor={nextColor}/>
            </linearGradient>
        );
    });
    
    // Generate arcs for each color range
    const arcs = colorRanges.map((range, index) => {
        let begin = min;
        if(index > 0){
            begin = (range.max - range.min) / 3 + range.min
        }

        let stop = max;
        const nextRange = colorRanges[index + 1];
        if(nextRange){
            stop = (nextRange.max - nextRange.min) / 3 + nextRange.min
        }

        
        // Calculate start and end points for gradient
        const startProportion = (begin - min) / (max - min);
        const endProportion = (stop - min) / (max - min);
    
        const startAngle = -120 + 240 * startProportion;
        const endAngle = -120 + 240 * endProportion;
    
    
        const pathData = describeArc(50, 50, 45, startAngle, endAngle);
    
        return (
            <path
                key={index}
                d={pathData}
                fill="none"
                stroke={`url(#grad${id}${index})`} // Use the linear gradient along the arc path
                strokeWidth="10"
            />
        );
    });
        
    
    return (
        <div className={styles.meterContainer} style={{height: `${size}px`, width: `${size}px`}}>
            <svg viewBox="0 0 100 100">
                {/* Colored arcs */}
                <defs>
                    {/* Define linear gradients */}
                    {gradientDefs}
                </defs>
                {/* Draw arcs */}
                {arcs}

                
                
                {/* Needle */}
                <line
                    x1={needleX1}
                    y1={needleY1}
                    x2={needleX2}
                    y2={needleY2}
                    stroke="#a10d00"
                    strokeWidth="2"
                />
                <g className={styles.labels}>
                    {labels.map((label, index) => {
                        const angleRad = (label.angle) * Math.PI / 180;
                        const x1 = 50 + 40 * Math.cos(angleRad); // inner radius for ticks
                        const y1 = 50 + 40 * Math.sin(angleRad);
                        const x2 = 50 + 50 * Math.cos(angleRad); // outer radius for ticks
                        const y2 = 50 + 50 * Math.sin(angleRad);
                        const xText = 50 + 35 * Math.cos(angleRad); // text position
                        const yText = 50 + 35 * Math.sin(angleRad);

                        return (
                            <g key={index}>
                                {/* Line (tick mark) */}
                                <line
                                    x1={x1}
                                    y1={y1}
                                    x2={x2}
                                    y2={y2}
                                    stroke="black"
                                    strokeWidth="0.5"
                                />
                                {/* Text label */}
                                <text
                                    x={xText}
                                    y={yText}
                                    textAnchor="middle"
                                    dominantBaseline="middle"
                                    fontSize="5"
                                    //transform={`rotate(${90}, ${xText}, ${yText})`}
                                >
                                    {Math.round(label.value)}
                                </text>
                            </g>
                        );
                    })}
                </g>
            </svg>
            <div className={styles.dataDisplay}>
                <div className={styles.dataValue}>{data}</div>
            </div>
            <div className={styles.units}>{units}</div>
        </div>
    );
};



export default CircularMeter;
