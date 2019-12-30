
typedef unsigned int  uint32_t;
typedef unsigned long uint64_t;    //注意：在C/C++中，64位需要两个long；在OpenCL中，64位为一个long。重要的是，这个位数影响计算结果。

#define SECP256K1_INLINE __inline

/* Unpacks a constant into a overlapping multi-limbed FE element. */
#define SECP256K1_FE_CONST_INNER(d7, d6, d5, d4, d3, d2, d1, d0) { \
    (d0) & 0x3FFFFFFUL, \
    (((uint32_t)d0) >> 26) | (((uint32_t)(d1) & 0xFFFFFUL) << 6), \
    (((uint32_t)d1) >> 20) | (((uint32_t)(d2) & 0x3FFFUL) << 12), \
    (((uint32_t)d2) >> 14) | (((uint32_t)(d3) & 0xFFUL) << 18), \
    (((uint32_t)d3) >> 8) | (((uint32_t)(d4) & 0x3UL) << 24), \
    (((uint32_t)d4) >> 2) & 0x3FFFFFFUL, \
    (((uint32_t)d4) >> 28) | (((uint32_t)(d5) & 0x3FFFFFUL) << 4), \
    (((uint32_t)d5) >> 22) | (((uint32_t)(d6) & 0xFFFFUL) << 10), \
    (((uint32_t)d6) >> 16) | (((uint32_t)(d7) & 0x3FFUL) << 16), \
    (((uint32_t)d7) >> 10) \
}
#define SECP256K1_FE_CONST(d7, d6, d5, d4, d3, d2, d1, d0) {SECP256K1_FE_CONST_INNER((d7), (d6), (d5), (d4), (d3), (d2), (d1), (d0))}
#define SECP256K1_GE_CONST(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) {SECP256K1_FE_CONST((a),(b),(c),(d),(e),(f),(g),(h)), SECP256K1_FE_CONST((i),(j),(k),(l),(m),(n),(o),(p)), 0}


typedef struct
{
    uint32_t n[10];
} secp256k1_fe;

typedef struct
{
    uint32_t n[8];
} secp256k1_fe_storage;

/** A group element of the secp256k1 curve, in jacobian coordinates. */
typedef struct
{
    secp256k1_fe x; /* actual X: x/z^2 */
    secp256k1_fe y; /* actual Y: y/z^3 */
    secp256k1_fe z;
    int infinity; /* whether this represents the point at infinity */
} secp256k1_gej;

typedef struct
{
    secp256k1_fe_storage x;
    secp256k1_fe_storage y;
} secp256k1_ge_storage;

/** A group element of the secp256k1 curve, in affine coordinates. */
typedef struct
{
    secp256k1_fe x;
    secp256k1_fe y;
    int infinity; /* whether this represents the point at infinity */
} secp256k1_ge;

//===================================================//

SECP256K1_INLINE static void secp256k1_fe_normalize_weak(secp256k1_fe *r)
{
    uint32_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4], t5 = r->n[5], t6 = r->n[6], t7 = r->n[7], t8 = r->n[8], t9 = r->n[9];

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    uint32_t x = t9 >> 22; t9 &= 0x3FFFFFUL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x3D1UL; t1 += (x << 6);
    t1 += (t0 >> 26); t0 &= 0x3FFFFFFUL;
    t2 += (t1 >> 26); t1 &= 0x3FFFFFFUL;
    t3 += (t2 >> 26); t2 &= 0x3FFFFFFUL;
    t4 += (t3 >> 26); t3 &= 0x3FFFFFFUL;
    t5 += (t4 >> 26); t4 &= 0x3FFFFFFUL;
    t6 += (t5 >> 26); t5 &= 0x3FFFFFFUL;
    t7 += (t6 >> 26); t6 &= 0x3FFFFFFUL;
    t8 += (t7 >> 26); t7 &= 0x3FFFFFFUL;
    t9 += (t8 >> 26); t8 &= 0x3FFFFFFUL;

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;
    r->n[5] = t5; r->n[6] = t6; r->n[7] = t7; r->n[8] = t8; r->n[9] = t9;
}

SECP256K1_INLINE static int secp256k1_fe_normalizes_to_zero(secp256k1_fe *r)
{
    uint32_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4], t5 = r->n[5], t6 = r->n[6], t7 = r->n[7], t8 = r->n[8], t9 = r->n[9];

    /* z0 tracks a possible raw value of 0, z1 tracks a possible raw value of P */
    uint32_t z0, z1;

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    uint32_t x = t9 >> 22; t9 &= 0x3FFFFFUL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x3D1UL; t1 += (x << 6);
    t1 += (t0 >> 26); t0 &= 0x3FFFFFFUL; z0  = t0; z1  = t0 ^ 0x3D0UL;
    t2 += (t1 >> 26); t1 &= 0x3FFFFFFUL; z0 |= t1; z1 &= t1 ^ 0x40UL;
    t3 += (t2 >> 26); t2 &= 0x3FFFFFFUL; z0 |= t2; z1 &= t2;
    t4 += (t3 >> 26); t3 &= 0x3FFFFFFUL; z0 |= t3; z1 &= t3;
    t5 += (t4 >> 26); t4 &= 0x3FFFFFFUL; z0 |= t4; z1 &= t4;
    t6 += (t5 >> 26); t5 &= 0x3FFFFFFUL; z0 |= t5; z1 &= t5;
    t7 += (t6 >> 26); t6 &= 0x3FFFFFFUL; z0 |= t6; z1 &= t6;
    t8 += (t7 >> 26); t7 &= 0x3FFFFFFUL; z0 |= t7; z1 &= t7;
    t9 += (t8 >> 26); t8 &= 0x3FFFFFFUL; z0 |= t8; z1 &= t8;
    z0 |= t9; z1 &= t9 ^ 0x3C00000UL;

    return (z0 == 0) | (z1 == 0x3FFFFFFUL);
}

SECP256K1_INLINE static void secp256k1_fe_negate(secp256k1_fe *r, const secp256k1_fe *a, int m)
{
    r->n[0] = 0x3FFFC2FUL * 2 * (m + 1) - a->n[0];
    r->n[1] = 0x3FFFFBFUL * 2 * (m + 1) - a->n[1];
    r->n[2] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[2];
    r->n[3] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[3];
    r->n[4] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[4];
    r->n[5] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[5];
    r->n[6] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[6];
    r->n[7] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[7];
    r->n[8] = 0x3FFFFFFUL * 2 * (m + 1) - a->n[8];
    r->n[9] = 0x03FFFFFUL * 2 * (m + 1) - a->n[9];
}

SECP256K1_INLINE static void secp256k1_fe_mul_int(secp256k1_fe *r, int a)
{
    r->n[0] *= a;
    r->n[1] *= a;
    r->n[2] *= a;
    r->n[3] *= a;
    r->n[4] *= a;
    r->n[5] *= a;
    r->n[6] *= a;
    r->n[7] *= a;
    r->n[8] *= a;
    r->n[9] *= a;
}

SECP256K1_INLINE static void secp256k1_fe_add(secp256k1_fe *r, const secp256k1_fe *a)
{
    r->n[0] += a->n[0];
    r->n[1] += a->n[1];
    r->n[2] += a->n[2];
    r->n[3] += a->n[3];
    r->n[4] += a->n[4];
    r->n[5] += a->n[5];
    r->n[6] += a->n[6];
    r->n[7] += a->n[7];
    r->n[8] += a->n[8];
    r->n[9] += a->n[9];
}

//重要!
SECP256K1_INLINE static void secp256k1_fe_mul(uint32_t *r, const uint32_t *a, const uint32_t * b)
{
    uint64_t c, d;
    uint64_t u0, u1, u2, u3, u4, u5, u6, u7, u8;
    uint32_t t9, t1, t0, t2, t3, t4, t5, t6, t7;
    const uint32_t M = 0x3FFFFFFUL, R1 = 0x400UL;  //, R0 = 0x3D10UL

    //下面一组变量用于计算64位数相乘，拆分后计算
    unsigned char szU0[8] = { 0 };
    uint64_t X00 = 0;
    uint64_t X01 = 0;
    uint64_t X02 = 0;
    uint64_t X03 = 0;
    uint64_t X04 = 0;
    uint64_t X05 = 0;
    uint64_t X06 = 0;
    uint64_t X07 = 0;

    d = (uint64_t)a[0] * b[9]
        + (uint64_t)a[1] * b[8]
        + (uint64_t)a[2] * b[7]
        + (uint64_t)a[3] * b[6]
        + (uint64_t)a[4] * b[5]
        + (uint64_t)a[5] * b[4]
        + (uint64_t)a[6] * b[3]
        + (uint64_t)a[7] * b[2]
        + (uint64_t)a[8] * b[1]
        + (uint64_t)a[9] * b[0];
    t9 = d & M; d >>= 26;

    c = (uint64_t)a[0] * b[0];

    d += (uint64_t)a[1] * b[9]
        + (uint64_t)a[2] * b[8]
        + (uint64_t)a[3] * b[7]
        + (uint64_t)a[4] * b[6]
        + (uint64_t)a[5] * b[5]
        + (uint64_t)a[6] * b[4]
        + (uint64_t)a[7] * b[3]
        + (uint64_t)a[8] * b[2]
        + (uint64_t)a[9] * b[1];

    u0 = d & M; d >>= 26;  //c += u0 * 0x3D10; //c += u0 * R0;

    //下面计算c += u0 * R0;
    szU0[0] = (u0     ) & 0xFF;
    szU0[1] = (u0>>  8) & 0xFF;
    szU0[2] = (u0>> 16) & 0xFF;
    szU0[3] = (u0>> 24) & 0xFF;
    szU0[4] = (u0>> 32) & 0xFF;
    szU0[5] = (u0>> 40) & 0xFF;
    szU0[6] = (u0>> 48) & 0xFF;
    szU0[7] = (u0>> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t0 = c & M; c >>= 26; c += u0 * R1;

    c += (uint64_t)a[0] * b[1]
        + (uint64_t)a[1] * b[0];

    d += (uint64_t)a[2] * b[9]
        + (uint64_t)a[3] * b[8]
        + (uint64_t)a[4] * b[7]
        + (uint64_t)a[5] * b[6]
        + (uint64_t)a[6] * b[5]
        + (uint64_t)a[7] * b[4]
        + (uint64_t)a[8] * b[3]
        + (uint64_t)a[9] * b[2];

    u1 = d & M; d >>= 26; //c += u1 * R0;
    //下面计算c += u1 * R0;
    szU0[0] = (u1      ) & 0xFF;
    szU0[1] = (u1 >>  8) & 0xFF;
    szU0[2] = (u1 >> 16) & 0xFF;
    szU0[3] = (u1 >> 24) & 0xFF;
    szU0[4] = (u1 >> 32) & 0xFF;
    szU0[5] = (u1 >> 40) & 0xFF;
    szU0[6] = (u1 >> 48) & 0xFF;
    szU0[7] = (u1 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t1 = c & M; c >>= 26; c += u1 * R1;

    c += (uint64_t)a[0] * b[2]
        + (uint64_t)a[1] * b[1]
        + (uint64_t)a[2] * b[0];

    d += (uint64_t)a[3] * b[9]
        + (uint64_t)a[4] * b[8]
        + (uint64_t)a[5] * b[7]
        + (uint64_t)a[6] * b[6]
        + (uint64_t)a[7] * b[5]
        + (uint64_t)a[8] * b[4]
        + (uint64_t)a[9] * b[3];

    u2 = d & M; d >>= 26; //c += u2 * R0;
    //下面计算c += u2 * R0;
    szU0[0] = (u2      ) & 0xFF;
    szU0[1] = (u2 >>  8) & 0xFF;
    szU0[2] = (u2 >> 16) & 0xFF;
    szU0[3] = (u2 >> 24) & 0xFF;
    szU0[4] = (u2 >> 32) & 0xFF;
    szU0[5] = (u2 >> 40) & 0xFF;
    szU0[6] = (u2 >> 48) & 0xFF;
    szU0[7] = (u2 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t2 = c & M; c >>= 26; c += u2 * R1;

    c += (uint64_t)a[0] * b[3]
        + (uint64_t)a[1] * b[2]
        + (uint64_t)a[2] * b[1]
        + (uint64_t)a[3] * b[0];

    d += (uint64_t)a[4] * b[9]
        + (uint64_t)a[5] * b[8]
        + (uint64_t)a[6] * b[7]
        + (uint64_t)a[7] * b[6]
        + (uint64_t)a[8] * b[5]
        + (uint64_t)a[9] * b[4];

    u3 = d & M; d >>= 26; //c += u3 * R0;
    //下面计算c += u3 * R0;
    szU0[0] = (u3      ) & 0xFF;
    szU0[1] = (u3 >>  8) & 0xFF;
    szU0[2] = (u3 >> 16) & 0xFF;
    szU0[3] = (u3 >> 24) & 0xFF;
    szU0[4] = (u3 >> 32) & 0xFF;
    szU0[5] = (u3 >> 40) & 0xFF;
    szU0[6] = (u3 >> 48) & 0xFF;
    szU0[7] = (u3 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t3 = c & M; c >>= 26; c += u3 * R1;

    c += (uint64_t)a[0] * b[4]
        + (uint64_t)a[1] * b[3]
        + (uint64_t)a[2] * b[2]
        + (uint64_t)a[3] * b[1]
        + (uint64_t)a[4] * b[0];

    d += (uint64_t)a[5] * b[9]
        + (uint64_t)a[6] * b[8]
        + (uint64_t)a[7] * b[7]
        + (uint64_t)a[8] * b[6]
        + (uint64_t)a[9] * b[5];

    u4 = d & M; d >>= 26; //c += u4 * R0;
    //下面计算c += u4 * R0;
    szU0[0] = (u4      ) & 0xFF;
    szU0[1] = (u4 >>  8) & 0xFF;
    szU0[2] = (u4 >> 16) & 0xFF;
    szU0[3] = (u4 >> 24) & 0xFF;
    szU0[4] = (u4 >> 32) & 0xFF;
    szU0[5] = (u4 >> 40) & 0xFF;
    szU0[6] = (u4 >> 48) & 0xFF;
    szU0[7] = (u4 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t4 = c & M; c >>= 26; c += u4 * R1;

    c += (uint64_t)a[0] * b[5]
        + (uint64_t)a[1] * b[4]
        + (uint64_t)a[2] * b[3]
        + (uint64_t)a[3] * b[2]
        + (uint64_t)a[4] * b[1]
        + (uint64_t)a[5] * b[0];

    d += (uint64_t)a[6] * b[9]
        + (uint64_t)a[7] * b[8]
        + (uint64_t)a[8] * b[7]
        + (uint64_t)a[9] * b[6];

    u5 = d & M; d >>= 26; //c += u5 * R0;
    //下面计算c += u5 * R0;
    szU0[0] = (u5      ) & 0xFF;
    szU0[1] = (u5 >>  8) & 0xFF;
    szU0[2] = (u5 >> 16) & 0xFF;
    szU0[3] = (u5 >> 24) & 0xFF;
    szU0[4] = (u5 >> 32) & 0xFF;
    szU0[5] = (u5 >> 40) & 0xFF;
    szU0[6] = (u5 >> 48) & 0xFF;
    szU0[7] = (u5 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t5 = c & M; c >>= 26; c += u5 * R1;

    c += (uint64_t)a[0] * b[6]
        + (uint64_t)a[1] * b[5]
        + (uint64_t)a[2] * b[4]
        + (uint64_t)a[3] * b[3]
        + (uint64_t)a[4] * b[2]
        + (uint64_t)a[5] * b[1]
        + (uint64_t)a[6] * b[0];

    d += (uint64_t)a[7] * b[9]
        + (uint64_t)a[8] * b[8]
        + (uint64_t)a[9] * b[7];

    u6 = d & M; d >>= 26; //c += u6 * R0;
    //下面计算c += u6 * R0;
    szU0[0] = (u6      ) & 0xFF;
    szU0[1] = (u6 >>  8) & 0xFF;
    szU0[2] = (u6 >> 16) & 0xFF;
    szU0[3] = (u6 >> 24) & 0xFF;
    szU0[4] = (u6 >> 32) & 0xFF;
    szU0[5] = (u6 >> 40) & 0xFF;
    szU0[6] = (u6 >> 48) & 0xFF;
    szU0[7] = (u6 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t6 = c & M; c >>= 26; c += u6 * R1;

    c += (uint64_t)a[0] * b[7]
        + (uint64_t)a[1] * b[6]
        + (uint64_t)a[2] * b[5]
        + (uint64_t)a[3] * b[4]
        + (uint64_t)a[4] * b[3]
        + (uint64_t)a[5] * b[2]
        + (uint64_t)a[6] * b[1]
        + (uint64_t)a[7] * b[0];

    d += (uint64_t)a[8] * b[9]
        + (uint64_t)a[9] * b[8];

    u7 = d & M; d >>= 26; //c += u7 * R0;
    //下面计算c += u7 * R0;
    szU0[0] = (u7      ) & 0xFF;
    szU0[1] = (u7 >>  8) & 0xFF;
    szU0[2] = (u7 >> 16) & 0xFF;
    szU0[3] = (u7 >> 24) & 0xFF;
    szU0[4] = (u7 >> 32) & 0xFF;
    szU0[5] = (u7 >> 40) & 0xFF;
    szU0[6] = (u7 >> 48) & 0xFF;
    szU0[7] = (u7 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t7 = c & M; c >>= 26; c += u7 * R1;

    c += (uint64_t)a[0] * b[8]
        + (uint64_t)a[1] * b[7]
        + (uint64_t)a[2] * b[6]
        + (uint64_t)a[3] * b[5]
        + (uint64_t)a[4] * b[4]
        + (uint64_t)a[5] * b[3]
        + (uint64_t)a[6] * b[2]
        + (uint64_t)a[7] * b[1]
        + (uint64_t)a[8] * b[0];

    d += (uint64_t)a[9] * b[9];

    u8 = d & M; d >>= 26; //c += u8 * R0;
    //下面计算c += u8 * R0;
    szU0[0] = (u8      ) & 0xFF;
    szU0[1] = (u8 >>  8) & 0xFF;
    szU0[2] = (u8 >> 16) & 0xFF;
    szU0[3] = (u8 >> 24) & 0xFF;
    szU0[4] = (u8 >> 32) & 0xFF;
    szU0[5] = (u8 >> 40) & 0xFF;
    szU0[6] = (u8 >> 48) & 0xFF;
    szU0[7] = (u8 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<=  8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    r[3] = t3;
    r[4] = t4;
    r[5] = t5;
    r[6] = t6;
    r[7] = t7;
    r[8] = c & M; c >>= 26; c += u8 * R1;

    //c += d * R0 + t9;
    //下面计算c += d * R0 + t9;
    szU0[0] = (d      ) & 0xFF;
    szU0[1] = (d >>  8) & 0xFF;
    szU0[2] = (d >> 16) & 0xFF;
    szU0[3] = (d >> 24) & 0xFF;
    szU0[4] = (d >> 32) & 0xFF;
    szU0[5] = (d >> 40) & 0xFF;
    szU0[6] = (d >> 48) & 0xFF;
    szU0[7] = (d >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D10;
    X01 = szU0[1] * 0x3D10; X01 <<= 8;
    X02 = szU0[2] * 0x3D10; X02 <<= 16;
    X03 = szU0[3] * 0x3D10; X03 <<= 24;
    X04 = szU0[4] * 0x3D10; X04 <<= 32;
    X05 = szU0[5] * 0x3D10; X05 <<= 40;
    X06 = szU0[6] * 0x3D10; X06 <<= 48;
    X07 = szU0[7] * 0x3D10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07 + t9;

    r[9] = c & (M >> 4); c >>= 22; c += d * (R1 << 4);

    //d = c * (R0 >> 4) + t0;
    //下面计算d = c * (R0 >> 4) + t0;
    szU0[0] = (c      ) & 0xFF;
    szU0[1] = (c >>  8) & 0xFF;
    szU0[2] = (c >> 16) & 0xFF;
    szU0[3] = (c >> 24) & 0xFF;
    szU0[4] = (c >> 32) & 0xFF;
    szU0[5] = (c >> 40) & 0xFF;
    szU0[6] = (c >> 48) & 0xFF;
    szU0[7] = (c >> 56) & 0xFF;
    X00 = szU0[0] * 0x3D1;
    X01 = szU0[1] * 0x3D1; X01 <<=  8;
    X02 = szU0[2] * 0x3D1; X02 <<= 16;
    X03 = szU0[3] * 0x3D1; X03 <<= 24;
    X04 = szU0[4] * 0x3D1; X04 <<= 32;
    X05 = szU0[5] * 0x3D1; X05 <<= 40;
    X06 = szU0[6] * 0x3D1; X06 <<= 48;
    X07 = szU0[7] * 0x3D1; X07 <<= 56;
    d = X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07 + t0;

    r[0] = d & M; d >>= 26;

    d += c * (R1 >> 4) + t1;

    r[1] = d & M; d >>= 26;

    d += t2;

    r[2] = d;

    /*
    printf("--->GPU--");
    unsigned char * pOutX = (unsigned char *)&d;
    for (unsigned int j = 0; j < 16; j++)
        printf("%02x ", pOutX[j]);
    printf("\r\n");
    */
}

//重要!
SECP256K1_INLINE static void secp256k1_fe_sqr(uint32_t *r, const uint32_t *a)
{
    uint64_t c, d;
    uint64_t u0 = 0, u1 = 0, u2 = 0, u3 = 0, u4 = 0, u5 = 0, u6 = 0, u7 = 0, u8 = 0;
    uint32_t t9, t0, t1, t2, t3, t4, t5, t6, t7;
    const uint32_t M = 0x3FFFFFFUL, R1 = 0x400UL;    //, R0 = 0x3D10UL

    //下面这组变量用于在Intel4600(OpenCL1.2环境下计算64位数的乘法，因为Intel4600存在一个Bug，如果直接计算就会结果错误)
    unsigned char szU0[8] = { 0 };
    uint64_t X00 = 0;
    uint64_t X01 = 0;
    uint64_t X02 = 0;
    uint64_t X03 = 0;
    uint64_t X04 = 0;
    uint64_t X05 = 0;
    uint64_t X06 = 0;
    uint64_t X07 = 0;

    d = (uint64_t)(a[0] * 2) * a[9]
        + (uint64_t)(a[1] * 2) * a[8]
        + (uint64_t)(a[2] * 2) * a[7]
        + (uint64_t)(a[3] * 2) * a[6]
        + (uint64_t)(a[4] * 2) * a[5];

    t9 = d & M; d >>= 26;

    c = (uint64_t)a[0] * a[0];

    d += (uint64_t)(a[1] * 2) * a[9]
        + (uint64_t)(a[2] * 2) * a[8]
        + (uint64_t)(a[3] * 2) * a[7]
        + (uint64_t)(a[4] * 2) * a[6]
        + (uint64_t)a[5] * a[5];

    u0 = d & M; d >>= 26; //c += u0 * R0;
    //下面计算c += u0 * R0;
    szU0[0] = (u0      ) & 0xFF;
    szU0[1] = (u0 >>  8) & 0xFF;
    szU0[2] = (u0 >> 16) & 0xFF;
    szU0[3] = (u0 >> 24) & 0xFF;
    szU0[4] = (u0 >> 32) & 0xFF;
    szU0[5] = (u0 >> 40) & 0xFF;
    szU0[6] = (u0 >> 48) & 0xFF;
    szU0[7] = (u0 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t0 = c & M; c >>= 26; c += u0 * R1;

    c += (uint64_t)(a[0] * 2) * a[1];

    d += (uint64_t)(a[2] * 2) * a[9]
        + (uint64_t)(a[3] * 2) * a[8]
        + (uint64_t)(a[4] * 2) * a[7]
        + (uint64_t)(a[5] * 2) * a[6];

    u1 = d & M; d >>= 26; //c += u1 * R0;
    //下面计算c += u1 * R0;
    szU0[0] = (u1      ) & 0xFF;
    szU0[1] = (u1 >>  8) & 0xFF;
    szU0[2] = (u1 >> 16) & 0xFF;
    szU0[3] = (u1 >> 24) & 0xFF;
    szU0[4] = (u1 >> 32) & 0xFF;
    szU0[5] = (u1 >> 40) & 0xFF;
    szU0[6] = (u1 >> 48) & 0xFF;
    szU0[7] = (u1 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t1 = c & M; c >>= 26; c += u1 * R1;

    c += (uint64_t)(a[0] * 2) * a[2]
        + (uint64_t)a[1] * a[1];

    d += (uint64_t)(a[3] * 2) * a[9]
        + (uint64_t)(a[4] * 2) * a[8]
        + (uint64_t)(a[5] * 2) * a[7]
        + (uint64_t)a[6] * a[6];

    u2 = d & M; d >>= 26; //c += u2 * R0;
    //下面计算c += u2 * R0;
    szU0[0] = (u2      ) & 0xFF;
    szU0[1] = (u2 >>  8) & 0xFF;
    szU0[2] = (u2 >> 16) & 0xFF;
    szU0[3] = (u2 >> 24) & 0xFF;
    szU0[4] = (u2 >> 32) & 0xFF;
    szU0[5] = (u2 >> 40) & 0xFF;
    szU0[6] = (u2 >> 48) & 0xFF;
    szU0[7] = (u2 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t2 = c & M; c >>= 26; c += u2 * R1;

    c += (uint64_t)(a[0] * 2) * a[3]
        + (uint64_t)(a[1] * 2) * a[2];

    d += (uint64_t)(a[4] * 2) * a[9]
        + (uint64_t)(a[5] * 2) * a[8]
        + (uint64_t)(a[6] * 2) * a[7];

    u3 = d & M; d >>= 26;  //c += u3 * R0;
    //下面计算c += u3 * R0;
    szU0[0] = (u3      ) & 0xFF;
    szU0[1] = (u3 >>  8) & 0xFF;
    szU0[2] = (u3 >> 16) & 0xFF;
    szU0[3] = (u3 >> 24) & 0xFF;
    szU0[4] = (u3 >> 32) & 0xFF;
    szU0[5] = (u3 >> 40) & 0xFF;
    szU0[6] = (u3 >> 48) & 0xFF;
    szU0[7] = (u3 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t3 = c & M; c >>= 26; c += u3 * R1;

    c += (uint64_t)(a[0] * 2) * a[4]
        + (uint64_t)(a[1] * 2) * a[3]
        + (uint64_t)a[2] * a[2];

    d += (uint64_t)(a[5] * 2) * a[9]
        + (uint64_t)(a[6] * 2) * a[8]
        + (uint64_t)a[7] * a[7];

    u4 = d & M; d >>= 26; //c += u4 * R0;
    //下面计算c += u4 * R0;
    szU0[0] = (u4      ) & 0xFF;
    szU0[1] = (u4 >>  8) & 0xFF;
    szU0[2] = (u4 >> 16) & 0xFF;
    szU0[3] = (u4 >> 24) & 0xFF;
    szU0[4] = (u4 >> 32) & 0xFF;
    szU0[5] = (u4 >> 40) & 0xFF;
    szU0[6] = (u4 >> 48) & 0xFF;
    szU0[7] = (u4 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t4 = c & M; c >>= 26; c += u4 * R1;

    c += (uint64_t)(a[0] * 2) * a[5]
        + (uint64_t)(a[1] * 2) * a[4]
        + (uint64_t)(a[2] * 2) * a[3];

    d += (uint64_t)(a[6] * 2) * a[9]
        + (uint64_t)(a[7] * 2) * a[8];

    u5 = d & M; d >>= 26; //c += u5 * R0;
    //下面计算c += u5 * R0;
    szU0[0] = (u5      ) & 0xFF;
    szU0[1] = (u5 >>  8) & 0xFF;
    szU0[2] = (u5 >> 16) & 0xFF;
    szU0[3] = (u5 >> 24) & 0xFF;
    szU0[4] = (u5 >> 32) & 0xFF;
    szU0[5] = (u5 >> 40) & 0xFF;
    szU0[6] = (u5 >> 48) & 0xFF;
    szU0[7] = (u5 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t5 = c & M; c >>= 26; c += u5 * R1;

    c += (uint64_t)(a[0] * 2) * a[6]
        + (uint64_t)(a[1] * 2) * a[5]
        + (uint64_t)(a[2] * 2) * a[4]
        + (uint64_t)a[3] * a[3];

    d += (uint64_t)(a[7] * 2) * a[9]
        + (uint64_t)a[8] * a[8];

    u6 = d & M; d >>= 26; //c += u6 * R0;
    //下面计算c += u6 * R0;
    szU0[0] = (u6      ) & 0xFF;
    szU0[1] = (u6 >>  8) & 0xFF;
    szU0[2] = (u6 >> 16) & 0xFF;
    szU0[3] = (u6 >> 24) & 0xFF;
    szU0[4] = (u6 >> 32) & 0xFF;
    szU0[5] = (u6 >> 40) & 0xFF;
    szU0[6] = (u6 >> 48) & 0xFF;
    szU0[7] = (u6 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t6 = c & M; c >>= 26; c += u6 * R1;

    c += (uint64_t)(a[0] * 2) * a[7]
        + (uint64_t)(a[1] * 2) * a[6]
        + (uint64_t)(a[2] * 2) * a[5]
        + (uint64_t)(a[3] * 2) * a[4];

    d += (uint64_t)(a[8] * 2) * a[9];

    u7 = d & M; d >>= 26; //c += u7 * R0;
    //下面计算c += u7 * R0;
    szU0[0] = (u7      ) & 0xFF;
    szU0[1] = (u7 >>  8) & 0xFF;
    szU0[2] = (u7 >> 16) & 0xFF;
    szU0[3] = (u7 >> 24) & 0xFF;
    szU0[4] = (u7 >> 32) & 0xFF;
    szU0[5] = (u7 >> 40) & 0xFF;
    szU0[6] = (u7 >> 48) & 0xFF;
    szU0[7] = (u7 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    t7 = c & M; c >>= 26; c += u7 * R1;

    c += (uint64_t)(a[0] * 2) * a[8]
        + (uint64_t)(a[1] * 2) * a[7]
        + (uint64_t)(a[2] * 2) * a[6]
        + (uint64_t)(a[3] * 2) * a[5]
        + (uint64_t)a[4] * a[4];

    d += (uint64_t)a[9] * a[9];

    u8 = d & M; d >>= 26; //c += u8 * R0;
    //下面计算c += u8 * R0;
    szU0[0] = (u8      ) & 0xFF;
    szU0[1] = (u8 >>  8) & 0xFF;
    szU0[2] = (u8 >> 16) & 0xFF;
    szU0[3] = (u8 >> 24) & 0xFF;
    szU0[4] = (u8 >> 32) & 0xFF;
    szU0[5] = (u8 >> 40) & 0xFF;
    szU0[6] = (u8 >> 48) & 0xFF;
    szU0[7] = (u8 >> 56) & 0xFF;
    X00 = szU0[0] * 0x3d10;
    X01 = szU0[1] * 0x3d10; X01 <<= 8;
    X02 = szU0[2] * 0x3d10; X02 <<= 16;
    X03 = szU0[3] * 0x3d10; X03 <<= 24;
    X04 = szU0[4] * 0x3d10; X04 <<= 32;
    X05 = szU0[5] * 0x3d10; X05 <<= 40;
    X06 = szU0[6] * 0x3d10; X06 <<= 48;
    X07 = szU0[7] * 0x3d10; X07 <<= 56;
    c += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07;

    r[3] = t3;
    r[4] = t4;
    r[5] = t5;
    r[6] = t6;
    r[7] = t7;
    r[8] = c & M; c >>= 26; c += u8 * R1;

    //c += d * R0 + t9;
    //下面计算c += d * R0 + t9;
    szU0[0] = (d      ) & 0xFF;
    szU0[1] = (d >>  8) & 0xFF;
    szU0[2] = (d >> 16) & 0xFF;
    szU0[3] = (d >> 24) & 0xFF;
    szU0[4] = (d >> 32) & 0xFF;
    szU0[5] = (d >> 40) & 0xFF;
    szU0[6] = (d >> 48) & 0xFF;
    szU0[7] = (d >> 56) & 0xFF;
    X00 = szU0[0] * 0x03d10;
    X01 = szU0[1] * 0x03d10; X01 <<= 8;
    X02 = szU0[2] * 0x03d10; X02 <<= 16;
    X03 = szU0[3] * 0x03d10; X03 <<= 24;
    X04 = szU0[4] * 0x03d10; X04 <<= 32;
    X05 = szU0[5] * 0x03d10; X05 <<= 40;
    X06 = szU0[6] * 0x03d10; X06 <<= 48;
    X07 = szU0[7] * 0x03d10; X07 <<= 56;
    c = c + X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07 + t9;

    r[9] = c & (M >> 4); c >>= 22; c += d * (R1 << 4);

    //d = c * (R0 >> 4) + t0;
    //下面计算d = c * (R0 >> 4) + t0;
    szU0[0] = (c      ) & 0xFF;
    szU0[1] = (c >>  8) & 0xFF;
    szU0[2] = (c >> 16) & 0xFF;
    szU0[3] = (c >> 24) & 0xFF;
    szU0[4] = (c >> 32) & 0xFF;
    szU0[5] = (c >> 40) & 0xFF;
    szU0[6] = (c >> 48) & 0xFF;
    szU0[7] = (c >> 56) & 0xFF;
    X00 = szU0[0] * 0x03d1;
    X01 = szU0[1] * 0x03d1; X01 <<=  8;
    X02 = szU0[2] * 0x03d1; X02 <<= 16;
    X03 = szU0[3] * 0x03d1; X03 <<= 24;
    X04 = szU0[4] * 0x03d1; X04 <<= 32;
    X05 = szU0[5] * 0x03d1; X05 <<= 40;
    X06 = szU0[6] * 0x03d1; X06 <<= 48;
    X07 = szU0[7] * 0x03d1; X07 <<= 56;
    d = X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07 + t0;

    r[0] = d & M; d >>= 26;

    //d += c * (R1 >> 4) + t1;
    //下面计算d += c * (R1 >> 4) + t1;
    szU0[0] = (c      ) & 0xFF;
    szU0[1] = (c >>  8) & 0xFF;
    szU0[2] = (c >> 16) & 0xFF;
    szU0[3] = (c >> 24) & 0xFF;
    szU0[4] = (c >> 32) & 0xFF;
    szU0[5] = (c >> 40) & 0xFF;
    szU0[6] = (c >> 48) & 0xFF;
    szU0[7] = (c >> 56) & 0xFF;
    X00 = szU0[0] * 0x40;
    X01 = szU0[1] * 0x40; X01 <<=  8;
    X02 = szU0[2] * 0x40; X02 <<= 16;
    X03 = szU0[3] * 0x40; X03 <<= 24;
    X04 = szU0[4] * 0x40; X04 <<= 32;
    X05 = szU0[5] * 0x40; X05 <<= 40;
    X06 = szU0[6] * 0x40; X06 <<= 48;
    X07 = szU0[7] * 0x40; X07 <<= 56;
    d += X00 + X01 + X02 + X03 + X04 + X05 + X06 + X07 + t1;

    r[1] = d & M; d >>= 26;

    d += t2;

    r[2] = d;

/*
    printf("-10-->GPU STRAT--->");
    unsigned char * pOutX = (unsigned char *)r;
    for (unsigned int j = 0; j < 16; j++)
        printf("%02x ", pOutX[j]);
    printf("<---GPU END<---\r\n");
*/

}

SECP256K1_INLINE static void secp256k1_fe_cmov(secp256k1_fe *r, const secp256k1_fe *a, int flag)
{
    uint32_t mask0, mask1;
    mask0 = flag + ~((uint32_t)0);
    mask1 = ~mask0;
    r->n[0] = (r->n[0] & mask0) | (a->n[0] & mask1);
    r->n[1] = (r->n[1] & mask0) | (a->n[1] & mask1);
    r->n[2] = (r->n[2] & mask0) | (a->n[2] & mask1);
    r->n[3] = (r->n[3] & mask0) | (a->n[3] & mask1);
    r->n[4] = (r->n[4] & mask0) | (a->n[4] & mask1);
    r->n[5] = (r->n[5] & mask0) | (a->n[5] & mask1);
    r->n[6] = (r->n[6] & mask0) | (a->n[6] & mask1);
    r->n[7] = (r->n[7] & mask0) | (a->n[7] & mask1);
    r->n[8] = (r->n[8] & mask0) | (a->n[8] & mask1);
    r->n[9] = (r->n[9] & mask0) | (a->n[9] & mask1);
}

SECP256K1_INLINE static void secp256k1_fe_inv(secp256k1_fe *r, const secp256k1_fe *a)
{
    secp256k1_fe x2, x3, x6, x9, x11, x22, x44, x88, x176, x220, x223, t1;
    int j;

    /** The binary representation of (p - 2) has 5 blocks of 1s, with lengths in
     *  { 1, 2, 22, 223 }. Use an addition chain to calculate 2^n - 1 for each block:
     *  [1], [2], 3, 6, 9, 11, [22], 44, 88, 176, 220, [223]
     */

    secp256k1_fe_sqr(x2.n, a->n);
    secp256k1_fe_mul(x2.n, x2.n, a->n);

    secp256k1_fe_sqr(x3.n, x2.n);
    secp256k1_fe_mul(x3.n, x3.n, a->n);

    x6 = x3;
    for (j = 0; j < 3; j++)
    {
        secp256k1_fe_sqr(x6.n, x6.n);
    }
    secp256k1_fe_mul(x6.n, x6.n, x3.n);

    x9 = x6;
    for (j = 0; j < 3; j++)
    {
        secp256k1_fe_sqr(x9.n, x9.n);
    }
    secp256k1_fe_mul(x9.n, x9.n, x3.n);

    x11 = x9;
    for (j = 0; j < 2; j++)
    {
        secp256k1_fe_sqr(x11.n, x11.n);
    }
    secp256k1_fe_mul(x11.n, x11.n, x2.n);

    x22 = x11;
    for (j = 0; j < 11; j++)
    {
        secp256k1_fe_sqr(x22.n, x22.n);
    }
    secp256k1_fe_mul(x22.n, x22.n, x11.n);

    x44 = x22;
    for (j = 0; j < 22; j++)
    {
        secp256k1_fe_sqr(x44.n, x44.n);
    }
    secp256k1_fe_mul(x44.n, x44.n, x22.n);

    x88 = x44;
    for (j = 0; j < 44; j++)
    {
        secp256k1_fe_sqr(x88.n, x88.n);
    }
    secp256k1_fe_mul(x88.n, x88.n, x44.n);

    x176 = x88;
    for (j = 0; j < 88; j++)
    {
        secp256k1_fe_sqr(x176.n, x176.n);
    }
    secp256k1_fe_mul(x176.n, x176.n, x88.n);

    x220 = x176;
    for (j = 0; j < 44; j++)
    {
        secp256k1_fe_sqr(x220.n, x220.n);
    }
    secp256k1_fe_mul(x220.n, x220.n, x44.n);

    x223 = x220;
    for (j = 0; j < 3; j++)
    {
        secp256k1_fe_sqr(x223.n, x223.n);
    }
    secp256k1_fe_mul(x223.n, x223.n, x3.n);

    /* The final result is then assembled using a sliding window over the blocks. */

    t1 = x223;
    for (j = 0; j < 23; j++)
    {
        secp256k1_fe_sqr(t1.n, t1.n);
    }
    secp256k1_fe_mul(t1.n, t1.n, x22.n);
    for (j = 0; j < 5; j++)
    {
        secp256k1_fe_sqr(t1.n, t1.n);
    }
    secp256k1_fe_mul(t1.n, t1.n, a->n);
    for (j = 0; j < 3; j++)
    {
        secp256k1_fe_sqr(t1.n, t1.n);
    }
    secp256k1_fe_mul(t1.n, t1.n, x2.n);
    for (j = 0; j < 2; j++)
    {
        secp256k1_fe_sqr(t1.n, t1.n);
    }

    secp256k1_fe_mul(r->n, a->n, t1.n);
}

//===================================================//
/** Generator for secp256k1, value 'g' defined in
 *  "Standards for Efficient Cryptography" (SEC2) 2.7.1.
 */
/*
const secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
    0x79BE667EUL, 0xF9DCBBACUL, 0x55A06295UL, 0xCE870B07UL,
    0x029BFCDBUL, 0x2DCE28D9UL, 0x59F2815BUL, 0x16F81798UL,
    0x483ADA77UL, 0x26A3C465UL, 0x5DA4FBFCUL, 0x0E1108A8UL,
    0xFD17B448UL, 0xA6855419UL, 0x9C47D08FUL, 0xFB10D4B8UL
);
#define CURVE_B (7)
*/

void secp256k1_ge_set_gej(secp256k1_ge *r, secp256k1_gej *a)
{
    secp256k1_fe z2, z3;
    r->infinity = a->infinity;
    secp256k1_fe_inv(&a->z, &a->z);
    secp256k1_fe_sqr(&z2.n, &a->z.n);
    secp256k1_fe_mul(&z3.n, &a->z.n, &z2.n);
    secp256k1_fe_mul(&a->x.n, &a->x.n, &z2.n);
    secp256k1_fe_mul(&a->y.n, &a->y.n, &z3.n);

    a->z.n[0] = 1;
    a->z.n[1] = a->z.n[2] = a->z.n[3] = a->z.n[4] = a->z.n[5] = a->z.n[6] = a->z.n[7] = a->z.n[8] = a->z.n[9] = 0;

    r->x = a->x;
    r->y = a->y;
}

void secp256k1_gej_set_ge(secp256k1_gej *r, const secp256k1_ge *a)
{
    r->infinity = a->infinity;
    r->x = a->x;
    r->y = a->y;

    r->z.n[0] = 1;
    r->z.n[1] = r->z.n[2] = r->z.n[3] = r->z.n[4] = r->z.n[5] = r->z.n[6] = r->z.n[7] = r->z.n[8] = r->z.n[9] = 0;
}

void secp256k1_gej_add_ge(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b)
{
    /* Operations: 7 mul, 5 sqr, 4 normalize, 21 mul_int/add/negate/cmov */
    const secp256k1_fe fe_1 = { 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    secp256k1_fe zz, u1, u2, s1, s2, t, tt, m, n, q, rr;
    secp256k1_fe m_alt, rr_alt;
    int infinity, degenerate;

    secp256k1_fe_sqr(zz.n, a->z.n);                     /* z = Z1^2 */
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);        /* u1 = U1 = X1*Z2^2 (1) */
    secp256k1_fe_mul(u2.n, b->x.n, zz.n);               /* u2 = U2 = X2*Z1^2 (1) */
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);        /* s1 = S1 = Y1*Z2^3 (1) */
    secp256k1_fe_mul(s2.n, b->y.n, zz.n);               /* s2 = Y2*Z1^2 (1) */
    secp256k1_fe_mul(s2.n, s2.n, a->z.n);               /* s2 = S2 = Y2*Z1^3 (1) */
    t = u1; secp256k1_fe_add(&t, &u2);                  /* t = T = U1+U2 (2) */
    m = s1; secp256k1_fe_add(&m, &s2);                  /* m = M = S1+S2 (2) */
    secp256k1_fe_sqr(rr.n, t.n);                        /* rr = T^2 (1) */
    secp256k1_fe_negate(&m_alt, &u2, 1);                /* Malt = -X2*Z1^2 */
    secp256k1_fe_mul(tt.n, u1.n, m_alt.n);              /* tt = -U1*U2 (2) */
    secp256k1_fe_add(&rr, &tt);                         /* rr = R = T^2-U1*U2 (3) */

    degenerate = secp256k1_fe_normalizes_to_zero(&m) & secp256k1_fe_normalizes_to_zero(&rr);

    rr_alt = s1;
    secp256k1_fe_mul_int(&rr_alt, 2);                   /* rr = Y1*Z2^3 - Y2*Z1^3 (2) */
    secp256k1_fe_add(&m_alt, &u1);                      /* Malt = X1*Z2^2 - X2*Z1^2 */
    secp256k1_fe_cmov(&rr_alt, &rr, !degenerate);
    secp256k1_fe_cmov(&m_alt, &m, !degenerate);
    secp256k1_fe_sqr(n.n, m_alt.n);                     /* n = Malt^2 (1) */
    secp256k1_fe_mul(q.n, n.n, t.n);                    /* q = Q = T*Malt^2 (1) */
    secp256k1_fe_sqr(&n.n, &n.n);
    secp256k1_fe_cmov(&n, &m, degenerate);              /* n = M^3 * Malt (2) */
    secp256k1_fe_sqr(&t.n, &rr_alt.n);                  /* t = Ralt^2 (1) */
    secp256k1_fe_mul(&r->z.n, &a->z.n, &m_alt.n);       /* r->z = Malt*Z (1) */
    infinity = secp256k1_fe_normalizes_to_zero(&r->z) * (1 - a->infinity);
    secp256k1_fe_mul_int(&r->z, 2);                     /* r->z = Z3 = 2*Malt*Z (2) */
    secp256k1_fe_negate(&q, &q, 1);                     /* q = -Q (2) */
    secp256k1_fe_add(&t, &q);                           /* t = Ralt^2-Q (3) */
    secp256k1_fe_normalize_weak(&t);
    r->x = t;                                           /* r->x = Ralt^2-Q (1) */
    secp256k1_fe_mul_int(&t, 2);                        /* t = 2*x3 (2) */
    secp256k1_fe_add(&t, &q);                           /* t = 2*x3 - Q: (4) */
    secp256k1_fe_mul(&t.n, &t.n, &rr_alt.n);            /* t = Ralt*(2*x3 - Q) (1) */
    secp256k1_fe_add(&t, &n);                           /* t = Ralt*(2*x3 - Q) + M^3*Malt (3) */
    secp256k1_fe_negate(&r->y, &t, 3);                  /* r->y = Ralt*(Q - 2x3) - M^3*Malt (4) */
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_mul_int(&r->x, 4);                     /* r->x = X3 = 4*(Ralt^2-Q) */
    secp256k1_fe_mul_int(&r->y, 4);                     /* r->y = Y3 = 4*Ralt*(Q - 2x3) - 4*M^3*Malt (4) */

    /** In case a->infinity == 1, replace r with (b->x, b->y, 1). */
    secp256k1_fe_cmov(&r->x, &b->x, a->infinity);
    secp256k1_fe_cmov(&r->y, &b->y, a->infinity);
    secp256k1_fe_cmov(&r->z, &fe_1, a->infinity);
    r->infinity = infinity;

    /*
    printf("--Step GPU-->");
    unsigned char * pOutX = (unsigned char *)& r->z;
    for (unsigned int j = 0; j < 65; j++)
        printf("%02x ", pOutX[j]);
    printf("\r\n");
    */
}

void secp256k1_ge_to_buff(unsigned char *out, const secp256k1_ge *a)
{
    //x
    out[0x00] = ( a->x.n[0]       | a->x.n[1] << 26 );
    out[0x01] = ( a->x.n[0]       | a->x.n[1] << 26 ) >>  8;
    out[0x02] = ( a->x.n[0]       | a->x.n[1] << 26 ) >> 16;
    out[0x03] = ( a->x.n[0]       | a->x.n[1] << 26 ) >> 24;

    out[0x04] = ( a->x.n[1] >>  6 | a->x.n[2] << 20 );
    out[0x05] = ( a->x.n[1] >>  6 | a->x.n[2] << 20 ) >>  8;
    out[0x06] = ( a->x.n[1] >>  6 | a->x.n[2] << 20 ) >> 16;
    out[0x07] = ( a->x.n[1] >>  6 | a->x.n[2] << 20 ) >> 24;

    out[0x08] = ( a->x.n[2] >> 12 | a->x.n[3] << 14 );
    out[0x09] = ( a->x.n[2] >> 12 | a->x.n[3] << 14 ) >>  8;
    out[0x0a] = ( a->x.n[2] >> 12 | a->x.n[3] << 14 ) >> 16;
    out[0x0b] = ( a->x.n[2] >> 12 | a->x.n[3] << 14 ) >> 24;

    out[0x0c] = ( a->x.n[3] >> 18 | a->x.n[4] <<  8 );
    out[0x0d] = ( a->x.n[3] >> 18 | a->x.n[4] <<  8 ) >>  8;
    out[0x0e] = ( a->x.n[3] >> 18 | a->x.n[4] <<  8 ) >> 16;
    out[0x0f] = ( a->x.n[3] >> 18 | a->x.n[4] <<  8 ) >> 24;

    out[0x10] = ( a->x.n[4] >> 24 | a->x.n[5] <<  2 | a->x.n[6] << 28 );
    out[0x11] = ( a->x.n[4] >> 24 | a->x.n[5] <<  2 | a->x.n[6] << 28 ) >>  8;
    out[0x12] = ( a->x.n[4] >> 24 | a->x.n[5] <<  2 | a->x.n[6] << 28 ) >> 16;
    out[0x13] = ( a->x.n[4] >> 24 | a->x.n[5] <<  2 | a->x.n[6] << 28 ) >> 24;

    out[0x14] = ( a->x.n[6] >>  4 | a->x.n[7] << 22 );
    out[0x15] = ( a->x.n[6] >>  4 | a->x.n[7] << 22 ) >>  8;
    out[0x16] = ( a->x.n[6] >>  4 | a->x.n[7] << 22 ) >> 16;
    out[0x17] = ( a->x.n[6] >>  4 | a->x.n[7] << 22 ) >> 24;

    out[0x18] = ( a->x.n[7] >> 10 | a->x.n[8] << 16 );
    out[0x19] = ( a->x.n[7] >> 10 | a->x.n[8] << 16 ) >>  8;
    out[0x1a] = ( a->x.n[7] >> 10 | a->x.n[8] << 16 ) >> 16;
    out[0x1b] = ( a->x.n[7] >> 10 | a->x.n[8] << 16 ) >> 24;

    out[0x1c] = ( a->x.n[8] >> 16 | a->x.n[9] << 10 );
    out[0x1d] = ( a->x.n[8] >> 16 | a->x.n[9] << 10 ) >>  8;
    out[0x1e] = ( a->x.n[8] >> 16 | a->x.n[9] << 10 ) >> 16;
    out[0x1f] = ( a->x.n[8] >> 16 | a->x.n[9] << 10 ) >> 24;

    //y
    out[0x20] = ( a->y.n[0]       | a->y.n[1] << 26 );
    out[0x21] = ( a->y.n[0]       | a->y.n[1] << 26 ) >>  8;
    out[0x22] = ( a->y.n[0]       | a->y.n[1] << 26 ) >> 16;
    out[0x23] = ( a->y.n[0]       | a->y.n[1] << 26 ) >> 24;

    out[0x24] = ( a->y.n[1] >>  6 | a->y.n[2] << 20 );
    out[0x25] = ( a->y.n[1] >>  6 | a->y.n[2] << 20 ) >>  8;
    out[0x26] = ( a->y.n[1] >>  6 | a->y.n[2] << 20 ) >> 16;
    out[0x27] = ( a->y.n[1] >>  6 | a->y.n[2] << 20 ) >> 24;

    out[0x28] = ( a->y.n[2] >> 12 | a->y.n[3] << 14 );
    out[0x29] = ( a->y.n[2] >> 12 | a->y.n[3] << 14 ) >>  8;
    out[0x2a] = ( a->y.n[2] >> 12 | a->y.n[3] << 14 ) >> 16;
    out[0x2b] = ( a->y.n[2] >> 12 | a->y.n[3] << 14 ) >> 24;

    out[0x2c] = ( a->y.n[3] >> 18 | a->y.n[4] <<  8 );
    out[0x2d] = ( a->y.n[3] >> 18 | a->y.n[4] <<  8 ) >>  8;
    out[0x2e] = ( a->y.n[3] >> 18 | a->y.n[4] <<  8 ) >> 16;
    out[0x2f] = ( a->y.n[3] >> 18 | a->y.n[4] <<  8 ) >> 24;

    out[0x30] = ( a->y.n[4] >> 24 | a->y.n[5] <<  2 | a->y.n[6] << 28 );
    out[0x31] = ( a->y.n[4] >> 24 | a->y.n[5] <<  2 | a->y.n[6] << 28 ) >>  8;
    out[0x32] = ( a->y.n[4] >> 24 | a->y.n[5] <<  2 | a->y.n[6] << 28 ) >> 16;
    out[0x33] = ( a->y.n[4] >> 24 | a->y.n[5] <<  2 | a->y.n[6] << 28 ) >> 24;

    out[0x34] = ( a->y.n[6] >>  4 | a->y.n[7] << 22 );
    out[0x35] = ( a->y.n[6] >>  4 | a->y.n[7] << 22 ) >>  8;
    out[0x36] = ( a->y.n[6] >>  4 | a->y.n[7] << 22 ) >> 16;
    out[0x37] = ( a->y.n[6] >>  4 | a->y.n[7] << 22 ) >> 24;

    out[0x38] = ( a->y.n[7] >> 10 | a->y.n[8] << 16 );
    out[0x39] = ( a->y.n[7] >> 10 | a->y.n[8] << 16 ) >>  8;
    out[0x3a] = ( a->y.n[7] >> 10 | a->y.n[8] << 16 ) >> 16;
    out[0x3b] = ( a->y.n[7] >> 10 | a->y.n[8] << 16 ) >> 24;

    out[0x3c] = ( a->y.n[8] >> 16 | a->y.n[9] << 10 );
    out[0x3d] = ( a->y.n[8] >> 16 | a->y.n[9] << 10 ) >>  8;
    out[0x3e] = ( a->y.n[8] >> 16 | a->y.n[9] << 10 ) >> 16;
    out[0x3f] = ( a->y.n[8] >> 16 | a->y.n[9] << 10 ) >> 24;
}
