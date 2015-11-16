#include "sha1.h"
#include <stdio.h>


/**
 * THIS IS A TESTBENCH FOR SHA1 PASSWORD CRACKING
 * \author Fabian Kammel <fabian.kammel@rub.de>
 * \author Maximilian Golla <maximilian.golla@rub.de>
 *
 * PLEASE SUPPLY YOUR IMPLEMENTATION HERE
 * AND RETURN THE STRING THAT WAS FOUND BY
 * YOUR IMPLEMENTATION
 * 
 * THE WHOLE PROJECT MUST COMPILE WITH: 
 * gcc -O2 -Wall -fomit-frame-pointer -msse2 -masm=intel testbench.c <YOUR IMPL>.c -o crackSHA1
 * 
 * ANY FURTHER INFORMATION CAN BE FOUND IN THE
 * PROBLEM DESCRIPTION
 * 
 * PROGRAMMAUFRUF MIT 
 * ./crackSHA1 33207880274e07b1b72d7183fdf9b196614b5a7f
 * ENTSPRECHENDES URBILD IST "gruppe"
 */

int crackHash(struct state hash, char *result) {
    //initialisiere Konstanten
    const uint32_t h0 = 0x67452301;
    const uint32_t h1 = 0xEFCDAB89;
    const uint32_t h2 = 0x98BADCFE;
    const uint32_t h3 = 0x10325476;
    const uint32_t h4 = 0xC3D2E1F0;
    
    const uint32_t k0 = 0x5A827999;
    const uint32_t k1 = 0x6ED9EBA1;
    const uint32_t k2 = 0x8F1BBCDC;
    const uint32_t k3 = 0xCA62C1D6;
    
    
    int i;
    uint32_t a, b, c, d, e, f, temp;
    
    /** 
     * Vorbereitung der Nachricht:
     * m € [a-z]^6, Initialisierung mit m = aaaaaa
     * Länge von m muss Vielfaches von 512 Bit sein
     * 6 Buchstaben = 48 Bit
     * 1. Hänge '1' an -> 10000000b = 0x80
     * 2. Hänge '0' an bis Länge = 448 Bit
     * 3. Hänge Bitlänge der Nachricht (immer 48) als big-endian an
     * -> m15 = 0, m16 = 48;
     */
    uint32_t m[80];
    m[0] = 0x61616161;
    m[1] = 0x61618000;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 0;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 0;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 48;
    
    // erweitere die 16 32-Bit-Worte auf 80 32-Bit-Worte
    for(i = 16; i < 80; i++) {
        m[i] = ((m[i-3] ^ m[i-8] ^ m[i-14] ^ m[i-16]) << 1) 
                | ((m[i-3] ^ m[i-8] ^ m[i-14] ^ m[i-16]) >> 31); 
    }
    
    a = h0;
    b = h1;
    c = h2;
    d = h3;
    e = h4;
    
    for(i = 0; i < 20; i++) {
        f = d ^ (b & (c ^ d));
        temp = ((a << 5) | (a >> 27)) + f + e + k0 + m[i];
        e = d;
        d = c;
        c = ((b << 30) | (b >> 2));
        b = a;
        a = temp;
    }
    for(i = 20; i < 40; i++) {
        f = b ^ c ^ d;
        temp = ((a << 5) | (a >> 27)) + f + e + k1 + m[i];
        e = d;
        d = c;
        c = ((b << 30) | (b >> 2));
        b = a;
        a = temp;
    }
    for(i = 40; i < 60; i++) {
        f = (b & c) | (d & (b | c));
        temp = ((a << 5) | (a >> 27)) + f + e + k2 + m[i];
        e = d;
        d = c;
        c = ((b << 30) | (b >> 2));
        b = a;
        a = temp;
    }
    for(i = 60; i < 80; i++) {
        f = b ^ c ^ d;
        temp = ((a << 5) | (a >> 27)) + f + e + k3 + m[i];
        e = d;
        d = c;
        c = ((b << 30) | (b >> 2));
        b = a;
        a = temp;
    }
    
    if((h0 + a) == hash.a && (h1 + b) == hash.b && (h2 + c) == hash.c 
            && (h3 + d) == hash.d && (h4 + e) == hash.e) {
        result[0] = 'a';
        result[1] = 'a';
        result[2] = 'a';
        result[3] = 'a';
        result[4] = 'a';
        result[5] = 'a';
        /* Found */
        return(EXIT_SUCCESS);
    }
    /* Not found */
    return(EXIT_FAILURE);
}