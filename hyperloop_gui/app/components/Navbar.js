import Link from 'next/link';
import styles from './Navbar.module.css';

export default function Navbar() {
return (
    <nav className={styles.navbar}>
        <ul className={styles.navItems}>
            <li className={styles.navItem}>
                <Link href="/">Home</Link>
            </li>
            <li className={styles.navItem}>
                <Link href="/motors">Motors</Link>
            </li>
            <li className={styles.navItem}>
                <Link href="/sensors">Sensors</Link>
            </li>
            <li className={styles.navItem}>
                <Link href="/status">Status</Link>
            </li>
        </ul>
    </nav>
);
}
