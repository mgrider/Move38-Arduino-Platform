/*
 * blinkboot.h
 *
 * This defines stuff that is needed to transfer games to the bootloader
 *
 */

// TODO: This is currently duped in blinkos and bootloader. Find a way to have a single copy of this file.

#define DOWNLOAD_PAGE_SIZE 128      // Flash pages size for ATMEGA168PB
                                    // We currently send in full pages because it save the hassle of reassembling packets, but does mean
                                    // that we must have bigger buffers. Maybe makes sense to send blocks instead?

#define DOWNLOAD_MAX_PAGES 56       // The maximum length of a downloaded game
                                    // Currently set to use ~7KB. This saves 7KB for built in game and 2KB for bootloader

// These header bytes are chosen to try and give some error robustness.
// So, for example, a header with a repeating pattern would be less robust
// because it is possible something blinking in the environment might replicate it

#define IR_PACKET_HEADER_PULLREQUEST     0b01101010      // If you get this, then the other side is saying they want to send you a game
#define IR_PACKET_HEADER_PULLFLASH       0b01011101      // You send this to request the next block of a game
#define IR_PACKET_HEADER_PUSHFLASH       0b11011011      // This contains a block of flash code to be programmed into the active area


struct push_payload_t {                 // Response to a pull with the flash block we asked for
    uint8_t data[DOWNLOAD_PAGE_SIZE];   // An actual page of the flash memory
    uint8_t page;                       // The block number in this packet. This comes after the data to keep the data word aligned.
    uint8_t packet_checksum;            // Simple sum of all preceding bytes in packet including header, then inverted. This comes at the end so we can compute it on the fly.
};

struct seed_payload_t {           // Sending blink telling neighbor there is a new game he needs to download
    uint8_t pages;                        // How many total blocks in this game? We put this first in case the compile wants to pad the header byte
    uint16_t program_checksum;            // The checksum of all the flash data in all of the packets with each page also has added in its page number
    uint8_t packet_checksum;            // Simple sum of all preceding bytes in packet including header, then inverted. This comes at the end so we can compute it on the fly.
};


struct pull_payload_t {                   // Blink asking neighbor for a block of a new game
    uint8_t page;                       // The block we want the neighbor to send
};


struct blinkboot_packet {

    uint8_t header;                             // One of the three above packet types

    union {

        push_payload_t          push_payload;
        seed_payload_t          seed_payload;
        pull_payload_t          pull_payload;

    };
};
