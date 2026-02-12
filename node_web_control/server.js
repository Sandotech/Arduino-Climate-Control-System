const express = require('express');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const path = require('path');

const app = express();
const PORT = 3000;

// ===== SERIAL CONFIGURATION =====
// Matches your C++ constants
const SERIAL_PORT = '/dev/ttyACM0'; 
const BAUD_RATE = 9600;

let port;
let parser;
let lastData = { temp: "--", hum: "--" };

try {
    port = new SerialPort({ path: SERIAL_PORT, baudRate: BAUD_RATE });
    
    // The parser handles reading lines ending in \n automatically
    parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

    port.on('open', () => {
        console.log(`Connected to Arduino on ${SERIAL_PORT}`);
        // Give Arduino time to reset
        setTimeout(() => console.log('Ready for commands'), 2000);
    });

    // Listen for data from Arduino constantly
    parser.on('data', (line) => {
        line = line.trim();
        const parts = line.split(',');
        if (parts.length === 2) {
            lastData = { temp: parts[0], hum: parts[1] };
            console.log(`Received: Temp ${parts[0]}C, Hum ${parts[1]}%`);
        }
    });

    port.on('error', (err) => {
        console.error('Serial Error:', err.message);
    });

} catch (error) {
    console.error("Failed to open serial port. Is the Arduino connected?");
}

// ===== WEB SERVER ROUTES =====

// Serve static files (HTML/CSS) from 'public' folder
app.use(express.static('public'));

// API to get latest sensor data
app.get('/api/data', (req, res) => {
    res.json(lastData);
});

// API to send commands
app.get('/api/command/:char', (req, res) => {
    const { char } = req.params;
    
    if (port && port.isOpen) {
        port.write(char, (err) => {
            if (err) {
                return res.status(500).json({ error: err.message });
            }
            console.log(`Sent command: ${char}`);
            res.json({ status: 'sent', command: char });
        });
    } else {
        res.status(503).json({ error: 'Serial port not open' });
    }
});

app.listen(PORT, () => {
    console.log(`Web interface running at http://localhost:${PORT}`);
});