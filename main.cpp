#include <iostream>

#include "network/SocketHolder.h"


int main(){
    struct sockaddr_in server_address;
    
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_address.sin_port = htons(PORT);

    ft::SocketHolder socket(AF_INET, SOCK_STREAM, 0);


    socket.bind(&server_address);
    socket.listen();


    for(int counter = 0; true ; )
    {

        ft::SocketHolder accepted_socket(socket.accept());

        /* READ REQ */
        std::string request_str = accepted_socket.read();
        std::cout << "REQ:" << std::endl << request_str << std::endl;

        /* PREPARE RESP */
        std::stringstream ss;
        /* if "favicon ico" request - just ignore for now*/
        if (request_str.find("favicon") != std::string::npos)
        {
            ss << "HTTP/1.1 200 OK\r\n\r\n<html>\n\r<body>\n\r\r NO FAVICON \n\r</body>\n</html>";
        } 
        else
        {
            ss << "HTTP/1.1 200 OK\r\n\r\n<html>\n\r<body>\n\r\rhello " << counter << " times\n\r</body>\n</html>";
            counter++;
        }
        accepted_socket.send(ss.str());
    }

}