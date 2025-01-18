import styles from "./QuickStatus.module.css";

const QuickStatus =({name, temp, rpm, status}) => {
    //console.log(status);
    return (
        <div className={styles.motorItem}>
            <div className={styles.label}>{name}</div>
            <div className={styles.data}>
                <div>
                <p>Temperature: {temp}</p>
                <p>RPM: {rpm}</p>
                </div>
                <div className={styles.bar} style={{backgroundColor: status}}></div>
            </div>
        </div>
    );
}
export default QuickStatus;