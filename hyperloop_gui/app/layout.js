import './globals.css'; // Global styles
import Navbar from './components/Navbar'; // Import the Navbar component

export const metadata = {
  title: 'Drill Dashboard',
  description: 'Drill Sensor and Motor Data Dashboard',
};

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body style={{backgroundColor: 'white', height: '100%'}}>
        {/* <Navbar />  */}
        {children}
      </body>
    </html>
  );
}
