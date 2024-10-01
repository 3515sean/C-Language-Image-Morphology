#define get_pix(x,y,Image,sw) Image.image[((x)+(y)*Image.width)*Image.channel+(sw)]
#include <sys/types.h>

int mkdir(const char *pathname, mode_t mode);

struct{
	unsigned char *image;
	int height;
	int width;
	int channel;
}typedef Image;
Image to_binary(Image input_image);
Image dilation(Image input_image);
Image erosion(Image input_image);
Image opening(Image input_image);
Image closing(Image input_image);
char* process(char* path, Image (*function)(Image));
void save_image(Image image,char* path,char* name);