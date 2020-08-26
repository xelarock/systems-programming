#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[] ){
    char c;
    signed char sc;
    unsigned char uc;
    char *cp;
    
    short s;
    signed short ss;
    unsigned short us;
    short *sp;
    
    int i;
    signed int si;
    unsigned int ui;
    int *ip;
    
    long l;
    signed long sl;
    unsigned long ul;
    long *lp;
    
    long long ll;
    signed long long sll;
    unsigned long long ull;
    long long *llp;
    
    int8_t i8;
    uint8_t ui8;
    int16_t i16;
    uint16_t ui16;
    int32_t i32;
    uint32_t ui32;
    int64_t i64;
    uint64_t ui64;
    
    float f;
    double d;
    long double ld;
    float *fp;
    double *dp;
    long double *ldp;
    
    void *vp;
    
    printf("Characters:\n");
    printf("\tsizeof(char): %lu. sizeof(c): %lu.\n", sizeof(char), sizeof(c) );
    printf("\tsizeof(signed char): %lu. sizeof(sc): %lu.\n", sizeof(signed char), sizeof(sc) );
    printf("\tsizeof(unsigned char): %lu. sizeof(uc): %lu.\n", sizeof(unsigned char), sizeof(uc) );

    printf("Shorts:\n");
    printf("\tsizeof(short): %lu. sizeof(s): %lu.\n", sizeof(short), sizeof(s) );
    printf("\tsizeof(signed short): %lu. sizeof(ss): %lu.\n", sizeof(signed short), sizeof(ss) );
    printf("\tsizeof(unsigned short): %lu. sizeof(us): %lu.\n\n", sizeof(unsigned short), sizeof(us) );

    printf("Integers:\n");
    printf("\tsizeof(int): %lu. sizeof(i): %lu.\n", sizeof(int), sizeof(i) );
    printf("\tsizeof(signed int): %lu. sizeof(si): %lu.\n", sizeof(signed int), sizeof(si) );
    printf("\tsizeof(unsigned int): %lu. sizeof(ui): %lu.\n\n", sizeof(unsigned int), sizeof(ui) );

    printf("Longs:\n");
    printf("\tsizeof(long): %lu. sizeof(l): %lu.\n", sizeof(long), sizeof(l) );
    printf("\tsizeof(signed long): %lu. sizeof(sl): %lu.\n", sizeof(signed long), sizeof(sl) );
    printf("\tsizeof(unsigned long): %lu. sizeof(ul): %lu.\n\n", sizeof(unsigned long), sizeof(ul) );
    
    printf("Long longs:\n");
    printf("\tsizeof(long long): %lu. sizeof(ll): %lu.\n", sizeof(long long), sizeof(ll) );
    printf("\tsizeof(signed long long): %lu. sizeof(sll): %lu.\n", sizeof(signed long long), sizeof(sll) );
    printf("\tsizeof(unsigned long long): %lu. sizeof(ull): %lu.\n\n", sizeof(unsigned long long), sizeof(ull) );
    
    printf("Exact Width Integers\n");
    printf("\tsizeof(int8_t): %lu. sizeof(i8): %lu.\n", sizeof(int8_t), sizeof(i8) );
    printf("\tsizeof(uint8_t): %lu. sizeof(ui8): %lu.\n", sizeof(uint8_t), sizeof(ui8) );
    printf("\tsizeof(int16_t): %lu. sizeof(i16): %lu.\n", sizeof(int16_t), sizeof(i16) );
    printf("\tsizeof(uint16_t): %lu. sizeof(ui16): %lu.\n", sizeof(uint16_t), sizeof(ui16) );
    printf("\tsizeof(int32_t): %lu. sizeof(i32): %lu.\n", sizeof(int32_t), sizeof(i32) );
    printf("\tsizeof(uint32_t): %lu. sizeof(ui32): %lu.\n", sizeof(uint32_t), sizeof(ui32) );
    printf("\tsizeof(int64_t): %lu. sizeof(i64): %lu.\n", sizeof(int64_t), sizeof(i64) );
    printf("\tsizeof(uint64_t): %lu. sizeof(ui64): %lu.\n\n", sizeof(uint64_t), sizeof(ui64) );
    
    printf("Floats:\n");
    printf("\tsizeof(float): %lu. sizeof(f): %lu.\n", sizeof(float), sizeof(f) );
    printf("\tsizeof(double): %lu. sizeof(d): %lu.\n", sizeof(double), sizeof(d) );
    printf("\tsizeof(long double): %lu. sizeof(ld): %lu.\n\n", sizeof( long double), sizeof(ld) );
    
    printf("Pointers:\n");
    printf("\tsizeof(char *): %lu. sizeof(cp): %lu.\n", sizeof(char *), sizeof(cp) );
    printf("\tsizeof(short *): %lu. sizeof(sp): %lu.\n", sizeof(short *), sizeof(sp) );
    printf("\tsizeof(int *): %lu. sizeof(ip): %lu.\n", sizeof(int *), sizeof(ip) );
    printf("\tsizeof(long *): %lu. sizeof(lp): %lu.\n", sizeof(long *), sizeof(lp) );
    printf("\tsizeof(long long *): %lu. sizeof(llp): %lu.\n", sizeof(long long *), sizeof(llp) );
    printf("\tsizeof(float *): %lu. sizeof(fp): %lu.\n", sizeof(float *), sizeof(fp) );
    printf("\tsizeof(double *): %lu. sizeof(dp): %lu.\n", sizeof(double *), sizeof(dp) );
    printf("\tsizeof(long double *): %lu. sizeof(ldp): %lu.\n", sizeof(long double *), sizeof(ldp) );
    printf("\tsizeof(void *): %lu. sizeof(vp): %lu\n\n", sizeof(void *), sizeof(vp) );
    
    return 0;
}


