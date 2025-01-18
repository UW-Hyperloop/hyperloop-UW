import styles from "./status.module.css"

const Status = ({description, color}) => {
    return(
        <div className={styles.container} style={{backgroundColor: color}}>
            <div className={styles.description}>{description}</div>
        </div>
    )

}
export default Status;