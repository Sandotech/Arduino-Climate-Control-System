// Send command to backend
async function send(char) {
    const statusEl = document.getElementById('status');
    try {
        await fetch(`/api/command/${char}`);
        statusEl.innerText = `Último Comando: ${char} (Enviado)`;
    } catch (e) {
        statusEl.innerText = "Error enviando comando";
    }
}

// Poll for sensor updates every 1 second
setInterval(async () => {
    try {
        const res = await fetch('/api/data');
        const data = await res.json();
        
        // Only update if we have valid numbers
        if(data.temp !== "--") {
            document.getElementById('temp').innerText = data.temp;
            document.getElementById('hum').innerText = data.hum;
        }
    } catch (e) {
        console.error("Conexión perdida");
    }
}, 1000);