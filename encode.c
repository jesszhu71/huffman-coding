// Encode

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

// Traverses the tree, prints tree dump to outfile
void post_dump(Node *n, uint8_t *tree, uint32_t tree_size, int outfile) {
    static uint32_t t_index = 0;
    if (n && n->left == NULL && n->right == NULL) {
        // current node is leaf, print L and node symbol
        tree[t_index] = 'L';
        tree[t_index + 1] = n->symbol;
        t_index = t_index + 2;

    } else if (n) {
        //current node is parent, print I
        post_dump(n->left, tree, tree_size, outfile);
        post_dump(n->right, tree, tree_size, outfile);
        // print I
        tree[t_index] = 'I';
        t_index = t_index + 1;
    }
    // if entire treedump has been filled, print dump to outfile
    if (t_index == tree_size) {
        write_bytes(outfile, tree, tree_size);
    }
}

int main(int argc, char **argv) {
    bytes_read = 0;
    bytes_written = 0;
    int opt = 0;
    bool help = false;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
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
        printf("  Compresses a file using the Huffman coding algorithm.\n");
        printf("\n");
        printf("USAGE\n");
        printf("  ./encode [-h] [-i infile] [-o outfile] [-v]\n");
        printf("\n");
        printf("OPTIONS\n");
        printf("  -h             Program usage and help.\n");
        printf("  -v             Pint compression statistics.\n");
        printf("  -i infile      Input file to compress.\n");
        printf("  -o outfile     Output of compressed data.\n");

        close(infile);
        close(outfile);
        if (not_option) {
            return 1;
        }
        return 0;
    }

    // HUFFMAN ENCODING BEGINS HERE!

    // 1. Create histogram.
    uint64_t histogram[ALPHABET] = { 0 };
    histogram[0] = histogram[0] + 1;
    histogram[255] = histogram[255] + 1;

    //2. Read and go through each byte read in buff and add to histogram

    // create temp file if stdin
    bool std_in = false; // is stdin used?
    int temp;
    if (infile == STDIN_FILENO) {
        temp = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
        std_in = true;
    }

    // read and sort into histogram
    int read = BLOCK;
    uint32_t unique = 2; //number of unique symbols
    // reads the entire file
    while (read == BLOCK) {
        uint8_t buffer[BLOCK];
        read = read_bytes(infile, buffer, BLOCK);
        // copy into temp file is stdin is used
        if (std_in) {
            write_bytes(temp, buffer, read);
        }
        // sort everythign in buffer into histogram
        for (int i = 0; i < read; i = i + 1) {
            uint8_t curr_byte = buffer[i];
            if (histogram[curr_byte] == 0) {
                unique = unique + 1;
            }
            histogram[curr_byte] = histogram[curr_byte] + 1;
        }
    }

    // Close and reopen temp file as infile.
    if (std_in) {
        close(temp);
        infile = open("temp.txt", O_RDONLY);
    }

    // 3. Build Huffman tree, using build_tree()
    Node *tree_root = build_tree(histogram);

    // 4. Create code table
    Code table[ALPHABET];
    build_codes(tree_root, table);

    // 5. Header stuff
    struct stat statbuf;
    fstat(infile, &statbuf);
    fchmod(outfile, statbuf.st_mode);

    Header h;
    h.magic = MAGIC;
    h.permissions = statbuf.st_mode;
    h.tree_size = (3 * unique) - 1;
    h.file_size = statbuf.st_size;

    // 6. Write header to outfile

    write_bytes(outfile, (uint8_t *) &h, sizeof(Header));

    // 7. Post order traversal of Huffman tree. L leaf nodes, I for interior nodes.
    uint8_t tree[h.tree_size];
    post_dump(tree_root, tree, h.tree_size, outfile);

    // 8. Go through entire file, print each corresponding code for each symbol to outfil

    //RESET FILE READING HERE!
    lseek(infile, 0, SEEK_SET);
    read = 1;
    // reads entire infile
    while (read > 0) {
        uint8_t buffer[BLOCK];
        read = read_bytes(infile, buffer, BLOCK);
        for (int i = 0; i < read; i = i + 1) {
            uint8_t curr_byte = buffer[i];
            write_code(outfile, &table[curr_byte]);
        }
    }
    // flushes whatever is still in buffer
    flush_codes(outfile);

    // Print stats here
    double uncompressed = bytes_read / 2;
    double compressed = bytes_written;
    if (std_in) {
        compressed = compressed - uncompressed;
    }
    double saved = (1 - (compressed / uncompressed)) * 100;
    if (v) {
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", (uint64_t) uncompressed);
        fprintf(stderr, "Compressed file size: %lu bytes\n", (uint64_t) compressed);
        fprintf(stderr, "Space saving: %5.2f%%\n", saved);
    }

    // clear tree
    delete_tree(&tree_root);

    // close files
    close(infile);
    close(outfile);

    // remove temp file if needed
    if (std_in) {
        unlink("temp.txt");
    }

    return 0;
}
