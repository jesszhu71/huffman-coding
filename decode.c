// Decode

#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// Extern vars, used for stats
uint64_t bytes_read;
uint64_t bytes_written;

int main(int argc, char **argv) {
    bytes_read = 0;
    bytes_written = 0;
    int opt = 0;
    bool help = false;
    int infile = STDIN_FILENO;
    int outfile = STDIN_FILENO;
    bool v = false;
    bool not_option = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            // h is selected
            help = true;
            break;
        case 'i':
            // i is selected
            infile = open(optarg, O_RDONLY);
            if (infile < 0) {
                printf("ERROR INFILE\n");
                return 1;
            }
            break;
        case 'o':
            // o is selected
            outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (outfile < 0) {
                printf("ERROR OUTFILE\n");
                return 1;
            }
            break;
        case 'v':
            // v is selected
            v = true;
            break;
        default:
            // Something not an option was picked
            not_option = true;
            break;
        }
    }

    // Print synopsis
    if (help || not_option) {
        printf("SYNOPSIS\n");
        printf("  A Huffman encoder.\n");
        printf("  Decompresses a file using the Huffman coding algorithm.\n");
        printf("\n");
        printf("USAGE\n");
        printf("  ./decode [-h] [-i infile] [-o outfile] [-v]\n");
        printf("\n");
        printf("OPTIONS\n");
        printf("  -h             Program usage and help.\n");
        printf("  -v             Pint compression statistics.\n");
        printf("  -i infile      Input file to decompress.\n");
        printf("  -o outfile     Output of decompressed data.\n");

        close(infile);
        close(outfile);
        if (not_option) {
            return 1;
        }
        return 0;
    }

    // 1. Read in header and verify
    Header h;

    read_bytes(infile, (uint8_t *) &h, sizeof(h));
    // problem if magic is not magic
    if (h.magic != MAGIC) {
        printf("Invalid magic number.\n");
        close(infile);
        close(outfile);
        return 1;
    }

    // 2. Set outfile permissions
    fchmod(outfile, h.permissions);

    // 3. Read tree dump and rebuild
    uint8_t t_dump[h.tree_size];
    read_bytes(infile, t_dump, h.tree_size);
    Node *root = rebuild_tree(h.tree_size, t_dump);

    // 4. Read infile using readbit and traverse down tree one link at a time for each bit
    uint8_t write_to_outfile[h.file_size]; //array to keep track of what to write to outfile
    uint32_t outfile_index = 0; // index in ^^ array
    Node *curr_node = root; // start at root node
    while (outfile_index < h.file_size) { // while not all bytes have been read
        if (curr_node && curr_node->left == NULL && curr_node->right == NULL) {
            // leaf found, put symbol into array being written into outfile, reset traverse.
            write_to_outfile[outfile_index] = curr_node->symbol;
            outfile_index = outfile_index + 1;
            curr_node = root;
        } else if (curr_node) {
            // Not leaf, so read and traverse
            uint8_t curr_bit;
            read_bit(infile, &curr_bit);
            if (curr_bit == 0) {
                // 0 = left
                curr_node = curr_node->left;
            } else {
                // 1 = right
                curr_node = curr_node->right;
            }
        }
    }
    // write array of symbols into outfile
    write_bytes(outfile, write_to_outfile, h.file_size);

    // Print stats here
    double compressed = bytes_read;
    double decompressed = bytes_written;
    double saved = 100 * (1 - (compressed / decompressed));

    if (v) {
        fprintf(stderr, "Compressed file size: %lu bytes\n", (uint64_t) compressed);
        fprintf(stderr, "Decompressed file size: %lu bytes\n", (uint64_t) decompressed);
        fprintf(stderr, "Space saving: %5.2f%%\n", saved);
    }

    // delete and close files
    delete_tree(&root);
    close(infile);
    close(outfile);
}
