# Assignment 6
This program encodes and decodes the files using huffman encoder and decoder. Files and compressed in encode and decompressed in decode in a lossless manner. Entropy will measure the amount of entropy in a file.
## Build

	$ make

## Running

	$ ./encode [-h] [-v] [-i infile] [-o outfile]
  	$ ./decode [-h] [-v] [-i infile] [-o outfile]
  	$ ./entropy < [input (reads from stdin)]

 **COMMAND LINE OPTIONS:**
 - -h: Program usage and help.
 - -v: Print compression statistics
 - -i infile: Input data to compress/decompress
 - -o outfile: Output of decompressed/compressed data


## Cleaning

	$ make clean
