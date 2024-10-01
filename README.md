# Web Image Editor

This program provides a web interface for basic image processing operations.

## Features

* Upload images through a web form.
* Apply binary thresholding, dilation, erosion, opening, and closing operations.
* View the processed images in the browser.

## Requirements

* Windows
* Winsock2 library

## Compile

Compile the source code using the provided command:
   ```bash
   gcc -o "<output_dir_path>\main.exe" "<dir_path>\http_server.c" "<dir_path>\gethandler.c" "<dir_path>\posthandler.c" "<dir_path>\image_process.c" -lws2_32
   ```

## Usage
1. Run the compiled executable.
2. Open a web browser and navigate to http://localhost:[port], where [port] is the port number displayed in the console.
3. Upload an image using the web form and select a processing operation.
4. The processed image will be displayed in the browser.

## Note
1. The server automatically opens the web page in the default browser when started.
2. The supported image format is JPEG.
3. The processed images are saved in the upload folder.

## Disclaimer
This is a basic image processing web application. It may not handle all image formats or complex operations.
