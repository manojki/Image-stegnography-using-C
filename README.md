# Image Steganography using C

## Overview

This project implements image steganography in C using the Least Significant Bit (LSB) technique. It allows users to securely hide a text file inside a BMP image and later extract it without noticeably affecting the image quality.

## Features

* Encode secret text into BMP images
* Decode hidden text from stego images
* Command-line interface
* Binary file handling
* Bitwise manipulation using the LSB algorithm
* Input validation and error handling

## Technologies Used

* C Programming
* Visual Studio Code
* GCC
* Linux
* File Handling
* Bit Manipulation

## Project Structure

* Encoding Module
* Decoding Module
* BMP Image Processing
* Command-line Argument Validation

## How to Compile

```bash
gcc *.c -o steg
```

## How to Run

### Encoding

```bash
./steg -e input.bmp secret.txt output.bmp
```

### Decoding

```bash
./steg -d output.bmp decoded.txt
```

## Skills Demonstrated

* C Programming
* File Handling
* Bit Manipulation
* Binary File Processing
* Debugging
* Command-line Programming
