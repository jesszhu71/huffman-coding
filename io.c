// I/O ADT

#include "code.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

//static and extern vars/arrays
uint64_t bytes_read;
uint64_t bytes_written;
static uint8_t buf[BLOCK];
static uint32_t buf_index = 0;

// Reads in nbtyes from infile into buf.
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int total = 0;
    int curr_call = 1; // is 1 bc we need to enter the while loop
    // Reads until nbytes are read.
    while (curr_call > 0 && total != nbytes) {
        curr_call = read(infile, buf, nbytes - total);
        total = total + curr_call;
    }
    bytes_read = bytes_read + total;
    return total;
}

// Writes nbytes from buf to outfile
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int total = 0;
    int curr_call = 1; // is 1 bc we need to enter the while loop
    // Writes until there are no more bytes in buffer to write
    while (curr_call > 0 && total != nbytes) {
        curr_call = write(outfile, buf, nbytes - total);
        total = total + curr_call;
    }
    bytes_written = bytes_written + total;
    return total;
}

// Reads 1 bit at a time from infile
bool read_bit(int infile, uint8_t *bit) {
    uint32_t last_bit = 8 * BLOCK;

    //fills the buffer is empty
    if (buf_index == 0) {
        int read = read_bytes(infile, buf, BLOCK);
        // when entire fil has been read, last bit doesn't land on last byte of buffer
        if (read < BLOCK) {
            last_bit = 8 * read + 1;
        }
    }
    // Get bit
    uint32_t location = buf_index / 8;
    uint32_t position = buf_index % 8;
    uint8_t curr_bit = ((1 << position) & buf[location]) >> position;
    // Pass back the bit
    *bit = curr_bit;
    buf_index = (buf_index + 1) % (BLOCK * 8); // resets index if buffer is all read
    if (last_bit == buf_index) { // if end of file reached?
        return false;
    } else {
        return true;
    }
}

// Writes code into buffer which is then written out into outfile when buffer is full.
void write_code(int outfile, Code *c) {
    //Eugenes section psuedocode
    //Buffer each bit of code
    for (uint32_t i = 0; i < code_size(c); i = i + 1) {
        // Get  bit
        uint32_t location = buf_index / 8;
        uint32_t position = buf_index % 8;
        uint8_t curr_bit = ((1 << (i % 8)) & c->bits[i / 8]) >> (i % 8);
        if (curr_bit == 1) {
            // Set bit
            uint8_t temp = 1 << position;
            buf[location] = buf[location] | temp;
        } else {
            // Clear bit
            uint8_t temp = ~(1 << position);
            buf[location] = buf[location] & temp;
        }
        buf_index = buf_index + 1;
        //if buffer is full, write buffer out
        if (buf_index == 8 * BLOCK) {
            write_bytes(outfile, buf, BLOCK);
            buf_index = 0;
        }
    }
}

void flush_codes(int outfile) {
    // if buffer still have remaining bits
    if (buf_index > 0) {
        int num_write = buf_index / 8;
        if (buf_index % 8 != 0) {
            num_write = num_write + 1;
        }
        // write out remaining number of bytes
        write_bytes(outfile, buf, num_write);
    }
    // RESET INDEX?
    buf_index = 0;
}
