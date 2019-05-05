#include <sys/types.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MSGBUFFERSIZE 1024	//String Nachrichtenbuffergröße für send/receive
#define DNS "rnit.space"	//FQDN für Server
#define IDLEMSG "..."		//Idle Nachrichten Trigger
#define PORT 8002			//Port für Server

/**
 * Makes a DNS lookup for a given hostname.
 * 
 * @param host Hostname to look up
 * @param ipbuffer Buffer to write IPv4 with format *.*.*.*
 * @return
 */
void dnsLookupIPv4(const char *host, char* ipBuffer, int ipBufferSize)
{
	struct addrinfo hints, *result;
	
	memset(ipBuffer, 0, sizeof(char) * ipBufferSize);	//Initialisierung von ipBuffer
	memset(&hints, 0, sizeof(hints));		//Initialisierung von hints
	hints.ai_family = AF_INET;				//IPv4
	hints.ai_socktype = 0;					//Alle Socket Typen

	if(getaddrinfo(host, NULL, &hints, &result) != 0)	//DNS lookup
	{
		perror("getaddrinfo");
		exit(-1);
	}

	struct in_addr inAddr = (struct in_addr)((struct sockaddr_in *)result->ai_addr)->sin_addr;
	//In Format für inet_ntoa bringen

	strcpy(ipBuffer, inet_ntoa(inAddr));				//IP Adresse auslesen

	printf ("~~~ Server ~~~\nHost DNS: %s\n", host);	//Informationen ausgeben
	printf ("IPv4 address: %s\n\n", ipBuffer);
}

/**
 * Sends network packet with given socket.
 * 
 * @param clientSocket Socket to use
 * @param message Message to send
 * @return
 */
void sendPacket(int clientSocket, char* message)
{
	if(send(clientSocket, message, strlen(message), 0) < 0)		//Nachricht senden
	{ 
		perror("send");
		exit(-1);
	}
	printf("\n ~~ PACKET SEND BEGIN ~~~\n%s ~~ PACKET SEND END ~~~\n", message);
}

/**
 * Receives network packet with given socket.
 * 
 * @param clientSocket Socket to use
 * @param buffer Buffer to write message
 * @param buffersize Size of buffer
 * @return
 */
int recvPacket(int clientSocket, char* buffer, int buffersize)
{
	int msgByteCount = 0;
	memset(buffer, 0, sizeof(char) * buffersize);				//Buffer initialisieren
	msgByteCount = recv(clientSocket, buffer, buffersize, 0);	//Nachricht empfangen
	
	if(msgByteCount < 0)		//Error beim Nachrichtenempfang z.B. Timeout
	{
		perror("\nError: recv");
		return -1;
	}
	else if(msgByteCount == 0)	//Verbindung wurde von Server geschlossen
	{
		close(clientSocket);	//Socket schließen
		printf("\nInfo: Connection closed by peer.\n");
		exit(0);				//kein Return!!! in diesem Fall einfacher - anpassen bei Wiederbenützung
	}
	printf("\n ~~ PACKET RECEIVE BEGIN ~~~\n%s\n ~~ PACKET RECEIVE END ~~~\n", buffer);
	return 1;
}

/**
 * Calculates binary (integer) mathematical expression given with operators +, -, *, /
 * i.e. "4 + 89", "47462 * 9042", etc.
 * 
 * @param text Textual mathematical expression
 * @return
 */
int calcBinMathText(char* text)
{
	char operator[1];	//Operator für Ausdruck
	char *saveptr;		//Pointer für strtok_r
	int left, right;	//Linker und Rechter Teil des binären Ausdrucks
	
	if(strstr(text, "+") != NULL)	//Operator herausfiltern und setzen
	{
		operator[0] = '+';
	}
	else if(strstr(text, "-") != NULL)
	{
		operator[0] = '-';
	}
	else if(strstr(text, "*") != NULL)
	{
		operator[0] = '*';
	}
	else if(strstr(text, "/") != NULL)
	{
		operator[0] = '/';
	}
	
	char* split = strtok_r(text, (const char*)operator, &saveptr);	//Linken Teil filtern
	left = atoi((const char*)split);
	
    split = strtok_r(NULL, (const char*)operator, &saveptr);	//Rechten Teil filtern
    right = atoi((const char*)split);
    
    if(operator[0] == '+')		//Operation ausführen
	{
		return left + right;
	}
	else if(operator[0] == '-')
	{
		return left - right;
	}
	else if(operator[0] == '*')
	{
		return left * right;
	}
	else if(operator[0] == '/')
	{
		return left / right;
	}
	printf("\ncalcBinMathText: Calculation failed. No valid operator found.\n");
	return 0;
}

/**
 * Checks if string starts with certain character.
 * 
 * @param pre Character to check for.
 * @param str String to check with pre.
 * @return	True or False
 */
_Bool startsWith(const char pre, const char *str)
{
    return str[0] == pre;
}

/**
 * Trims certain leading character of string.
 * Will trim until character can't be found at string[0].
 * CREDITS GO TO
 * https://www.daniweb.com/programming/software-development/code/216919/implementing-string-trimming-ltrim-and-rtrim-in-c
 * 
 * @param string String to trim.
 * @param trimChar Character to trim.
 * @return Trimmed String.
 */
void ltrim(char *string, char junk)
{
    char* original = string;
    char *p = original;
    int trimmed = 0;
    do
    {
        if (*original != junk || trimmed)
        {
            trimmed = 1;
            *p++ = *original;
        }
    }
    while (*original++ != '\0');
}

/**
 * Solves riddles given within a string.
 * Relies on function calcBinMathText for solving riddles.
 * 
 * @param buffer String containing riddles.
 * @param msgBufferSize Size of string buffer.
 * @param results String to save results to.
 * @param riddleToken Tokens to trigger riddle solving.
 * @param riddleTokenCount Count of tokens to trigger riddle solving.
 * @return
 */
void solveRiddle(char* buffer, int msgBufferSize, char* results,
						char* riddleTokens, int riddleTokenCount)
{
	char *saveptr;			//saveptr für strtok_r
    char* token = strtok_r(buffer, riddleTokens, &saveptr);	//Ersten Teil des Strings filtern
    int i = 0;				//Zählervariable
    _Bool tokenCheck = 0;	//Wahrheitswert für Token Check
        
	memset(results, 0, sizeof(char) * msgBufferSize);	//Initialisiere results
	ltrim(buffer, ' ');		//Trimme führende Spaces
	
	while(!tokenCheck && i < riddleTokenCount)			//Suche ob Token in buffer
	{
		tokenCheck |= startsWith((const char)riddleTokens[i], buffer);
		i++;
	}
	
	if(!tokenCheck)	//Falls buffer nicht mit Token beginnt -> springe zum ersten Token
    {
		token = strtok_r(NULL, riddleTokens, &saveptr);
	}
	
	char result[msgBufferSize];	//String Zwischenspeicher für einzelne Ergebnisse
	
    while (token != NULL)	//Solange ein Token gefunden wird
    {
		memset(result, 0, sizeof(char) * msgBufferSize);
				
        sprintf(result, "%i", calcBinMathText(token));	//Löse Rätsel
        strcat(results, result);	//Schreibe Ergebnis
        strcat(results, "\n");
        
        token = strtok_r(NULL, riddleTokens, &saveptr);	//Springe zu nächstem Token
    }
}

int main(int argc, char const *argv[]) 
{ 
	char* calcTokens = ">›";	//Token um Berechnung durchzuführen
	int calcTokenCount = 2;		//Anzahl der Tokens in calcTokens
	char srvIP[16] = {0};	//IP string *.*.*.*
	int clientSocket = 0;	//Socket für Servervebindung
	struct sockaddr_in srvAdress; 			//Serverinfo
	char packetRecvBuffer[MSGBUFFERSIZE] = {0};	//Buffer für einzelne erhaltene Pakete
	char msgRecvBuffer[MSGBUFFERSIZE] = {0};	//Buffer für zusammengesetzte erhaltene Pakete
	char packetSendBuffer[MSGBUFFERSIZE] = {0};	//Buffer zum Senden von Paketen
	struct timeval tv;		//struct für recv Timeout
	
	tv.tv_sec = 5;			//Standard Timeout 5.5s
	tv.tv_usec = 500000;
	
	dnsLookupIPv4(DNS, srvIP, 16);	//DNS Lookup für Server

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 	//Erstelle Socket
	{ 
		perror("socket");
		return -1; 
	} 

	memset(&srvAdress, '0', sizeof(srvAdress)); 				//Initialisiere Server Info
	srvAdress.sin_family = AF_INET; 							//IPv4
	srvAdress.sin_port = htons(PORT); 							//Port setzen
	if(inet_pton(AF_INET, srvIP, &srvAdress.sin_addr) <= 0)  	//IP Adresse binär kodieren
	{ 
		perror("inte_pton"); 
		return -1; 
	} 

	if(connect(clientSocket, (struct sockaddr *)&srvAdress, sizeof(srvAdress)) < 0) 	//Verbinde zu Server
	{ 
		perror("connect");
		return -1; 
	} 

	while(1)
	{
		memset(msgRecvBuffer, 0, sizeof(char) * MSGBUFFERSIZE);			//Initialisere alle Buffer
		memset(packetRecvBuffer, 0, sizeof(char) * MSGBUFFERSIZE);
		strcpy(packetRecvBuffer, IDLEMSG);
		
		tv.tv_sec = 5;		//Setze zurück auf Standard Timeout
		setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
		
		while(strstr(packetRecvBuffer, IDLEMSG) != NULL)	//Warte solange Idle Nachrichten erhalten werden
		{
			recvPacket(clientSocket, packetRecvBuffer, MSGBUFFERSIZE);
		}
		
		strcpy(msgRecvBuffer, packetRecvBuffer);			//Erstes Paket in msgRecvBuffer kopieren
		
		tv.tv_sec = 0;		//Setze Timeout für Paketempfang 0.5s
		setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
		
		//Solange weitere Pakete erhalten werden
		while(recvPacket(clientSocket, packetRecvBuffer, MSGBUFFERSIZE) > 0)
		{
			strcat(msgRecvBuffer, packetRecvBuffer);		//hänge weitere Pakete an msgRecvBuffer an
		}
		
		solveRiddle(msgRecvBuffer, MSGBUFFERSIZE, packetSendBuffer, calcTokens, calcTokenCount);
		//Löse Rätsel
		
		if(packetSendBuffer[0] != '\0')	sendPacket(clientSocket, packetSendBuffer);
		//Sende Antwort falls diese nicht leer ist
	}
		
	close(clientSocket);	//Socket schließen

	return 0; 
} 
