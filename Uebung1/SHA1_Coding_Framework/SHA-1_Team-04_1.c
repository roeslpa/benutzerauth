#include "sha1.h"
#include <stdio.h>

#define ROL(m) (m << 1) | (m >> 31)
#define ROLL(m,i) (m << i) | (m >> (32-i))
#define F1(f,b,c,d) f = d ^ (b & (c ^ d))
#define F2(f,b,c,d) f = b ^ c ^ d
#define F3(f,b,c,d) f = (b & c) | (d & (b | c))
#define FF(a,b,c,d,e,f,k,m) temp = ((a << 5) | (a >> 27)) + f + e + k + m; e = d; d = c; c = ((b << 30) | (b >> 2)); b = a; a = temp;

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
    
    
    //int i;
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
    uint32_t m[80],pm[80],w[25];
    m[0] = 0x61616161;
    m[1] = 0x61618000;
    m[15] = 48;

    unsigned l5,l4,l3,l2,l1,l0;

    for(l5=0; l5<26; l5++) {
        m[1] = (m[1]& ~(0xff<<16))+((l5+'a')<<16);
        for(l4=0; l4<26; l4++) {
            m[1] = (m[1]& ~(0xff<<24))+((l4+'a')<<24);

            pm[16] = 0;
            pm[17] = ROL(m[1]);
            pm[18] = 96;
            pm[19] = ROL(pm[16]);
            pm[20] = ROL(pm[17]);
            pm[21] = 192;
            pm[22] = ROL(pm[19]);
            pm[23] = ROL(pm[20] ^ 48);
            pm[24] = ROL(192 ^ pm[16]);
            pm[25] = ROL(pm[22] ^ pm[17]);
            pm[26] = ROL(pm[23] ^ 96);
            pm[27] = ROL(pm[24] ^ pm[19]);
            pm[28] = ROL(pm[25] ^ pm[20]);
            pm[29] = ROL(pm[26] ^ 192 ^ 48);
            pm[30] = ROL(pm[27] ^ pm[22] ^ pm[16]);
            pm[31] = ROL(pm[28] ^ pm[23] ^ pm[17] ^ 48);
            pm[32] = ROL(pm[29] ^ pm[24] ^ 96 ^ pm[16]);
            pm[33] = ROL(pm[30] ^ pm[25] ^ pm[19] ^ pm[17]);
            pm[34] = ROL(pm[31] ^ pm[26] ^ pm[20] ^ 96);
            pm[35] = ROL(pm[32] ^ pm[27] ^ 192 ^ pm[19]);
            pm[36] = ROL(pm[33] ^ pm[28] ^ pm[22] ^ pm[20]);
            pm[37] = ROL(pm[34] ^ pm[29] ^ pm[23] ^ 192);
            pm[38] = ROL(pm[35] ^ pm[30] ^ pm[24] ^ pm[22]);
            pm[39] = ROL(pm[36] ^ pm[31] ^ pm[25] ^ pm[23]);
            pm[40] = ROL(pm[37] ^ pm[32] ^ pm[26] ^ pm[24]);
            pm[41] = ROL(pm[38] ^ pm[33] ^ pm[27] ^ pm[25]);
            pm[42] = ROL(pm[39] ^ pm[34] ^ pm[28] ^ pm[26]);
            pm[43] = ROL(pm[40] ^ pm[35] ^ pm[29] ^ pm[27]);
            pm[44] = ROL(pm[41] ^ pm[36] ^ pm[30] ^ pm[28]);
            pm[45] = ROL(pm[42] ^ pm[37] ^ pm[31] ^ pm[29]);
            pm[46] = ROL(pm[43] ^ pm[38] ^ pm[32] ^ pm[30]);
            pm[47] = ROL(pm[44] ^ pm[39] ^ pm[33] ^ pm[31]);
            pm[48] = ROL(pm[45] ^ pm[40] ^ pm[34] ^ pm[32]);
            pm[49] = ROL(pm[46] ^ pm[41] ^ pm[35] ^ pm[33]);
            pm[50] = ROL(pm[47] ^ pm[42] ^ pm[36] ^ pm[34]);
            pm[51] = ROL(pm[48] ^ pm[43] ^ pm[37] ^ pm[35]);
            pm[52] = ROL(pm[49] ^ pm[44] ^ pm[38] ^ pm[36]);
            pm[53] = ROL(pm[50] ^ pm[45] ^ pm[39] ^ pm[37]);
            pm[54] = ROL(pm[51] ^ pm[46] ^ pm[40] ^ pm[38]);
            pm[55] = ROL(pm[52] ^ pm[47] ^ pm[41] ^ pm[39]);
            pm[56] = ROL(pm[53] ^ pm[48] ^ pm[42] ^ pm[40]);
            pm[57] = ROL(pm[54] ^ pm[49] ^ pm[43] ^ pm[41]);
            pm[58] = ROL(pm[55] ^ pm[50] ^ pm[44] ^ pm[42]);
            pm[59] = ROL(pm[56] ^ pm[51] ^ pm[45] ^ pm[43]);
            pm[60] = ROL(pm[57] ^ pm[52] ^ pm[46] ^ pm[44]);
            pm[61] = ROL(pm[58] ^ pm[53] ^ pm[47] ^ pm[45]);
            pm[62] = ROL(pm[59] ^ pm[54] ^ pm[48] ^ pm[46]);
            pm[63] = ROL(pm[60] ^ pm[55] ^ pm[49] ^ pm[47]);
            pm[64] = ROL(pm[61] ^ pm[56] ^ pm[50] ^ pm[48]);
            pm[65] = ROL(pm[62] ^ pm[57] ^ pm[51] ^ pm[49]);
            pm[66] = ROL(pm[63] ^ pm[58] ^ pm[52] ^ pm[50]);
            pm[67] = ROL(pm[64] ^ pm[59] ^ pm[53] ^ pm[51]);
            pm[68] = ROL(pm[65] ^ pm[60] ^ pm[54] ^ pm[52]);
            pm[69] = ROL(pm[66] ^ pm[61] ^ pm[55] ^ pm[53]);
            pm[70] = ROL(pm[67] ^ pm[62] ^ pm[56] ^ pm[54]);
            pm[71] = ROL(pm[68] ^ pm[63] ^ pm[57] ^ pm[55]);
            pm[72] = ROL(pm[69] ^ pm[64] ^ pm[58] ^ pm[56]);
            pm[73] = ROL(pm[70] ^ pm[65] ^ pm[59] ^ pm[57]);
            pm[74] = ROL(pm[71] ^ pm[66] ^ pm[60] ^ pm[58]);
            pm[75] = ROL(pm[72] ^ pm[67] ^ pm[61] ^ pm[59]);

            for(l3=0; l3<26; l3++) {
                m[0] = (m[0]& ~(0xff<<0))+((l3+'a')<<0);
                for(l2=0; l2<26; l2++) {
                    m[0] = (m[0]& ~(0xff<<8))+((l2+'a')<<8);
                    for(l1=0; l1<26; l1++) {
                        m[0] = (m[0]& ~(0xff<<16))+((l1+'a')<<16);
                        for(l0=0; l0<26; l0++) {
                            m[0] = (m[0]& ~(0xff<<24))+((l0+'a')<<24);

    w[1] = ROLL(m[0],1);
    w[2] = ROLL(m[0],2);
    w[3] = ROLL(m[0],3);
    w[4] = ROLL(m[0],4);
    w[5] = ROLL(m[0],5);
    w[6] = ROLL(m[0],6);
    w[7] = ROLL(m[0],7);
    w[8] = ROLL(m[0],8);
    w[9] = ROLL(m[0],9);
    w[10] = ROLL(m[0],10);
    w[11] = ROLL(m[0],11);
    w[12] = ROLL(m[0],12);
    w[13] = ROLL(m[0],13);
    w[14] = ROLL(m[0],14);
    w[15] = ROLL(m[0],15);
    w[16] = ROLL(m[0],16);
    w[17] = ROLL(m[0],17);
    w[18] = ROLL(m[0],18);
    w[19] = ROLL(m[0],19);
    w[20] = ROLL(m[0],20);
    //6__4, 8__4, 8__12, 6__4__7
    w[21] = w[6] ^ w[4];
    w[22] = w[8] ^ w[4];
    w[23] = w[8] ^ w[12];
    w[24] = w[6] ^ w[4] ^ w[7];
    
    m[16] = pm[16] ^ w[1];
    m[17] = pm[17];
    m[18] = pm[18];
    m[19] = pm[19] ^ w[2];
    m[20] = pm[20];
    m[21] = pm[21];
    m[22] = pm[22] ^ w[3];
    m[23] = pm[23];
    m[24] = pm[24] ^ w[2];
    m[25] = pm[25] ^ w[4];
    m[26] = pm[26];
    m[27] = pm[27];
    m[28] = pm[28] ^ w[5];
    m[29] = pm[29];
    m[30] = pm[30] ^ w[4] ^ w[2];
    m[31] = pm[31] ^ w[6];
    m[32] = pm[32] ^ w[3] ^ w[2];
    m[33] = pm[33];
    m[34] = pm[34] ^ w[7];
    m[35] = pm[35] ^ w[4];
    m[36] = pm[36] ^ w[21];
    m[37] = pm[37] ^ w[8];
    m[38] = pm[38] ^ w[4];
    m[39] = pm[39];
    m[40] = pm[40] ^ w[4] ^ w[9];
    m[41] = pm[41];
    m[42] = pm[42] ^ w[6] ^ w[8];
    m[43] = pm[43] ^ w[10];
    m[44] = pm[44] ^ w[6] ^ w[3] ^ w[7];
    m[45] = pm[45];
    m[46] = pm[46] ^ w[4] ^ w[11];
    m[47] = pm[47] ^ w[22];
    m[48] = pm[48] ^ w[22] ^ w[3] ^ w[10] ^ w[5];
    m[49] = pm[49] ^ w[12];
    m[50] = pm[50] ^ w[8];
    m[51] = pm[51] ^ w[21];
    m[52] = pm[52] ^ w[22] ^ w[13];
    m[53] = pm[53];
    m[54] = pm[54] ^ w[7] ^ w[10] ^ w[12];
    m[55] = pm[55] ^ w[14];
    m[56] = pm[56] ^ w[24] ^ w[11] ^ w[10];
    m[57] = pm[57] ^ w[8];
    m[58] = pm[58] ^ w[22] ^ w[15];
    m[59] = pm[59] ^ w[23];
    m[60] = pm[60] ^ w[23] ^ w[4] ^ w[7] ^ w[14];
    m[61] = pm[61] ^ w[16];
    m[62] = pm[62] ^ w[21] ^ w[23];
    m[63] = pm[63] ^ w[8];
    m[64] = pm[64] ^ w[24] ^ w[23] ^ w[17];
    m[65] = pm[65];
    m[66] = pm[66] ^ w[14] ^ w[16];
    m[67] = pm[67] ^ w[8] ^ w[18];
    m[68] = pm[68] ^ w[11] ^ w[14] ^ w[15];
    m[69] = pm[69];
    m[70] = pm[70] ^ w[12] ^ w[19];
    m[71] = pm[71] ^ w[12] ^ w[16];
    m[72] = pm[72] ^ w[11] ^ w[12] ^ w[18] ^ w[13] ^ w[16] ^ w[5];
    m[73] = pm[73] ^ w[20];
    m[74] = pm[74] ^ w[8] ^ w[16];
    m[75] = pm[75] ^ w[6] ^ w[12] ^ w[14];

    /*
    1.
    for(i = 0; i < 20; i++) {
        f = d ^ (b & (c ^ d));
        temp = ((a << 5) | (a >> 27)) + f + e + k0 + m[i];
        e = d;
        d = c;
        c = ((b << 30) | (b >> 2));
        b = a;
        a = temp;
    }
    2.
    for(i = 0; i < 20; i++) {
        F1(f,b,c,d);
        FF(a,b,c,d,e,f,k0,m[i]);
    }
    3.
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,m[0]);
    4.
    FF(a,b,c,d,e,0x98badcfe,k0,m[0]);
    5.
    a = 0x9fb498b3 + m[0]; e = h3; d = h2; c = 0x7bf36ae2; b = h0;
    FF(a,b,c,d,e,0xfbfbfefe,k0,m[1]);
    */
    
    e = h2;
    c = 0x59d148c0;
    d = 0x7bf36ae2;
    b = 0x9fb498b3 + m[0];
    a = ((b << 5) | (b >> 27)) + m[1] + 0x66b0cd0d;
    // TODO: Initial step unrolling can be continued...
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,0);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,48);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,m[16]);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,m[17]);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,96);
    F1(f,b,c,d);
    FF(a,b,c,d,e,f,k0,m[19]);

    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[20]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,192);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[22]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[23]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[24]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[25]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[26]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[27]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[28]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[29]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[30]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[31]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[32]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[33]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[34]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[35]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[36]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[37]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[38]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k1,m[39]);

    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[40]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[41]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[42]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[43]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[44]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[45]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[46]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[47]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[48]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[49]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[50]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[51]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[52]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[53]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[54]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[55]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[56]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[57]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[58]);
    F3(f,b,c,d);
    FF(a,b,c,d,e,f,k2,m[59]);

    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[60]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[61]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[62]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[63]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[64]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[65]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[66]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[67]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[68]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[69]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[70]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[71]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[72]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[73]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[74]);
    F2(f,b,c,d);
    FF(a,b,c,d,e,f,k3,m[75]);

    if((h4 + ((a << 30) | (a >> 2))) == hash.e) {
        m[76] = ROL(m[73] ^ m[68] ^ m[62] ^ m[60]);
        F2(f,b,c,d);
        FF(a,b,c,d,e,f,k3,m[76]);
        if((h3 + ((a << 30) | (a >> 2))) == hash.d) {
            m[77] = ROL(m[74] ^ m[69] ^ m[63] ^ m[61]);
            F2(f,b,c,d);
            FF(a,b,c,d,e,f,k3,m[77]);
            if((h2 + ((a << 30) | (a >> 2))) == hash.c) {
                m[78] = ROL(m[75] ^ m[70] ^ m[64] ^ m[62]);
                F2(f,b,c,d);
                FF(a,b,c,d,e,f,k3,m[78]);
                if((h1 + a) == hash.b) {
                    m[79] = ROL(m[76] ^ m[71] ^ m[65] ^ m[63]);
                    F2(f,b,c,d);
                    FF(a,b,c,d,e,f,k3,m[79]);
                    
                    if((h0 + a) == hash.a) {
                        result[0] = 'a'+l0;
                        result[1] = 'a'+l1;
                        result[2] = 'a'+l2;
                        result[3] = 'a'+l3;
                        result[4] = 'a'+l4;
                        result[5] = 'a'+l5;
                        /* Found */
                        return(EXIT_SUCCESS);
                    }
                }
            }
        }
    }

    }}}}}}
    /* Not found */
    return(EXIT_FAILURE);
}