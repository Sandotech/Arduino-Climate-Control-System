#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#include <unistd.h>  // UNIX standard function definitions

using namespace std;

// ===== CONSTANTES =====
const char *PORT_NAME = "/dev/ttyACM0";
const int BAUD_RATE = B9600;

// ===== CONFIGURAR SERIAL =====
int configurarSerial(const char *portname)
{
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        cerr << "[ERROR] No se pudo abrir " << portname << ": " << strerror(errno) << endl;
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        cerr << "[ERROR] tcgetattr falló" << endl;
        return -1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;      // 8-bit chars
    tty.c_cflag &= ~PARENB;                          // No parity
    tty.c_cflag &= ~CSTOPB;                          // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                         // No flow control
    tty.c_lflag = 0;                                 // Non-canonical mode
    tty.c_oflag = 0;                                 // Raw output
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK); // Disable specials

    // Timeout settings
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; // 1.0 seconds timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        cerr << "[ERROR] tcsetattr falló" << endl;
        return -1;
    }
    return fd;
}

// ===== LEER ARDUINO CON TIMEOUT =====
string leerArduino(int fd)
{
    string linea = "";
    char c;
    int timeouts = 0;

    // Limpiar buffer de entrada para tener datos frescos
    tcflush(fd, TCIFLUSH);

    // Intentamos leer hasta encontrar '\n' o exceder intentos
    while (true)
    {
        int n = read(fd, &c, 1);
        if (n > 0)
        {
            timeouts = 0; // Resetear contador si recibimos datos
            if (c == '\n')
                break;
            if (c != '\r')
                linea += c;
        }
        else if (n == 0)
        {
            // Timeout ocurrido (VTIME paso sin datos)
            timeouts++;
            if (timeouts >= 3)
            { // Si pasan ~3 segundos sin línea completa
                return "";
            }
        }
        else
        {
            return ""; // Error de lectura
        }
    }
    return linea;
}

void mostrarInterfaz(string temp, string hum)
{
    cout << "\n===============================" << endl;
    cout << "   ESTADO ACTUAL DEL SISTEMA   " << endl;
    cout << "===============================" << endl;
    cout << " Temperatura : " << temp << " °C" << endl;
    cout << " Humedad     : " << hum << " %" << endl;
    cout << "===============================" << endl;
}

int main()
{
    int serialInfo = configurarSerial(PORT_NAME);
    if (serialInfo < 0)
        return 1;

    cout << "Conectando al Arduino en " << PORT_NAME << "..." << endl;
    sleep(2); // Esperar reinicio del Arduino
    cout << "Conexión establecida.\n"
         << endl;

    int opcion = -1;

    while (opcion != 0)
    {
        cout << "\n--- PANEL DE CONTROL ---" << endl;
        cout << "1. Alternar Monitoreo (ON/OFF)" << endl;
        cout << "2. Alternar Ventilador (ON/OFF)" << endl;
        cout << "3. Alternar Alarma (ON/OFF)" << endl;
        cout << "4. RECIBIR DATOS (Temp/Hum)" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione: ";

        if (!(cin >> opcion))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            opcion = -1;
            continue;
        }

        char comando = 0;
        switch (opcion)
        {
        case 1:
            comando = 'M';
            break;
        case 2:
            comando = 'V';
            break;
        case 3:
            comando = 'A';
            break;
        case 4:
        {
            cout << "Leyendo sensores..." << endl;
            string datos = leerArduino(serialInfo);

            if (!datos.empty())
            {
                stringstream ss(datos);
                string temp, hum;
                getline(ss, temp, ',');
                getline(ss, hum, ',');

                if (!temp.empty() && !hum.empty())
                {
                    mostrarInterfaz(temp, hum);
                }
                else
                {
                    cout << "[AVISO] Datos incompletos recibidos: " << datos << endl;
                }
            }
            else
            {
                cout << "[TIMEOUT] No se recibieron datos. ¿Está activado el monitoreo?" << endl;
            }
            break;
        }
        case 0:
            cout << "Cerrando conexión..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
        }

        if (comando != 0)
        {
            write(serialInfo, &comando, 1);
            cout << ">> Comando '" << comando << "' enviado." << endl;
        }
    }

    close(serialInfo);
    return 0;
}
