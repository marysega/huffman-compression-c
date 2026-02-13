# Huffman Coding Explained (short)

Huffman coding is a compression method that assigns **shorter binary codes to frequent symbols**
and **longer codes to rare symbols**, reducing the overall file size.

## Core idea
1. Compute frequency of each byte/character in the input
2. Build a binary tree:
   - combine the 2 least frequent nodes repeatedly
   - each combination creates a new node with summed frequency
3. Generate codes:
   - left edge = 0, right edge = 1 (convention)
   - each symbol gets a unique prefix-free binary code

## Why it works
The tree creates **prefix-free** codes (no code is the prefix of another), so decoding is unambiguous.

## Compression / decompression
- Compression: write a header (frequencies or tree) + encoded bitstream
- Decompression: rebuild the tree from the header, then decode bits by traversing the tree
