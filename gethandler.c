#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "gethandler.h"
#include "image_process.h"

char* url_decode(char* encoded_url){
	char* decoded_url;
    int i, j;
	
	decoded_url = malloc(strlen(encoded_url)+1);
    for (i = 0, j = 0; i < strlen(encoded_url); i++, j++) {
        if (encoded_url[i] == '%') {
            int hex_num;
            sscanf(&encoded_url[i+1], "%2x", &hex_num); // convert the two characters after % to hex number
            decoded_url[j] = (char) hex_num;
            i += 2; // skip the two characters after %
        } else {
            decoded_url[j] = encoded_url[i];
        }
    }
    decoded_url[j] = '\0'; // terminate the string
    sprintf(encoded_url,"%s",decoded_url);
    return encoded_url;
}



void handle_get(SOCKET client_socket, char* request) {
    char filename[MAX_FILENAME_LENGTH]; 
	char content_type[39];
	int args_len, filename_len;
	char request_args[256];
	args_len = strstr(request + 5," ") - (request + 5);
	if(args_len == 0){
		strcpy(filename,"views/ImageProcess.html");
		strcpy(request_args,"");
	}else{
		strncpy(filename ,request + 5, args_len);
		filename[args_len] = '\0';
	}
	char* p = request + 5;
	int i;
	for(i = 0;i < args_len; i++,p++){
		if(*p == '?'){
			strncpy(filename ,request + 5, i);
			filename[i] = '\0';
			args_len -= i+1;//+1 for ?
			strncpy(request_args , p + 1, args_len);
			request_args[args_len] = '\0';
			break;
		}
	}
	if(i == args_len){		
		strcpy(request_args,"");
		args_len = 0;
	}
	for(i = 0;i < args_len; i++){
		if(request_args[i] == '&'){
			request_args[i] = '\0';
			args_len = i;
			break;
		}
	}
	if(args_len > 0){
		printf("%s\n",request_args);
		char* val = strstr(request_args, "process=") + 8;//+strlen("process=")
		printf("%s,%d\n",val,strlen(val));
		char* result = NULL;
		if(strncmp(val, "binary", strlen(val)) == 0){
			printf("binary:%s\n",filename);
			result = process(filename, to_binary);
		}else if(strncmp(val, "dilation", strlen(val)) == 0){
			printf("dilation:%s\n",filename);
			result = process(filename, dilation);
		}else if(strncmp(val, "erosion", strlen(val)) == 0){
			printf("erosion:%s\n",filename);
			result = process(filename, erosion);
		}else if(strncmp(val, "opening", strlen(val)) == 0){
			printf("opening:%s\n",filename);
			result = process(filename, opening);
		}else if(strncmp(val, "closing", strlen(val)) == 0){
			printf("closing:%s\n",filename);
			result = process(filename, closing);
		}
		if(result == NULL){
			char response[MAX_REQUEST_SIZE];
			sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
			send(client_socket, response, strlen(response), 0);
			return;
		}
	}
	//split file extension
	char* dot = strrchr(filename, '.');
	char extension[10];
    if (dot != NULL && dot > filename) {
        strcpy(extension, dot + 1);
    } else {
        strcpy(extension, "");
    }
	//set response type
	if(strncmp(extension, "html", 10) == 0){
		strcpy(content_type,"text/html");
	}else if(strncmp(extension, "js", 10) == 0){
		strcpy(content_type,"application/javascript");
	}else if(strncmp(extension, "css", 10) == 0){
		strcpy(content_type,"text/css");
	}else{
		strcpy(content_type,"image/jpeg");
	}
    FILE* file = fopen(filename, "rb");

    if (file) {
        // Read the file contents into a buffer
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* filedata = malloc(filesize);
        fread(filedata, 1, filesize, file);

        // Construct the response with the file contents
        char response[MAX_REQUEST_SIZE];
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", content_type, filesize);
        send(client_socket, response, strlen(response), 0);
        send(client_socket, filedata, filesize, 0);

        free(filedata);
        fclose(file);
    } else {
        // Return a 404 response if the file doesn't exist
        char response[MAX_REQUEST_SIZE];
        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
        send(client_socket, response, strlen(response), 0);
    }
}