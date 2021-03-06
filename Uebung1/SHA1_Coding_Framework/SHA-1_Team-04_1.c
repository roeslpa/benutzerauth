#include "sha1.h"
#include <stdio.h>
#include <emmintrin.h>  

#define ROL(m) ((m) << 1) | ((m) >> 31)
#define F1(f,b,c,d) f = d ^ (b & (c ^ d))
#define F2(f,b,c,d) f = b ^ c ^ d
#define F3(f,b,c,d) f = (b & c) | (d & (b | c))
#define FF(a,b,c,d,e,f,k,m) temp = ((a << 5) | (a >> 27)) + f + e + k + m; e = d; d = c; c = ((b << 30) | (b >> 2)); b = a; a = temp;
// SIMD Makros
// Returns a 128bit vector: va1|va1|va1|va1
#define SET1INT(va1) (_mm_set1_epi32(va1))
// Returns a 128bit vector: va1|va2|va3|va4
#define SET4INT(va1, va2, va3, va4) (_mm_set_epi32(va1, va2, va3, va4))

#define OR(x,y) (_mm_or_si128(x,y))
#define XOR(x,y) (_mm_xor_si128(x,y))
#define AND(x,y) (_mm_and_si128(x,y))
#define ADD(x,y) (_mm_add_epi32(x,y))
#define SIMDROLX(m, x) OR(_mm_sll_epi32(m, SET1INT(x)), _mm_srl_epi32(m, SET1INT(32-x)))
#define SIMDROL(m) SIMDROLX(m, 1)
#define SIMDF1(f,b,c,d) f = XOR(d, AND(b, XOR(c,d)))
#define SIMDF2(f,b,c,d) f = XOR(b, XOR(c,d))
#define SIMDF3(f,b,c,d) f = OR(AND(b,c), AND(d, OR(b,c)))
#define SIMDFF(a,b,c,d,e,f,k,m) temp = ADD(ADD(ADD(ADD(SIMDROLX(a, 5), f), e), k), m); e = d; d = c; c = SIMDROLX(b, 30); b = a; a = temp;

typedef union
{
    __m128i v;
    uint32_t a[4];
} vectors;

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
 *
 * Team 04: Philipp Markert, Paul Rösler
 *
 */

int crackHash(struct state hash, char *result) {
    //initialisiere Konstanten
    const __m128i h0 = SET1INT(0x67452301);
    const __m128i h1 = SET1INT(0xEFCDAB89);
    const __m128i h2 = SET1INT(0x98BADCFE);
    const __m128i h3 = SET1INT(0x10325476);
    const __m128i h4 = SET1INT(0xC3D2E1F0);
    
    const __m128i k0 = SET1INT(0x5A827999);
    const __m128i k1 = SET1INT(0x6ED9EBA1);
    const __m128i k2 = SET1INT(0x8F1BBCDC);
    const __m128i k3 = SET1INT(0xCA62C1D6);
    
    
    uint32_t m00, m10, m20, m30;
    
    uint32_t m[80],pm[80];
    __m128i m4[80],w4[25];
    __m128i a, b, c, d, e, f, temp;
    vectors test;
    // Startwerte setzen: m=aaaaaa, Hänge '1' an -> 10000000b = 0x80, 6 Buchstaben = 48 Bit, restliche Werte sind 0
    m[0] = 0x61616161;
    m[1] = 0x61618000;
    m[15] = 48;
    
    unsigned l5,l4,l3,l2,l1,l0,offset;

    offset = 0;

    // Iterationen, die m[1] betreffen
    for(l5=0; l5<26; l5++) {
        m[1] = (m[1]& ~(0xff<<16))+((l5+'a')<<16);
        for(l4=0; l4<26; l4++) {
            m[1] = (m[1]& ~(0xff<<24))+((l4+'a')<<24);

            // Jens Steube Message Expansion
            // Bekannte Werte ersetzt und vorberechnet (zero based und initial step)
            m[17] = ROL(m[1]);
            m4[17] = SET1INT(m[17]);
            m[20] = ROL(m[17]);
            m4[20] = SET1INT(m[20]);
            m[23] = ROL(m[20] ^ 48);
            m4[23] = SET1INT(m[23]);
            pm[25] = m[20];
            m[26] = ROL(m[23] ^ 96);
            m4[26] = SET1INT(m[26]);
            pm[28] = ROL(pm[25] ^ m[20]);
            m[29] = ROL(m[26] ^ 240);
            m4[29] = SET1INT(m[29]);
            pm[31] = ROL(pm[28] ^ m[23] ^ m[17] ^ 48);
            pm[32] = ROL(m[29] ^ 480);
            m[33] = ROL(1536 ^ pm[25] ^ m[17]);
            m4[33] = SET1INT(m[33]);
            pm[34] = ROL(pm[31] ^ m[26] ^ m[20] ^ 96);
            pm[35] = ROL(pm[32] ^ 960);
            pm[36] = ROL(m[33] ^ pm[28] ^ m[20]);
            pm[37] = ROL(pm[34] ^ m[29] ^ m[23] ^ 192);
            pm[38] = ROL(pm[35] ^ 1920);
            m[39] = ROL(pm[36] ^ pm[31] ^ pm[25] ^ m[23]);
            m4[39] = SET1INT(m[39]);
            pm[40] = ROL(pm[37] ^ pm[32] ^ m[26] ^ 384);
            m[41] = ROL(pm[38] ^ m[33] ^ 768 ^ pm[25]);
            m4[41] = SET1INT(m[41]);
            pm[42] = ROL(m[39] ^ pm[34] ^ pm[28] ^ m[26]);
            pm[43] = ROL(pm[40] ^ pm[35] ^ m[29] ^ 768);
            pm[44] = ROL(m[41] ^ pm[36] ^ 1536 ^ pm[28]);
            m[45] = ROL(pm[42] ^ pm[37] ^ pm[31] ^ m[29]);
            m4[45] = SET1INT(m[45]);
            pm[46] = ROL(pm[43] ^ pm[38] ^ pm[32] ^ 1536);
            pm[47] = ROL(pm[44] ^ m[39] ^ m[33] ^ pm[31]);
            pm[48] = ROL(m[45] ^ pm[40] ^ pm[34] ^ pm[32]);
            pm[49] = ROL(pm[46] ^ m[41] ^ pm[35] ^ m[33]);
            pm[50] = ROL(pm[47] ^ pm[42] ^ pm[36] ^ pm[34]);
            pm[51] = ROL(pm[48] ^ pm[43] ^ pm[37] ^ pm[35]);
            pm[52] = ROL(pm[49] ^ pm[44] ^ pm[38] ^ pm[36]);
            m[53] = ROL(pm[50] ^ m[45] ^ m[39] ^ pm[37]);
            m4[53] = SET1INT(m[53]);
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
            m4[65] = SET1INT(m[65]);
            pm[66] = ROL(pm[63] ^ pm[58] ^ pm[52] ^ pm[50]);
            pm[67] = ROL(pm[64] ^ pm[59] ^ m[53] ^ pm[51]);
            pm[68] = ROL(m[65] ^ pm[60] ^ pm[54] ^ pm[52]);
            m[69] = ROL(pm[66] ^ pm[61] ^ pm[55] ^ m[53]);
            m4[69] = SET1INT(m[69]);
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
                        for(l0=offset; l0<26; l0=l0+4) {
                            m[0] = (m[0]& ~(0xff<<24))+((l0+'a')<<24);
                            //Falls diese Runde noch voll wird (l0=22 danach +4=26) erstes, sonst schon für nächste Runde vorberechnen
                            if(l0!=24) {
                                m00 = (m[0]& ~(0xff<<24))+((l0+'a')<<24);
                                m10 = (m[0]& ~(0xff<<24))+((l0+'b')<<24);
                                m20 = (m[0]& ~(0xff<<24))+((l0+'c')<<24);
                                m30 = (m[0]& ~(0xff<<24))+((l0+'d')<<24);
                                offset = 0;
                            } else {
                                m00 = (m[0]& ~(0xff<<24))+(('y')<<24);
                                m10 = (m[0]& ~(0xff<<24))+(('z')<<24);
                                //Schon fuer naechste "Alphabet" um vorherige 26 zu 28 auzufuellen
                                m[0] = (m[0]& ~(0xff<<16))+((l1+1+'a')<<16);
                                m20 = (m[0]& ~(0xff<<24))+(('a')<<24);
                                m30 = (m[0]& ~(0xff<<24))+(('b')<<24);
                                offset = 2;
                            }

    // SIMD Berechnung m[0] 1-20 mal rotiert
    m4[0] = SET4INT(m30, m20, m10, m00);
    m4[1] = SET1INT(m[1]);
    w4[1] = SIMDROL(m4[0]);
    w4[2] = SIMDROL(w4[1]);
    w4[3] = SIMDROL(w4[2]);
    w4[4] = SIMDROL(w4[3]);
    w4[5] = SIMDROL(w4[4]);
    w4[6] = SIMDROL(w4[5]);
    w4[7] = SIMDROL(w4[6]);
    w4[8] = SIMDROL(w4[7]);
    w4[9] = SIMDROL(w4[8]);
    w4[10] = SIMDROL(w4[9]);
    w4[11] = SIMDROL(w4[10]);
    w4[12] = SIMDROL(w4[11]);
    w4[13] = SIMDROL(w4[12]);
    w4[14] = SIMDROL(w4[13]);
    w4[15] = SIMDROL(w4[14]);
    w4[16] = SIMDROL(w4[15]);
    w4[17] = SIMDROL(w4[16]);
    w4[18] = SIMDROL(w4[17]);
    w4[19] = SIMDROL(w4[18]);
    w4[20] = SIMDROL(w4[19]);
    // Vorberechnung mehrfach verwendeter Kombinationen: 6__4, 8__4, 8__12, 6__4__7
    w4[21] = XOR(w4[6],w4[4]);
    w4[22] = XOR(w4[8],w4[4]);
    w4[23] = XOR(w4[8],w4[12]);
    w4[24] = XOR(XOR(w4[6],w4[4]),w4[7]);
    
    m4[16] = w4[1];
    m4[19] = w4[2];
    m4[22] = w4[3];
    m4[24] = XOR(SET1INT(384), w4[2]);
    m4[25] = XOR(SET1INT(pm[25]), w4[4]);
    m4[28] = XOR(SET1INT(pm[28]), w4[5]);
    m4[30] = XOR(XOR(SET1INT(1536), w4[4]), w4[2]);
    m4[31] = XOR(SET1INT(pm[31]), w4[6]);
    m4[32] = XOR(XOR(SET1INT(pm[32]), w4[3]), w4[2]);
    m4[34] = XOR(SET1INT(pm[34]), w4[7]);
    m4[35] = XOR(SET1INT(pm[35]), w4[4]);
    m4[36] = XOR(SET1INT(pm[36]), w4[21]);
    m4[37] = XOR(SET1INT(pm[37]), w4[8]);
    m4[38] = XOR(SET1INT(pm[38]), w4[4]);
    m4[40] = XOR(XOR(SET1INT(pm[40]), w4[4]), w4[9]);
    m4[42] = XOR(XOR(SET1INT(pm[42]), w4[6]), w4[8]);
    m4[43] = XOR(SET1INT(pm[43]), w4[10]);
    m4[44] = XOR(XOR(XOR(SET1INT(pm[44]), w4[6]), w4[3]), w4[7]);
    m4[46] = XOR(XOR(SET1INT(pm[46]), w4[4]), w4[11]);
    m4[47] = XOR(SET1INT(pm[47]), w4[22]);
    m4[48] = XOR(XOR(XOR(XOR(SET1INT(pm[48]), w4[22]), w4[3]), w4[10]), w4[5]);
    m4[49] = XOR(SET1INT(pm[49]), w4[12]);
    m4[50] = XOR(SET1INT(pm[50]), w4[8]);
    m4[51] = XOR(SET1INT(pm[51]), w4[21]);
    m4[52] = XOR(XOR(SET1INT(pm[52]), w4[22]), w4[13]);
    m4[54] = XOR(XOR(XOR(SET1INT(pm[54]), w4[7]), w4[10]), w4[12]);
    m4[55] = XOR(SET1INT(pm[55]), w4[14]);
    m4[56] = XOR(XOR(XOR(SET1INT(pm[56]), w4[24]), w4[11]), w4[10]);
    m4[57] = XOR(SET1INT(pm[57]), w4[8]);
    m4[58] = XOR(XOR(SET1INT(pm[58]), w4[22]), w4[15]);
    m4[59] = XOR(SET1INT(pm[59]), w4[23]);
    m4[60] = XOR(XOR(XOR(XOR(SET1INT(pm[60]), w4[23]), w4[4]), w4[7]), w4[14]);
    m4[61] = XOR(SET1INT(pm[61]), w4[16]);
    m4[62] = XOR(XOR(SET1INT(pm[62]), w4[21]), w4[23]);
    m4[63] = XOR(SET1INT(pm[63]), w4[8]);
    m4[64] = XOR(XOR(XOR(SET1INT(pm[64]), w4[24]), w4[23]), w4[17]);
    m4[66] = XOR(XOR(SET1INT(pm[66]), w4[14]), w4[16]);
    m4[67] = XOR(XOR(SET1INT(pm[67]), w4[8]), w4[18]);
    m4[68] = XOR(XOR(XOR(SET1INT(pm[68]), w4[11]), w4[14]), w4[15]);
    m4[70] = XOR(XOR(SET1INT(pm[70]), w4[12]), w4[19]);
    m4[71] = XOR(XOR(SET1INT(pm[71]), w4[12]), w4[16]);
    m4[72] = XOR(XOR(XOR(XOR(XOR(XOR(SET1INT(pm[72]), w4[11]), w4[12]), w4[18]), w4[13]), w4[16]), w4[5]);
    m4[73] = XOR(SET1INT(pm[73]), w4[20]);
    m4[74] = XOR(XOR(SET1INT(pm[74]), w4[8]), w4[16]);
    m4[75] = XOR(XOR(XOR(SET1INT(pm[75]), w4[6]), w4[12]), w4[14]);

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
    c = SET1INT(0x59d148c0);
    d = SET1INT(0x7bf36ae2);
    b = ADD(SET1INT(0x9fb498b3), m4[0]);
    a = ADD(ADD(SIMDROLX(b,5), m4[1]), SET1INT(0x66b0cd0d));
    // Weitere Vorberechnungen sind nicht sinnvoll, da Rechenkomplexitaet damit nicht abnimmt (m[0] bzw m[1] sind nun in a und b und fliessen dadurch in weitere Variablen ein)
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(0));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(48));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,m4[16]);
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,m4[17]);
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,SET1INT(96));
    SIMDF1(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k0,m4[19]);

    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[20]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,SET1INT(192));
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[22]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[23]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[24]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[25]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[26]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,SET1INT(768));
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[28]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[29]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[30]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[31]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[32]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[33]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[34]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[35]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[36]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[37]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[38]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k1,m4[39]);

    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[40]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[41]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[42]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[43]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[44]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[45]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[46]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[47]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[48]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[49]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[50]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[51]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[52]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[53]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[54]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[55]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[56]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[57]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[58]);
    SIMDF3(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k2,m4[59]);

    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[60]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[61]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[62]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[63]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[64]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[65]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[66]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[67]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[68]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[69]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[70]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[71]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[72]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[73]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[74]);
    SIMDF2(f,b,c,d);
    SIMDFF(a,b,c,d,e,f,k3,m4[75]);
    
    // Early Exit A-->B-(<<)->C-->D-->E
    test.v = ADD(h4,SIMDROLX(a,30));

    if(test.a[0] == hash.e || test.a[1] == hash.e || test.a[2] == hash.e || test.a[3] == hash.e) {
    //if((h4 + ((a << 30) | (a >> 2))) == hash.e) {
        m4[76] = SIMDROL(XOR(XOR(XOR(m4[73], m4[68]), m4[62]), m4[60]));
        SIMDF2(f,b,c,d);
        SIMDFF(a,b,c,d,e,f,k3,m4[76]);

        test.v = ADD(h3,SIMDROLX(a,30));
        if(test.a[0] == hash.d || test.a[1] == hash.d || test.a[2] == hash.d || test.a[3] == hash.d) {
        //if((h3 + ((a << 30) | (a >> 2))) == hash.d) {
            m4[77] = SIMDROL(XOR(XOR(XOR(m4[74], m4[69]), m4[63]), m4[61]));
            SIMDF2(f,b,c,d);
            SIMDFF(a,b,c,d,e,f,k3,m4[77]);
            
            test.v = ADD(h2,SIMDROLX(a,30));
            if(test.a[0] == hash.c || test.a[1] == hash.c || test.a[2] == hash.c || test.a[3] == hash.c) {
            //if((h2 + ((a << 30) | (a >> 2))) == hash.c) {
                m4[78] = SIMDROL(XOR(XOR(XOR(m4[75], m4[70]), m4[64]), m4[62]));
                SIMDF2(f,b,c,d);
                SIMDFF(a,b,c,d,e,f,k3,m4[78]);
                
                test.v = ADD(h1,a);
                if(test.a[0] == hash.b || test.a[1] == hash.b || test.a[2] == hash.b || test.a[3] == hash.b) {
                //if((h1 + a) == hash.b) {
                    m4[79] = SIMDROL(XOR(XOR(XOR(m4[76], m4[71]), m4[65]), m4[63]));
                    SIMDF2(f,b,c,d);
                    SIMDFF(a,b,c,d,e,f,k3,m4[79]);
                    
                    test.v = ADD(h0,a);
                    if(test.a[0] == hash.a) {
                        //Abhaengig davon, ob a und b des naechsten mit berechnet wurden, das ergebnis "ausgeben"
                        if(offset == 0) {
                            result[0] = 'a'+l0;
                            result[1] = 'a'+l1;
                        } else {
                            result[0] = 'y';
                            result[1] = 'a'+l1;
                        }
                        
                        result[2] = 'a'+l2;
                        result[3] = 'a'+l3;
                        result[4] = 'a'+l4;
                        result[5] = 'a'+l5;
                        return(EXIT_SUCCESS);
                    } else if(test.a[1] == hash.a) {
                        if(offset == 0) {
                            result[0] = 'b'+l0;
                            result[1] = 'a'+l1;
                        } else {
                            result[0] = 'z';
                            result[1] = 'a'+l1;
                        }
                        result[2] = 'a'+l2;
                        result[3] = 'a'+l3;
                        result[4] = 'a'+l4;
                        result[5] = 'a'+l5;
                        return(EXIT_SUCCESS);
                    } else if(test.a[2] == hash.a) {
                        if(offset == 0) {
                            result[0] = 'c'+l0;
                            result[1] = 'a'+l1;
                        } else {
                            result[0] = 'a';
                            result[1] = 'a'+l1+1;
                        }
                        result[1] = 'a'+l1;
                        result[2] = 'a'+l2;
                        result[3] = 'a'+l3;
                        result[4] = 'a'+l4;
                        result[5] = 'a'+l5;
                        return(EXIT_SUCCESS);
                    } else if(test.a[3] == hash.a) {
                    //if((h0 + a) == hash.a) {
                        if(offset == 0) {
                            result[0] = 'd'+l0;
                            result[1] = 'a'+l1;
                        } else {
                            result[0] = 'b';
                            result[1] = 'a'+l1+1;
                        }
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