#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./img_lib/stb_image.h"
#include "./img_lib/stb_image_write.h"
#include "image_process.h"

Image to_binary(Image input_image){
	Image binary;
	binary.image = (unsigned char*) malloc(input_image.width*input_image.height);	
	if(binary.image==NULL){
		printf("Could not allocate memory");
		exit(0);
	}  
	binary.height = input_image.height;
	binary.width = input_image.width;
	binary.channel = 1;
	int color;
	for(int x=0;x<input_image.width;x++){
		for(int y=0;y<input_image.height;y++){
			color = 0;
			for(int d=0;d<input_image.channel;d++){
		        color += get_pix(x,y,input_image,d);
		    }
		    if(color/input_image.channel > 127){
		    	get_pix(x,y,binary,0)= -1;
			}else{
				get_pix(x,y,binary,0)= 0;
			}
    	}
	}
	return binary;
}
Image fill(Image input_image, unsigned char color){
	for(int i=0;i<input_image.width*input_image.height*input_image.channel;i++){
	    input_image.image[i]=color;//r=g=b=255=white
    }
	return input_image;
}

Image dilation(Image input_image){
	if(input_image.channel > 1){
		printf("Only binary image can be process!!!");
		return input_image;
	}
	Image binary;
	binary.image = (unsigned char*) malloc(input_image.width*input_image.height);	
	binary.height = input_image.height;
	binary.width = input_image.width;
	binary.channel = 1;
    fill(binary,0);
	for(int x=1;x<input_image.width-1;x++){
		for(int y=1;y<input_image.height-1;y++){
		    if(get_pix(x,y,input_image,0) > 127){
				get_pix(x-1,y-1,binary,0)= -1;
		    	get_pix(x-1,y,binary,0)= -1;
				get_pix(x-1,y+1,binary,0)= -1;
				get_pix(x,y-1,binary,0)= -1;
				get_pix(x,y,binary,0)= -1;
				get_pix(x,y+1,binary,0)= -1;
				get_pix(x+1,y-1,binary,0)= -1;
				get_pix(x+1,y,binary,0)= -1;
				get_pix(x+1,y+1,binary,0)= -1;
			}
    	}
	}
	return binary;
}

Image erosion(Image input_image){
	if(input_image.channel > 1){
		printf("Only binary image can be process!!!");
		return input_image;
	}
	Image binary;
	binary.image = (unsigned char*) malloc(input_image.width*input_image.height);	
	binary.height = input_image.height;
	binary.width = input_image.width;
	binary.channel = 1;
    fill(binary,-1);
	for(int x=1;x<input_image.width-1;x++){
		for(int y=1;y<input_image.height-1;y++){
		    if(get_pix(x,y,input_image,0) < 127){
				get_pix(x-1,y-1,binary,0)= 0;
		    	get_pix(x-1,y,binary,0)= 0;
				get_pix(x-1,y+1,binary,0)= 0;
				get_pix(x,y-1,binary,0)= 0;
				get_pix(x,y,binary,0)= 0;
				get_pix(x,y+1,binary,0)= 0;
				get_pix(x+1,y-1,binary,0)= 0;
				get_pix(x+1,y,binary,0)= 0;
				get_pix(x+1,y+1,binary,0)= 0;
			}
    	}
	}
	return binary;
}

Image opening(Image input_image){
	Image output_image;
    output_image = erosion(input_image);
	output_image = dilation(output_image);
	return output_image;
}

Image closing(Image input_image){
	Image output_image;
    output_image = dilation(input_image);
	output_image = erosion(output_image);
	return output_image;
}

char* process(char* path, Image (*function)(Image)){
	printf("processing...\n");
    //Initial
    int w,h,n;
    Image input_image;
    input_image.image =  stbi_load(path, &w, &h, &n, 0);//load image
	if(input_image.image == NULL)return NULL;
    input_image.width = w;
    input_image.height = h;
    input_image.channel = n;
    printf("%d, %d, %d\n", w, h, n);
	
	Image binary_image;
    binary_image = to_binary(input_image);
    Image output_image;
    output_image = function(binary_image);
	
	//output image
	char *pic_name = path;
	//find the filename
	for(int i=0;pic_name[i]!='\0';i++){
		if(pic_name[i]=='\\'){			
			pic_name+=i+1;
			i=0;
		}
	}
	//save image
	save_image(output_image,".",pic_name);
	//free memory
	stbi_image_free(input_image.image);
    free(binary_image.image);	
    free(output_image.image);
    printf("image saved\n");
    return path;
}

void save_image(Image image,char* path,char* name){	
	char str_buffer[50];
	struct stat st = {0};
	sprintf(str_buffer,"%s",path);
	if (stat(str_buffer, &st) == -1) {
		mkdir(str_buffer, 0700);
	}
	sprintf(str_buffer,"%s\\%s",str_buffer,name);
	printf("%s\n",str_buffer);
	stbi_write_jpg(str_buffer, image.width, image.height,image.channel, image.image, 100);
}