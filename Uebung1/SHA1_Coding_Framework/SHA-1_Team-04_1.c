#include "sha1.h"
#include <stdio.h>
#include <emmintrin.h>  

#define ROL(m) ((m) << 1) | ((m) >> 31)
#define F1(f,b,c,d) f = d ^ (b & (c ^ d))
#define F2(f,b,c,d) f = b ^ c ^ d
#define F3(f,b,c,d) f = (b & c) | (d & (b | c))
#define FF(a,b,c,d,e,f,k,m) temp = ((a << 5) | (a >> 27)) + f + e + k + m; e = d; d = c; c = ((b << 30) | (b >> 2)); b = a; a = temp;
// SIMD Makros
#define OR(x,y) (_mm_or_si128(x,y))
#define XOR(x,y) (_mm_xor_si128(x,y))
#define AND(x,y) (_mm_and_si128(x,y))
#define ADD(x,y) (_mm_add_epi32(x,y))
#define SIMDROLX(m, x) OR(_mm_sll_epi32(m, x), _mm_srl_epi32(m, 32-x))
#define SIMDROL(m) SIMDROLX(m, 1)
#define SIMDF1(f,b,c,d) f = XOR(d, AND(b, XOR(c,d)))
#define SIMDF2(f,b,c,d) f = XOR(b, XOR(c,d))
#define SIMDF3(f,b,c,d) f = OR(AND(b,c), AND(d, OR(b,c)))
#define SIMDFF(a,b,c,d,e,f,k,m) temp = ADD(ADD(ADD(ADD(SIMDROLX(a, 5), f), e), k), m); e = d; d = c; c = SIMDROLX(b, 30); b = a; a = temp;
// Returns a 128bit vector: va1|va1|va1|va1
#define SET1INT(va1) (_mm_set1_epi32(va1))
// Returns a 128bit vector: va1|va2|va3|va4
#define SET4INT(va1, va2, va3, va4) (_mm_set_epi32(va1, va2, va3, va4))

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
 * gcc -O2 -Wall -fomit-frame-pointer -msse2 -masm=intel testbench.c SHA-1_Team-04_1.c -o crackSHA1
 * 
 * ANY FURTHER INFORMATION CAN BE FOUND IN THE
 * PROBLEM DESCRIPTION
 * 
 * PROGRAMMAUFRUF MIT 
 * ./crackSHA1 3192ca69cc3b03f7e89df6a21ba69db6b980d7a6
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
    
    
    uint32_t a, b, c, d, e, f, temp;
    
    uint32_t m[80],pm[80],w[25];
    // Startwerte setzen: m=aaaaaa, Hänge '1' an -> 10000000b = 0x80, 6 Buchstaben = 48 Bit, restliche Werte sind 0
    m[0] = 0x61616161;
    m[1] = 0x61618000;
    m[15] = 48;
    
    unsigned l5,l4,l3,l2,l1,l0;

    // Iterationen, die m[1] betreffen
    for(l5=0; l5<26; l5++) {
        m[1] = (m[1]& ~(0xff<<16))+((l5+'a')<<16);
        for(l4=0; l4<26; l4++) {
            m[1] = (m[1]& ~(0xff<<24))+((l4+'a')<<24);

            // Jens Steube Message Expansion
            // Bekannte Werte ersetzt und vorberechnet (zero based und initial step)
            m[17] = ROL(m[1]);
            m[20] = ROL(m[17]);
            m[23] = ROL(m[20] ^ 48);
            pm[25] = m[20];
            m[26] = ROL(m[23] ^ 96);
            pm[28] = ROL(pm[25] ^ m[20]);
            m[29] = ROL(m[26] ^ 240);
            pm[31] = ROL(pm[28] ^ m[23] ^ m[17] ^ 48);
            pm[32] = ROL(m[29] ^ 480);
            m[33] = ROL(1536 ^ pm[25] ^ m[17]);
            pm[34] = ROL(pm[31] ^ m[26] ^ m[20] ^ 96);
            pm[35] = ROL(pm[32] ^ 960);
            pm[36] = ROL(m[33] ^ pm[28] ^ m[20]);
            pm[37] = ROL(pm[34] ^ m[29] ^ m[23] ^ 192);
            pm[38] = ROL(pm[35] ^ 1920);
            m[39] = ROL(pm[36] ^ pm[31] ^ pm[25] ^ m[23]);
            pm[40] = ROL(pm[37] ^ pm[32] ^ m[26] ^ 384);
            m[41] = ROL(pm[38] ^ m[33] ^ 768 ^ pm[25]);
            pm[42] = ROL(m[39] ^ pm[34] ^ pm[28] ^ m[26]);
            pm[43] = ROL(pm[40] ^ pm[35] ^ m[29] ^ 768);
            pm[44] = ROL(m[41] ^ pm[36] ^ 1536 ^ pm[28]);
            m[45] = ROL(pm[42] ^ pm[37] ^ pm[31] ^ m[29]);
            pm[46] = ROL(pm[43] ^ pm[38] ^ pm[32] ^ 1536);
            pm[47] = ROL(pm[44] ^ m[39] ^ m[33] ^ pm[31]);
            pm[48] = ROL(m[45] ^ pm[40] ^ pm[34] ^ pm[32]);
            pm[49] = ROL(pm[46] ^ m[41] ^ pm[35] ^ m[33]);
            pm[50] = ROL(pm[47] ^ pm[42] ^ pm[36] ^ pm[34]);
            pm[51] = ROL(pm[48] ^ pm[43] ^ pm[37] ^ pm[35]);
            pm[52] = ROL(pm[49] ^ pm[44] ^ pm[38] ^ pm[36]);
            m[53] = ROL(pm[50] ^ m[45] ^ m[39] ^ pm[37]);
            pm[54] = ROL(pm[51] ^ pm[46] ^ pm[40] ^ pm[38]);
            pm[55] = ROL(pm[52] ^ pm[47] ^ m[41] ^ m[39]);
            pm[56] = ROL(m[53] ^ pm[48] ^ pm[42] ^ pm[40]);
            pm[57] = ROL(pm[54] ^ pm[49] ^ pm[43] ^ m[41]);
            pm[58] = ROL(pm[55] ^ pm[50] ^ pm[44] ^ pm[42]);
            pm[59] = ROL(pm[56] ^ pm[51] ^ m[45] ^ pm[43]);
            pm[60] = ROL(pm[57] ^ pm[52] ^ pm[46] ^ pm[44]);
            pm[61] = ROL(pm[58] ^ m[53] ^ pm[47] ^ m[45]);
            pm[62] = ROL(pm[59] ^ pm[54] ^ pm[48] ^ pm[46]);
            pm[63] = ROL(pm[60] ^ pm[55] ^ pm[49] ^ pm[47]);
            pm[64] = ROL(pm[61] ^ pm[56] ^ pm[50] ^ pm[48]);
            m[65] = ROL(pm[62] ^ pm[57] ^ pm[51] ^ pm[49]);
            pm[66] = ROL(pm[63] ^ pm[58] ^ pm[52] ^ pm[50]);
            pm[67] = ROL(pm[64] ^ pm[59] ^ m[53] ^ pm[51]);
            pm[68] = ROL(m[65] ^ pm[60] ^ pm[54] ^ pm[52]);
            m[69] = ROL(pm[66] ^ pm[61] ^ pm[55] ^ m[53]);
            pm[70] = ROL(pm[67] ^ pm[62] ^ pm[56] ^ pm[54]);
            pm[71] = ROL(pm[68] ^ pm[63] ^ pm[57] ^ pm[55]);
            pm[72] = ROL(m[69] ^ pm[64] ^ pm[58] ^ pm[56]);
            pm[73] = ROL(pm[70] ^ m[65] ^ pm[59] ^ pm[57]);
            pm[74] = ROL(pm[71] ^ pm[66] ^ pm[60] ^ pm[58]);
            pm[75] = ROL(pm[72] ^ pm[67] ^ pm[61] ^ pm[59]);

            // Iterationen, die m[0] betreffen
            for(l3=0; l3<26; l3++) {
                m[0] = (m[0]& ~(0xff<<0))+((l3+'a')<<0);
                for(l2=0; l2<26; l2++) {
                    m[0] = (m[0]& ~(0xff<<8))+((l2+'a')<<8);
                    for(l1=0; l1<26; l1++) {
                        m[0] = (m[0]& ~(0xff<<16))+((l1+'a')<<16);
                        for(l0=0; l0<26; l0++) {
                            m[0] = (m[0]& ~(0xff<<24))+((l0+'a')<<24);

    // Berechnung m[0] 1-20 mal rotiert
    w[1] = ROL(m[0]);
    w[2] = ROL(w[1]);
    w[3] = ROL(w[2]);
    w[4] = ROL(w[3]);
    w[5] = ROL(w[4]);
    w[6] = ROL(w[5]);
    w[7] = ROL(w[6]);
    w[8] = ROL(w[7]);
    w[9] = ROL(w[8]);
    w[10] = ROL(w[9]);
    w[11] = ROL(w[10]);
    w[12] = ROL(w[11]);
    w[13] = ROL(w[12]);
    w[14] = ROL(w[13]);
    w[15] = ROL(w[14]);
    w[16] = ROL(w[15]);
    w[17] = ROL(w[16]);
    w[18] = ROL(w[17]);
    w[19] = ROL(w[18]);
    w[20] = ROL(w[19]);
    // Vorberechnung mehrfach verwendeter Kombinationen: 6__4, 8__4, 8__12, 6__4__7
    w[21] = w[6] ^ w[4];
    w[22] = w[8] ^ w[4];
    w[23] = w[8] ^ w[12];
    w[24] = w[6] ^ w[4] ^ w[7];
    
    m[16] = w[1];
    m[19] = w[2];
    m[22] = w[3];
    m[24] = 384 ^ w[2];
    m[25] = pm[25] ^ w[4];
    m[28] = pm[28] ^ w[5];
    m[30] = 1536 ^ w[4] ^ w[2];
    m[31] = pm[31] ^ w[6];
    m[32] = pm[32] ^ w[3] ^ w[2];
    m[34] = pm[34] ^ w[7];
    m[35] = pm[35] ^ w[4];
    m[36] = pm[36] ^ w[21];
    m[37] = pm[37] ^ w[8];
    m[38] = pm[38] ^ w[4];
    m[40] = pm[40] ^ w[4] ^ w[9];
    m[42] = pm[42] ^ w[6] ^ w[8];
    m[43] = pm[43] ^ w[10];
    m[44] = pm[44] ^ w[6] ^ w[3] ^ w[7];
    m[46] = pm[46] ^ w[4] ^ w[11];
    m[47] = pm[47] ^ w[22];
    m[48] = pm[48] ^ w[22] ^ w[3] ^ w[10] ^ w[5];
    m[49] = pm[49] ^ w[12];
    m[50] = pm[50] ^ w[8];
    m[51] = pm[51] ^ w[21];
    m[52] = pm[52] ^ w[22] ^ w[13];
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
    m[66] = pm[66] ^ w[14] ^ w[16];
    m[67] = pm[67] ^ w[8] ^ w[18];
    m[68] = pm[68] ^ w[11] ^ w[14] ^ w[15];
    m[70] = pm[70] ^ w[12] ^ w[19];
    m[71] = pm[71] ^ w[12] ^ w[16];
    m[72] = pm[72] ^ w[11] ^ w[12] ^ w[18] ^ w[13] ^ w[16] ^ w[5];
    m[73] = pm[73] ^ w[20];
    m[74] = pm[74] ^ w[8] ^ w[16];
    m[75] = pm[75] ^ w[6] ^ w[12] ^ w[14];

    /*
    Unroll der Funktionen mit bekannten Werten schrittweise (zero based und initial step):
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
    // Weitere Vorberechnungen sind nicht sinnvoll, da Rechenkomplexitaet damit nicht abnimmt (m[0] bzw m[1] sind nun in a und b und fliessen dadurch in weitere Variablen ein)
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
    FF(a,b,c,d,e,f,k1,768);
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

    // Early Exit A-->B-(<<)->C-->D-->E
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