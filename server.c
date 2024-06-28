#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//PORT
#define PORT 8003
#define BUFFER_SIZE 1024

int main () {
  char buffer[BUFFER_SIZE];
  char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world</html>\r\n";

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("webserver (socket)");
    return 1;
  }
  printf("Socket created...\n");
  
  struct sockaddr_in host_addr;
  int host_addrlen = sizeof(host_addr);

  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(PORT);
  host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  struct sockaddr_in client_addr;
  int client_addrlen = sizeof(client_addr);
  
  if (bind(server_socket, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
    perror("webserver (bind)");
    return 1;
  };
  printf("socket successfully bound to address\n");

  if (listen(server_socket, SOMAXCONN) != 0) {
    perror("webserver (listen)");
    return 1;
  }
  printf("server listening for connections\n");

  while (1) {
    int new_server_socket = accept(server_socket, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

    if (new_server_socket < 0) {
      perror("webserver (accept)");
      continue;
    }
    printf("connection accepted\n");

    int sock_name = getsockname(new_server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
    if (sock_name < 0) {
      perror("webserver (getsocketname)");
      continue;
    };

    int read_val = read(new_server_socket, buffer, BUFFER_SIZE);
    if (read_val < 0) {
      perror("webserver (read)");
      continue;
    };

    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    int write_val = write(new_server_socket, resp, strlen(resp));
    if (write_val < 0) {
      perror("webserver (write)");
      continue;
    };

    close(new_server_socket);
  }

  return 0;
}