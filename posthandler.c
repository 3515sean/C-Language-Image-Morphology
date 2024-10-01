#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "posthandler.h"

//Some binary file include '\0' ,that where make strstr didn't work as expect.
//Add the string length to argument to avoid using '\0' as ending rule.
char* find_file_end(string, substring, length)
    register char *string;	/* String to search. */
    char *substring;		/* Substring to try to find in string. */
	int length;
{
    register char *a, *b;
    char *end;
    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
	end = string + length;
    if (*b == 0) {
		return string;
    }
    for ( ; string != end; string += 1) {
		if (*string != *b) {
			continue;
		}
		a = string;
		while (1) {
			if (*b == 0) {
				return string;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = substring;
    }
    return NULL;
}
void handle_post(SOCKET client_socket, char* request) {
	int content_length = atoi(strstr(request, "Content-Length: ") + strlen("Content-Length: "));
    // Find the boundary string in the request
    char* boundary = strstr(request, "boundary=");
    if (boundary == NULL) {
        return;
    }
    
    // Extract the boundary string and create the boundary marker
    char* boundary_str = strtok(boundary, "=");
    char* boundary_marker = strtok(NULL, "\r\n");
    char full_boundary_marker[256];
    sprintf(full_boundary_marker, "--%s", boundary_marker);
	
	//Get the body sector of the post request
	//char request[MAX_REQUEST_SIZE+1];
	int request_length = recv(client_socket, request, MAX_REQUEST_SIZE, 0);
	request[request_length] = '\0';
    // Extract the filename from the request headers
    char* filename_start = strstr(request, "filename=\"") + strlen("filename=\"");
    if (filename_start == NULL) {
        return;
    }
    char* filename_end = strstr(filename_start, "\"");
    if (filename_end == NULL) {
        return;
    }
	
    int filename_length = filename_end - filename_start;
    char filename[MAX_FILENAME_LENGTH];
    strncpy(filename, filename_start, filename_length);
    filename[filename_length] = '\0';
	char filepath[MAX_FILENAME_LENGTH];
	sprintf(filepath,"upload\\%s",filename);
    printf("%s\n",filepath);
    // Save the file data to disk
    FILE* file = fopen(filepath, "wb");
    if (file == NULL) {
        return;
    }
		
	// Find the start of the file data in the request
    char* file_start = strstr(request, "\r\n\r\n") + 4;
    if (file_start == NULL + 4) {
        return;
    }
    char* file_end;
	for(int c = 0;c < (content_length/MAX_REQUEST_SIZE) + 1; c++){
		// Find the end of the file data
		file_end = find_file_end(file_start, full_boundary_marker, request_length) - 2;		
		if (file_end == NULL - 2) {
			file_end = request + request_length;
			fwrite(file_start, file_end - file_start, 1, file);
			request_length = recv(client_socket, request, MAX_REQUEST_SIZE, 0);
			request[request_length] = '\0';
			file_start = request;
			
		}else{
			fwrite(file_start, file_end - file_start, 1, file);
		}	
	}    
    fclose(file);

    // Send a response back to the client
    char response[256];
	sprintf(response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n%s",filepath);
    send(client_socket, response, strlen(response), 0);
}