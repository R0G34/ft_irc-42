
// ======================================================================
// Archivo: utils.cpp
// Propósito: Funciones auxiliares y utilidades generales (sockets no bloqueantes, fechas, etc.).
// ======================================================================

#include <irc.hpp>
#include <fcntl.h> 
#include <ctime>
#include <string>

// Actualiza el atributo correspondiente.
bool	setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		return false;
	return true;
}

// Comprueba o valida una condición concreta.
bool isSpecial(char c)
{
    return (c == '[' || c == ']' || c == '\\' || c == '`' ||
            c == '_' || c == '^' || c == '{' || c == '|' );
}


// Método de la clase std que realiza la operación principal asociada.
std::string currentDateTimeString() {
    std::time_t t = time(NULL);
    std::tm* tm_info = localtime(&t);

    char buf[64];
    strftime(buf, sizeof(buf), "%b %d %Y at %H:%M:%S", tm_info);

    return std::string(buf);
}
