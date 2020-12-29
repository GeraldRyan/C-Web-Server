
/**
 * webserver.c -- A webserver written in C
 * 
 * Test with curl (if you don't have it, install it):
 * 
 *    curl -D - http://localhost:3490/
 *    curl -D - http://localhost:3490/d20
 *    curl -D - http://localhost:3490/date
 * 
 * You can also test the above URLs in your browser! They should work!
 * 
 * Posting Data:
 * 
 *    curl -D - -X POST -H 'Content-Type: text/plain' -d 'Hello, sample data!' http://localhost:3490/save
 * 
 * (Posting data is harder to test from a browser.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"
#include "file.h"
#include "mime.h"
#include "cache.h"

#define PORT "3490" // the port users will be connecting to

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"

#define true (1 == 1)
#define false (!true)
#define WRAP_BODY false // need to debug otherwise it can crash or glitch. works sometimes at certain endpoints /d20 for certain tags
#define EXTRA true

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */

int send_response(int fd, char *header, char *content_type, void *body, int content_length)
{
    const int max_response_size = 262144;
    char response[max_response_size];
    char header_only[max_response_size];
    char page_header[] = "<div style=\"color:black\">Endpoings are <ul>/d20</ul><ul></ul><ul>/d20</ul>, </div>";
    char opent[] = "<h1>";
    char closet[] = "</h1>";

    // Build HTTP response and store it in response
    time_t now;
    time(&now);
    // header = HTTP/1.1 200 OK
    // body = 10;

    if (strcmp(content_type, "image/jpg")) // is not jpeg (strcmp returns 0 if match)
    {
        if (WRAP_BODY)
        {
            printf("body %s and old length %zu\n", (char *)body, strlen(body));
            void *body_wrapper = malloc(sizeof(char) * 100);
            sprintf(body_wrapper, "%s%s%s", opent, (char *)body, closet);
            sprintf(body, "%s", (char *)body_wrapper);
            printf("body %s and body wrapper %s and new length %zu\n", (char *)body, (char *)body_wrapper, strlen(body));
            content_length = strlen(body);
        }
        sprintf(response, "%s\n Date: %s Connection: close\n Content-length: %i\n Content-type: %s\n\n %s\n", header, ctime(&now), content_length, content_type, (char *)body);
        int response_length = strlen(response);
        int rv = send(fd, response, response_length, 0); // send it off
        if (rv < 0)
        {
            perror("send");
        }
        return rv;
    }
    else // is jpeg. Two sends
    {
        sprintf(header_only, "%s\n Date: %s Connection: close\n Content-length: %i\n Content-type: %s\n\n", header, ctime(&now), content_length, content_type);
        int header_length = strlen(header_only);
        int rv = send(fd, header_only, header_length, 0);
        int body_length = strlen(body);
        int rvbody = send(fd, body, content_length, 0);
        if (rv < 0)
        {
            perror("send");
        }

        return rv;
    }
}

void get_d20(int fd)
{
    time_t t;
    srand(time(&t));
    char res_num[10];
    sprintf(res_num, "%d", rand() % 21);
    printf("res_num, %s", res_num);
    if (EXTRA)
    {
        char body_to_send[34];
        sprintf(body_to_send, "Your random number of the day is %s", res_num);
        send_response(fd, "HTTP/1.1 200 OK", "text/html", body_to_send, 34);
    }
    else
    {
        send_response(fd, "HTTP/1.1 200 OK", "text/html", res_num, 1);
    }
}

void resp_404(int fd)
{
    char filepath[4096];
    struct file_data *filedata;
    char *mime_type;

    // Fetch the 404.html file
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    filedata = file_load(filepath);

    if (filedata == NULL)
    {
        // TODO: make this non-fatal
        fprintf(stderr, "cannot find system 404 file\n");
        exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, struct cache *cache, char *request_path)
{
    char filepath[50];
    struct file_data *filedata;
    char *mime_type;

    // fetch the file if any
    snprintf(filepath, sizeof filepath, "%s%s", SERVER_ROOT, request_path);

    filedata = file_load(filepath);

    if (filedata == NULL)
    {
        resp_404(fd);
        // return;
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);
    file_free(filedata);
}

/**
 * Search for the end of the HTTP header
 * 
 * "Newlines" in HTTP can be \r\n (carriage return followed by newline) or \n
 * (newline) or \r (carriage return).
 */
char *find_start_of_body(char *header)
{
    ///////////////////
    // IMPLEMENT ME! // (Stretch)
    ///////////////////
    return header;
}

/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, struct cache *cache)
{
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0)
    {
        perror("recv");
        return;
    }

    char method[100], path[99], protocol[153];
    printf("REQUEST LINE 198 %s\n", request);
    sscanf(request, "%s %s %s", method, path, protocol);
    printf("REQUEST LINE 200 %s\n", request);
    printf("\nMETHOD!! %s\n", method); // GET

    if (!strcmp("GET", method))
    {
        if (!strcmp("/d20", path))
        {
            get_d20(fd); // method must be changing here
        }

        else
        {
            if (!strcmp(path, "/"))
            {
                sprintf(path, "/index.html");
            }
            get_file(fd, cache, path); // base case
        }
    }
    else if (!strcmp("POST", method))
    {
        // base case
        resp_404(fd); // base case
    }
    else if (!strcmp("POST", method))
    {
        // TODO STRETCH GOAL

        // Read body (so complete function above)
        // parse data for releveant stuff
        // do what you want with it
    }
    else
    {
        resp_404(fd); // base case
    }
    printf("\nMETHOD!! %s\n", method); // <h3>18</h3>
    printf("METHOD (239): %s PATH %s PROTOCOL %s\n", method, path, protocol);

    // (Stretch) If POST, handle the post request
}

/**
 * Main
 */
int main(void)
{
    int newfd;                          // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    struct cache *cache = cache_create(10, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0)
    {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);

    // This is the main loop that accepts incoming connections and
    // responds to the request. The main parent process
    // then goes back to waiting for new connections.

    while (1)
    {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1)
        {
            perror("accept");
            continue;
        }

        // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.

        handle_http_request(newfd, cache);
        close(newfd);
    }

    // Unreachable code

    return 0;
}
