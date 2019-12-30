
//===============计算secp256k1Begin==================//

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
    uint32_t x = t9 >> 22; t9 &= 0x03FFFFFUL;

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
    const uint32_t M = 0x3FFFFFFUL, R0 = 0x3D10UL, R1 = 0x400UL;

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

    u0 = d & M; d >>= 26; c += u0 * R0;

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

    u1 = d & M; d >>= 26; c += u1 * R0;

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

    u2 = d & M; d >>= 26; c += u2 * R0;

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

    u3 = d & M; d >>= 26; c += u3 * R0;

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

    u4 = d & M; d >>= 26; c += u4 * R0;

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

    u5 = d & M; d >>= 26; c += u5 * R0;

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

    u6 = d & M; d >>= 26; c += u6 * R0;

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

    u7 = d & M; d >>= 26; c += u7 * R0;

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

    u8 = d & M; d >>= 26; c += u8 * R0;

    r[3] = t3;
    r[4] = t4;
    r[5] = t5;
    r[6] = t6;
    r[7] = t7;
    r[8] = c & M; c >>= 26; c += u8 * R1;

    c += d * R0 + t9;

    r[9] = c & (M >> 4); c >>= 22; c += d * (R1 << 4);

    d = c * (R0 >> 4) + t0;

    r[0] = d & M; d >>= 26;

    d += c * (R1 >> 4) + t1;

    r[1] = d & M; d >>= 26;

    d += t2;

    r[2] = d;
}

//重要!
SECP256K1_INLINE static void secp256k1_fe_sqr(uint32_t *r, const uint32_t *a)
{
    uint64_t c, d;
    uint64_t u0, u1, u2, u3, u4, u5, u6, u7, u8;
    uint32_t t9, t0, t1, t2, t3, t4, t5, t6, t7;
    const uint32_t M = 0x3FFFFFFUL, R0 = 0x3D10UL, R1 = 0x400UL;

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

    u0 = d & M;
    d >>= 26;
    c += u0 * R0;

    t0 = c & M; c >>= 26; c += u0 * R1;

    c += (uint64_t)(a[0] * 2) * a[1];

    d += (uint64_t)(a[2] * 2) * a[9]
        + (uint64_t)(a[3] * 2) * a[8]
        + (uint64_t)(a[4] * 2) * a[7]
        + (uint64_t)(a[5] * 2) * a[6];

    u1 = d & M; d >>= 26; c += u1 * R0;

    t1 = c & M; c >>= 26; c += u1 * R1;

    c += (uint64_t)(a[0] * 2) * a[2]
        + (uint64_t)a[1] * a[1];

    d += (uint64_t)(a[3] * 2) * a[9]
        + (uint64_t)(a[4] * 2) * a[8]
        + (uint64_t)(a[5] * 2) * a[7]
        + (uint64_t)a[6] * a[6];

    u2 = d & M; d >>= 26; c += u2 * R0;

    t2 = c & M; c >>= 26; c += u2 * R1;


    c += (uint64_t)(a[0] * 2) * a[3]
        + (uint64_t)(a[1] * 2) * a[2];

    d += (uint64_t)(a[4] * 2) * a[9]
        + (uint64_t)(a[5] * 2) * a[8]
        + (uint64_t)(a[6] * 2) * a[7];

    u3 = d & M; d >>= 26; c += u3 * R0;

    t3 = c & M; c >>= 26; c += u3 * R1;

    c += (uint64_t)(a[0] * 2) * a[4]
        + (uint64_t)(a[1] * 2) * a[3]
        + (uint64_t)a[2] * a[2];

    d += (uint64_t)(a[5] * 2) * a[9]
        + (uint64_t)(a[6] * 2) * a[8]
        + (uint64_t)a[7] * a[7];

    u4 = d & M; d >>= 26; c += u4 * R0;

    t4 = c & M; c >>= 26; c += u4 * R1;

    c += (uint64_t)(a[0] * 2) * a[5]
        + (uint64_t)(a[1] * 2) * a[4]
        + (uint64_t)(a[2] * 2) * a[3];

    d += (uint64_t)(a[6] * 2) * a[9]
        + (uint64_t)(a[7] * 2) * a[8];

    u5 = d & M; d >>= 26; c += u5 * R0;

    t5 = c & M; c >>= 26; c += u5 * R1;

    c += (uint64_t)(a[0] * 2) * a[6]
        + (uint64_t)(a[1] * 2) * a[5]
        + (uint64_t)(a[2] * 2) * a[4]
        + (uint64_t)a[3] * a[3];

    d += (uint64_t)(a[7] * 2) * a[9]
        + (uint64_t)a[8] * a[8];

    u6 = d & M; d >>= 26; c += u6 * R0;

    t6 = c & M; c >>= 26; c += u6 * R1;

    c += (uint64_t)(a[0] * 2) * a[7]
        + (uint64_t)(a[1] * 2) * a[6]
        + (uint64_t)(a[2] * 2) * a[5]
        + (uint64_t)(a[3] * 2) * a[4];

    d += (uint64_t)(a[8] * 2) * a[9];

    u7 = d & M; d >>= 26; c += u7 * R0;

    t7 = c & M; c >>= 26; c += u7 * R1;

    c += (uint64_t)(a[0] * 2) * a[8]
        + (uint64_t)(a[1] * 2) * a[7]
        + (uint64_t)(a[2] * 2) * a[6]
        + (uint64_t)(a[3] * 2) * a[5]
        + (uint64_t)a[4] * a[4];

    d += (uint64_t)a[9] * a[9];

    u8 = d & M; d >>= 26; c += u8 * R0;

    r[3] = t3;
    r[4] = t4;
    r[5] = t5;
    r[6] = t6;
    r[7] = t7;
    r[8] = c & M; c >>= 26; c += u8 * R1;

    c += d * R0 + t9;

    r[9] = c & (M >> 4); c >>= 22; c += d * (R1 << 4);

    d = c * (R0 >> 4) + t0;

    r[0] = d & M; d >>= 26;

    d += c * (R1 >> 4) + t1;

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
//===============计算secp256k1  End==================//

//===============计算rmd-160Begin==================//
/********************************************************************\
 *
 *      FILE:     rmd160.c
 *
 *      CONTENTS: A sample C-implementation of the RIPEMD-160
 *                hash-function.
 *      TARGET:   any computer with an ANSI C compiler
 *
 *      AUTHOR:   Antoon Bosselaers, ESAT-COSIC
 *      DATE:     1 March 1996
 *      VERSION:  1.0
 *
 *      Copyright (c) Katholieke Universiteit Leuven
 *      1996, All Rights Reserved
 *
\********************************************************************/

/* typedef 8, 16 and 32 bit types, resp.  */
/* adapt these, if necessary, 
   for your operating system and compiler */
typedef    unsigned char        byte;   /* unsigned 8-bit integer */
typedef    unsigned short       word;   /* unsigned 16-bit integer */
typedef    unsigned long        dword;  /* unsigned 32-bit integer */ 
#ifndef dword
#define dword unsigned int
#endif

/********************************************************************/

/* macro definitions */

/* collect four bytes into one word: */
#define BYTES_TO_DWORD(strptr)                    \
            (((dword) *((strptr)+3) << 24) | \
             ((dword) *((strptr)+2) << 16) | \
             ((dword) *((strptr)+1) <<  8) | \
             ((dword) *(strptr)))

/* ROL(x, n) cyclically rotates x over n bits to the left */
/* x must be of an unsigned 32 bits type and 0 <= n < 32. */
#define ROL(x, n)        (((x) << (n)) | ((x) >> (32-(n))))

/* the three basic functions F(), G() and H() */
#define F(x, y, z)        ((x) ^ (y) ^ (z)) 
#define G(x, y, z)        (((x) & (y)) | (~(x) & (z))) 
#define H(x, y, z)        (((x) | ~(y)) ^ (z))
#define I(x, y, z)        (((x) & (z)) | ((y) & ~(z))) 
#define J(x, y, z)        ((x) ^ ((y) | ~(z)))
  
/* the eight basic operations FF() through III() */
#define FF(a, b, c, d, e, x, s)        {\
      (a) += F((b), (c), (d)) + (x);\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define GG(a, b, c, d, e, x, s)        {\
      (a) += G((b), (c), (d)) + (x) + 0x5a827999UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define HH(a, b, c, d, e, x, s)        {\
      (a) += H((b), (c), (d)) + (x) + 0x6ed9eba1UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define II(a, b, c, d, e, x, s)        {\
      (a) += I((b), (c), (d)) + (x) + 0x8f1bbcdcUL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define JJ(a, b, c, d, e, x, s)        {\
      (a) += J((b), (c), (d)) + (x) + 0xa953fd4eUL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define FFF(a, b, c, d, e, x, s)        {\
      (a) += F((b), (c), (d)) + (x);\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define GGG(a, b, c, d, e, x, s)        {\
      (a) += G((b), (c), (d)) + (x) + 0x7a6d76e9UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define HHH(a, b, c, d, e, x, s)        {\
      (a) += H((b), (c), (d)) + (x) + 0x6d703ef3UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define III(a, b, c, d, e, x, s)        {\
      (a) += I((b), (c), (d)) + (x) + 0x5c4dd124UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
#define JJJ(a, b, c, d, e, x, s)        {\
      (a) += J((b), (c), (d)) + (x) + 0x50a28be6UL;\
      (a) = ROL((a), (s)) + (e);\
      (c) = ROL((c), 10);\
   }
 

/********************************************************************/
//__kernel void rmd160(__global dword *MDbuf, __global byte *strptr)
void rmd160( dword *MDbuf, byte *strptr)
{
    MDbuf[0] = 0x67452301UL;
	MDbuf[1] = 0xefcdab89UL;
	MDbuf[2] = 0x98badcfeUL;
	MDbuf[3] = 0x10325476UL;
	MDbuf[4] = 0xc3d2e1f0UL;

	dword X[16];
	X[0] = 0;X[0x1] = 0;X[0x2] = 0;X[0x3] = 0;X[0x4] = 0;X[0x5] = 0;X[0x6] = 0;X[0x7] = 0;
	X[8] = 0;X[0x9] = 0;X[0xa] = 0;X[0xb] = 0;X[0xc] = 0;X[0xd] = 0;X[0xe] = 0;X[0xf] = 0;
	dword *pTmp = (dword *) strptr;
	X[0x0]=pTmp[0x0];
	X[0x1]=pTmp[0x1];
	X[0x2]=pTmp[0x2];
	X[0x3]=pTmp[0x3];
	X[0x4]=pTmp[0x4];
	X[0x5]=pTmp[0x5];
	X[0x6]=pTmp[0x6];
	X[0x7]=pTmp[0x7];
	X[0x8]=0x80;
	X[14] = 0x100;
	X[15] = 0;

    dword  aa = MDbuf[0],  bb = MDbuf[1],  cc = MDbuf[2],  dd = MDbuf[3],  ee = MDbuf[4];
    dword aaa = MDbuf[0], bbb = MDbuf[1], ccc = MDbuf[2], ddd = MDbuf[3], eee = MDbuf[4];

   /* round 1 */
   FF(aa, bb, cc, dd, ee, X[ 0], 11);
   FF(ee, aa, bb, cc, dd, X[ 1], 14);
   FF(dd, ee, aa, bb, cc, X[ 2], 15);
   FF(cc, dd, ee, aa, bb, X[ 3], 12);
   FF(bb, cc, dd, ee, aa, X[ 4],  5);
   FF(aa, bb, cc, dd, ee, X[ 5],  8);
   FF(ee, aa, bb, cc, dd, X[ 6],  7);
   FF(dd, ee, aa, bb, cc, X[ 7],  9);
   FF(cc, dd, ee, aa, bb, X[ 8], 11);
   FF(bb, cc, dd, ee, aa, X[ 9], 13);
   FF(aa, bb, cc, dd, ee, X[10], 14);
   FF(ee, aa, bb, cc, dd, X[11], 15);
   FF(dd, ee, aa, bb, cc, X[12],  6);
   FF(cc, dd, ee, aa, bb, X[13],  7);
   FF(bb, cc, dd, ee, aa, X[14],  9);
   FF(aa, bb, cc, dd, ee, X[15],  8);
                             
   /* round 2 */
   GG(ee, aa, bb, cc, dd, X[ 7],  7);
   GG(dd, ee, aa, bb, cc, X[ 4],  6);
   GG(cc, dd, ee, aa, bb, X[13],  8);
   GG(bb, cc, dd, ee, aa, X[ 1], 13);
   GG(aa, bb, cc, dd, ee, X[10], 11);
   GG(ee, aa, bb, cc, dd, X[ 6],  9);
   GG(dd, ee, aa, bb, cc, X[15],  7);
   GG(cc, dd, ee, aa, bb, X[ 3], 15);
   GG(bb, cc, dd, ee, aa, X[12],  7);
   GG(aa, bb, cc, dd, ee, X[ 0], 12);
   GG(ee, aa, bb, cc, dd, X[ 9], 15);
   GG(dd, ee, aa, bb, cc, X[ 5],  9);
   GG(cc, dd, ee, aa, bb, X[ 2], 11);
   GG(bb, cc, dd, ee, aa, X[14],  7);
   GG(aa, bb, cc, dd, ee, X[11], 13);
   GG(ee, aa, bb, cc, dd, X[ 8], 12);

   /* round 3 */
   HH(dd, ee, aa, bb, cc, X[ 3], 11);
   HH(cc, dd, ee, aa, bb, X[10], 13);
   HH(bb, cc, dd, ee, aa, X[14],  6);
   HH(aa, bb, cc, dd, ee, X[ 4],  7);
   HH(ee, aa, bb, cc, dd, X[ 9], 14);
   HH(dd, ee, aa, bb, cc, X[15],  9);
   HH(cc, dd, ee, aa, bb, X[ 8], 13);
   HH(bb, cc, dd, ee, aa, X[ 1], 15);
   HH(aa, bb, cc, dd, ee, X[ 2], 14);
   HH(ee, aa, bb, cc, dd, X[ 7],  8);
   HH(dd, ee, aa, bb, cc, X[ 0], 13);
   HH(cc, dd, ee, aa, bb, X[ 6],  6);
   HH(bb, cc, dd, ee, aa, X[13],  5);
   HH(aa, bb, cc, dd, ee, X[11], 12);
   HH(ee, aa, bb, cc, dd, X[ 5],  7);
   HH(dd, ee, aa, bb, cc, X[12],  5);

   /* round 4 */
   II(cc, dd, ee, aa, bb, X[ 1], 11);
   II(bb, cc, dd, ee, aa, X[ 9], 12);
   II(aa, bb, cc, dd, ee, X[11], 14);
   II(ee, aa, bb, cc, dd, X[10], 15);
   II(dd, ee, aa, bb, cc, X[ 0], 14);
   II(cc, dd, ee, aa, bb, X[ 8], 15);
   II(bb, cc, dd, ee, aa, X[12],  9);
   II(aa, bb, cc, dd, ee, X[ 4],  8);
   II(ee, aa, bb, cc, dd, X[13],  9);
   II(dd, ee, aa, bb, cc, X[ 3], 14);
   II(cc, dd, ee, aa, bb, X[ 7],  5);
   II(bb, cc, dd, ee, aa, X[15],  6);
   II(aa, bb, cc, dd, ee, X[14],  8);
   II(ee, aa, bb, cc, dd, X[ 5],  6);
   II(dd, ee, aa, bb, cc, X[ 6],  5);
   II(cc, dd, ee, aa, bb, X[ 2], 12);

   /* round 5 */
   JJ(bb, cc, dd, ee, aa, X[ 4],  9);
   JJ(aa, bb, cc, dd, ee, X[ 0], 15);
   JJ(ee, aa, bb, cc, dd, X[ 5],  5);
   JJ(dd, ee, aa, bb, cc, X[ 9], 11);
   JJ(cc, dd, ee, aa, bb, X[ 7],  6);
   JJ(bb, cc, dd, ee, aa, X[12],  8);
   JJ(aa, bb, cc, dd, ee, X[ 2], 13);
   JJ(ee, aa, bb, cc, dd, X[10], 12);
   JJ(dd, ee, aa, bb, cc, X[14],  5);
   JJ(cc, dd, ee, aa, bb, X[ 1], 12);
   JJ(bb, cc, dd, ee, aa, X[ 3], 13);
   JJ(aa, bb, cc, dd, ee, X[ 8], 14);
   JJ(ee, aa, bb, cc, dd, X[11], 11);
   JJ(dd, ee, aa, bb, cc, X[ 6],  8);
   JJ(cc, dd, ee, aa, bb, X[15],  5);
   JJ(bb, cc, dd, ee, aa, X[13],  6);

   /* parallel round 1 */
   JJJ(aaa, bbb, ccc, ddd, eee, X[ 5],  8);
   JJJ(eee, aaa, bbb, ccc, ddd, X[14],  9);
   JJJ(ddd, eee, aaa, bbb, ccc, X[ 7],  9);
   JJJ(ccc, ddd, eee, aaa, bbb, X[ 0], 11);
   JJJ(bbb, ccc, ddd, eee, aaa, X[ 9], 13);
   JJJ(aaa, bbb, ccc, ddd, eee, X[ 2], 15);
   JJJ(eee, aaa, bbb, ccc, ddd, X[11], 15);
   JJJ(ddd, eee, aaa, bbb, ccc, X[ 4],  5);
   JJJ(ccc, ddd, eee, aaa, bbb, X[13],  7);
   JJJ(bbb, ccc, ddd, eee, aaa, X[ 6],  7);
   JJJ(aaa, bbb, ccc, ddd, eee, X[15],  8);
   JJJ(eee, aaa, bbb, ccc, ddd, X[ 8], 11);
   JJJ(ddd, eee, aaa, bbb, ccc, X[ 1], 14);
   JJJ(ccc, ddd, eee, aaa, bbb, X[10], 14);
   JJJ(bbb, ccc, ddd, eee, aaa, X[ 3], 12);
   JJJ(aaa, bbb, ccc, ddd, eee, X[12],  6);

   /* parallel round 2 */
   III(eee, aaa, bbb, ccc, ddd, X[ 6],  9); 
   III(ddd, eee, aaa, bbb, ccc, X[11], 13);
   III(ccc, ddd, eee, aaa, bbb, X[ 3], 15);
   III(bbb, ccc, ddd, eee, aaa, X[ 7],  7);
   III(aaa, bbb, ccc, ddd, eee, X[ 0], 12);
   III(eee, aaa, bbb, ccc, ddd, X[13],  8);
   III(ddd, eee, aaa, bbb, ccc, X[ 5],  9);
   III(ccc, ddd, eee, aaa, bbb, X[10], 11);
   III(bbb, ccc, ddd, eee, aaa, X[14],  7);
   III(aaa, bbb, ccc, ddd, eee, X[15],  7);
   III(eee, aaa, bbb, ccc, ddd, X[ 8], 12);
   III(ddd, eee, aaa, bbb, ccc, X[12],  7);
   III(ccc, ddd, eee, aaa, bbb, X[ 4],  6);
   III(bbb, ccc, ddd, eee, aaa, X[ 9], 15);
   III(aaa, bbb, ccc, ddd, eee, X[ 1], 13);
   III(eee, aaa, bbb, ccc, ddd, X[ 2], 11);

   /* parallel round 3 */
   HHH(ddd, eee, aaa, bbb, ccc, X[15],  9);
   HHH(ccc, ddd, eee, aaa, bbb, X[ 5],  7);
   HHH(bbb, ccc, ddd, eee, aaa, X[ 1], 15);
   HHH(aaa, bbb, ccc, ddd, eee, X[ 3], 11);
   HHH(eee, aaa, bbb, ccc, ddd, X[ 7],  8);
   HHH(ddd, eee, aaa, bbb, ccc, X[14],  6);
   HHH(ccc, ddd, eee, aaa, bbb, X[ 6],  6);
   HHH(bbb, ccc, ddd, eee, aaa, X[ 9], 14);
   HHH(aaa, bbb, ccc, ddd, eee, X[11], 12);
   HHH(eee, aaa, bbb, ccc, ddd, X[ 8], 13);
   HHH(ddd, eee, aaa, bbb, ccc, X[12],  5);
   HHH(ccc, ddd, eee, aaa, bbb, X[ 2], 14);
   HHH(bbb, ccc, ddd, eee, aaa, X[10], 13);
   HHH(aaa, bbb, ccc, ddd, eee, X[ 0], 13);
   HHH(eee, aaa, bbb, ccc, ddd, X[ 4],  7);
   HHH(ddd, eee, aaa, bbb, ccc, X[13],  5);

   /* parallel round 4 */   
   GGG(ccc, ddd, eee, aaa, bbb, X[ 8], 15);
   GGG(bbb, ccc, ddd, eee, aaa, X[ 6],  5);
   GGG(aaa, bbb, ccc, ddd, eee, X[ 4],  8);
   GGG(eee, aaa, bbb, ccc, ddd, X[ 1], 11);
   GGG(ddd, eee, aaa, bbb, ccc, X[ 3], 14);
   GGG(ccc, ddd, eee, aaa, bbb, X[11], 14);
   GGG(bbb, ccc, ddd, eee, aaa, X[15],  6);
   GGG(aaa, bbb, ccc, ddd, eee, X[ 0], 14);
   GGG(eee, aaa, bbb, ccc, ddd, X[ 5],  6);
   GGG(ddd, eee, aaa, bbb, ccc, X[12],  9);
   GGG(ccc, ddd, eee, aaa, bbb, X[ 2], 12);
   GGG(bbb, ccc, ddd, eee, aaa, X[13],  9);
   GGG(aaa, bbb, ccc, ddd, eee, X[ 9], 12);
   GGG(eee, aaa, bbb, ccc, ddd, X[ 7],  5);
   GGG(ddd, eee, aaa, bbb, ccc, X[10], 15);
   GGG(ccc, ddd, eee, aaa, bbb, X[14],  8);

   /* parallel round 5 */
   FFF(bbb, ccc, ddd, eee, aaa, X[12] ,  8);
   FFF(aaa, bbb, ccc, ddd, eee, X[15] ,  5);
   FFF(eee, aaa, bbb, ccc, ddd, X[10] , 12);
   FFF(ddd, eee, aaa, bbb, ccc, X[ 4] ,  9);
   FFF(ccc, ddd, eee, aaa, bbb, X[ 1] , 12);
   FFF(bbb, ccc, ddd, eee, aaa, X[ 5] ,  5);
   FFF(aaa, bbb, ccc, ddd, eee, X[ 8] , 14);
   FFF(eee, aaa, bbb, ccc, ddd, X[ 7] ,  6);
   FFF(ddd, eee, aaa, bbb, ccc, X[ 6] ,  8);
   FFF(ccc, ddd, eee, aaa, bbb, X[ 2] , 13);
   FFF(bbb, ccc, ddd, eee, aaa, X[13] ,  6);
   FFF(aaa, bbb, ccc, ddd, eee, X[14] ,  5);
   FFF(eee, aaa, bbb, ccc, ddd, X[ 0] , 15);
   FFF(ddd, eee, aaa, bbb, ccc, X[ 3] , 13);
   FFF(ccc, ddd, eee, aaa, bbb, X[ 9] , 11);
   FFF(bbb, ccc, ddd, eee, aaa, X[11] , 11);

   /* combine results */
   ddd += cc + MDbuf[1];               /* final result for MDbuf[0] */
   MDbuf[1] = MDbuf[2] + dd + eee;
   MDbuf[2] = MDbuf[3] + ee + aaa;
   MDbuf[3] = MDbuf[4] + aa + bbb;
   MDbuf[4] = MDbuf[0] + bb + ccc;
   MDbuf[0] = ddd;

   return;
}
//===============计算rmd-160  End==================//

//===============计算SHA-256Begin==================//
#ifndef uint32_t
#define uint32_t unsigned int
#endif

#define uint unsigned int
#define uchar unsigned char

#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19


uint rotr(uint x, int n) {
  if (n < 32) return (x >> n) | (x << (32 - n));
  return x;
}

uint ch(uint x, uint y, uint z) {
  return (x & y) ^ (~x & z);
}

uint maj(uint x, uint y, uint z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

uint sigma0(uint x) {
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint sigma1(uint x) {
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint gamma0(uint x) {
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint gamma1(uint x) {
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

//__kernel void sha256_crypt_kernel(__global uint *data_info,__global char *plain_key,  __global uint *digest)
void sha256(unsigned char *plain_key, unsigned int ulen , unsigned int *digest)
{
  int t, gid, msg_pad;
  int stop, mmod;
  uint i, item, total;
  uint W[80], temp, A,B,C,D,E,F,G,H,T1,T2;
  int current_pad;

  uint K[64]={
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

  msg_pad=0;

  total = ulen%64>=56?2:1 + ulen/64;

//  printf("ulen: %u total:%u\n", ulen, total);

  digest[0] = H0;
  digest[1] = H1;
  digest[2] = H2;
  digest[3] = H3;
  digest[4] = H4;
  digest[5] = H5;
  digest[6] = H6;
  digest[7] = H7;
  for(item=0; item<total; item++)
  {

    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];
    F = digest[5];
    G = digest[6];
    H = digest[7];

#pragma unroll
    for (t = 0; t < 80; t++){
    W[t] = 0x00000000;
    }
    msg_pad=item*64;
    if(ulen > msg_pad)
    {
      current_pad = (ulen-msg_pad)>64?64:(ulen-msg_pad);
    }
    else
    {
      current_pad =-1;    
    }

  //  printf("current_pad: %d\n",current_pad);
    if(current_pad>0)
    {
      i=current_pad;

      stop =  i/4;
  //    printf("i:%d, stop: %d msg_pad:%d\n",i,stop, msg_pad);
      for (t = 0 ; t < stop ; t++){
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
        W[t] |= (uchar)  plain_key[msg_pad + t * 4 + 3];
        //printf("W[%u]: %u\n",t,W[t]);
      }
      mmod = i % 4;
      if ( mmod == 3){
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
        W[t] |=  ((uchar) 0x80) ;
      } else if (mmod == 2) {
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |=  0x8000 ;
      } else if (mmod == 1) {
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |=  0x800000 ;
      } else /*if (mmod == 0)*/ {
        W[t] =  0x80000000 ;
      }
      
      if (current_pad<56)
      {
        W[15] =  ulen*8 ;
        //printf("ulen avlue 2 :w[15] :%u\n", W[15]);
      }
    }
    else if(current_pad <0)
    {
      if( ulen%64==0)
        W[0]=0x80000000;
      W[15]=ulen*8;
      //printf("ulen avlue 3 :w[15] :%u\n", W[15]);
    }

    for (t = 0; t < 64; t++) {
      if (t >= 16)
        W[t] = gamma1(W[t - 2]) + W[t - 7] + gamma0(W[t - 15]) + W[t - 16];
      T1 = H + sigma1(E) + ch(E, F, G) + K[t] + W[t];
      T2 = sigma0(A) + maj(A, B, C);
      H = G; G = F; F = E; E = D + T1; D = C; C = B; B = A; A = T1 + T2;
    }
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
    digest[5] += F;
    digest[6] += G;
    digest[7] += H;

  //  for (t = 0; t < 80; t++)
  //    {
  //    printf("W[%d]: %u\n",t,W[t]);
  //    }
  }

  unsigned char * Tmp = digest;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 1;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 2;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 3;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 4;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 5;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 6;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];

  Tmp = digest + 7;
  Tmp[0] = Tmp[0] ^ Tmp[3]; Tmp[3] = Tmp[0] ^ Tmp[3]; Tmp[0] = Tmp[0] ^ Tmp[3];
  Tmp[1] = Tmp[1] ^ Tmp[2]; Tmp[2] = Tmp[1] ^ Tmp[2]; Tmp[1] = Tmp[1] ^ Tmp[2];
}
//===============计算SHA-256  End==================//


//===============在数据集合中搜索目标钱包Begin==================//
int MEMCMP2(unsigned char * pWalletKey, unsigned char * pWallet, unsigned int iLength )
{
	for( unsigned int i=0; i<iLength; i++ )
	{
		if( *pWalletKey > *pWallet )
			return 1;
		else if( *pWalletKey < *pWallet )
			return -1;
		else
		{
			pWalletKey ++;
			pWallet ++;
			continue;
		}
	}

	return 0;
}

//查找钱包
int wallet_bin_find(unsigned char * pWallet, unsigned char * pWalletAll, unsigned int iCount)
{
	unsigned int iLow  = 0;             //数组最小索引值
	unsigned int iHigh = iCount - 1;    //数组最大索引值

	unsigned int iMiddle = 0;
	while( iLow<=iHigh )
	{
		//printf("-->%d--%d--\r\n" , iLow , iHigh);
		iMiddle = ( iLow + iHigh ) / 2;
		if( 0 == MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			return 0;
		else if( 0 < MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			iLow = iMiddle + 1;
		else if( 0 > MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			iHigh = iMiddle - 1;
	}

	return -1; //iLow>iHigh的情况，这种情况下key的值大于arr中最大的元素值或者key的值小于arr中最小的元素值
}
//===============在数据集合中搜索目标钱包  End==================//

void ProcPrivateAddInt(unsigned char *pPrivateDest, unsigned char * szPrivateKey, unsigned int iNum)
{
	unsigned char iFirst  = iNum & 0xff; iNum = iNum >> 8;
	unsigned char iSecond = iNum & 0xff; iNum = iNum >> 8;
	unsigned char iThird  = iNum & 0xff; iNum = iNum >> 8;
	unsigned char iFourth = iNum & 0xff; iNum = iNum >> 8;

	//前28位原样不动
	unsigned int iTmp = szPrivateKey[31] + iFirst;    pPrivateDest[31] = szPrivateKey[31] + iFirst;
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[30] + (unsigned char)iSecond + 1; pPrivateDest[30] = szPrivateKey[30] + (unsigned char)iSecond + 1;
	}
	else
	{
		iTmp = szPrivateKey[30] + (unsigned char)iSecond; pPrivateDest[30] = szPrivateKey[30] + (unsigned char)iSecond;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[29] + iThird + 1; pPrivateDest[29] = szPrivateKey[29] + iThird + 1;
	}
	else
	{
		iTmp = szPrivateKey[29] + iThird; pPrivateDest[29] = szPrivateKey[29] + iThird;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[28] + iFourth + 1; pPrivateDest[28] = szPrivateKey[28] + iFourth + 1;
	}
	else
	{
		iTmp = szPrivateKey[28] + iFourth; pPrivateDest[28] = szPrivateKey[28] + iFourth;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[27] + 1; pPrivateDest[27] = szPrivateKey[27] + 1;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[26] + 1; pPrivateDest[26] = szPrivateKey[26] + 1;
	}

	pPrivateDest[27] = szPrivateKey[27];
	pPrivateDest[26] = szPrivateKey[26];
	pPrivateDest[25] = szPrivateKey[25];
	pPrivateDest[24] = szPrivateKey[24];
	pPrivateDest[23] = szPrivateKey[23];
	pPrivateDest[22] = szPrivateKey[22];
	pPrivateDest[21] = szPrivateKey[21];
	pPrivateDest[20] = szPrivateKey[20];
	pPrivateDest[19] = szPrivateKey[19];
	pPrivateDest[18] = szPrivateKey[18];
	pPrivateDest[17] = szPrivateKey[17];
	pPrivateDest[16] = szPrivateKey[16];
	pPrivateDest[15] = szPrivateKey[15];
	pPrivateDest[14] = szPrivateKey[14];
	pPrivateDest[13] = szPrivateKey[13];
	pPrivateDest[12] = szPrivateKey[12];
	pPrivateDest[11] = szPrivateKey[11];
	pPrivateDest[10] = szPrivateKey[10];
	pPrivateDest[ 9] = szPrivateKey[ 9];
	pPrivateDest[ 8] = szPrivateKey[ 8];
	pPrivateDest[ 7] = szPrivateKey[ 7];
	pPrivateDest[ 6] = szPrivateKey[ 6];
	pPrivateDest[ 5] = szPrivateKey[ 5];
	pPrivateDest[ 4] = szPrivateKey[ 4];
	pPrivateDest[ 3] = szPrivateKey[ 3];
	pPrivateDest[ 2] = szPrivateKey[ 2];
	pPrivateDest[ 1] = szPrivateKey[ 1];
	pPrivateDest[ 0] = szPrivateKey[ 0];
}

__kernel void main(__global unsigned char *pPublicKey,
                   __global unsigned char *pPrivateKey,
                   __global unsigned int  *iCalcCount,
                   __global unsigned char *pWalletData,
                   __global unsigned int  *iWalletCount,
                   __global unsigned char *pOutput,
                   __global unsigned int  *pOutputCount)
{
    //线程ID
    unsigned int idx = (unsigned int) get_global_id(0);
    //65535个线程应该足够使用了
    unsigned int iIndex = idx & 0xFFFF;

    //目标数据中的所有钱包地址
    unsigned char * pWalletAll = (unsigned char *)pWalletData;
    //目标数据中的钱包数量
    //unsigned int iWalletCount  = *pWalletCount;

    //总的计算数量
    unsigned int iPrivateCount = *iCalcCount;
    //记录最大的输出数量
    unsigned int iOutCountMax  = pOutputCount;
//printf("--%0x--->%0x\r\n" , iPrivateCount, *iWalletCount);
    //常量G点定义
    secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
        0x79BE667EUL, 0xF9DCBBACUL, 0x55A06295UL, 0xCE870B07UL,
        0x029BFCDBUL, 0x2DCE28D9UL, 0x59F2815BUL, 0x16F81798UL,
        0x483ADA77UL, 0x26A3C465UL, 0x5DA4FBFCUL, 0x0E1108A8UL,
        0xFD17B448UL, 0xA6855419UL, 0x9C47D08FUL, 0xFB10D4B8UL
    );

    //私钥的开始位置
    unsigned char * szPrivateKey = (unsigned char *)(pPrivateKey + iIndex * 32 );
    //公钥的开始位置
    unsigned char * szPublicKey  = (unsigned char *)(pPublicKey  + iIndex * 66 );
    //unsigned char szPublicKey[66] = {
    //CPU 字节序
    //0x4d, 0xd2, 0x58, 0xcc, 0x3e, 0x05, 0x0b, 0x57, 0x02, 0x99, 0xef, 0x45, 0xde, 0x5d, 0x96, 0xe5, 0x24, 0x05, 0x10, 0x96, 0xa2, 0xa9, 0xae, 0x52, 0xd2, 0x2b, 0xa8, 0x92, 0x7b, 0x16, 0x7f, 0xce,
    //0xf2, 0x97, 0xf3, 0x5a, 0x0d, 0xe8, 0xb7, 0xc5, 0x78, 0x92, 0x64, 0xd2, 0xde, 0x85, 0x8d, 0xc8, 0x58, 0x2c, 0x39, 0x36, 0x8c, 0x39, 0x9f, 0xd9, 0x1d, 0xc5, 0xa9, 0x2c, 0x33, 0xd8, 0x5a, 0xa1, 0x0};

    uint32_t iPublic0 = ((szPublicKey[ 0] << 24) & 0xFF000000) | ((szPublicKey[ 1] << 16) & 0x00FF0000) | ((szPublicKey[ 2] << 8) & 0x0000FF00) | (szPublicKey[ 3] & 0xFF);
    uint32_t iPublic1 = ((szPublicKey[ 4] << 24) & 0xFF000000) | ((szPublicKey[ 5] << 16) & 0x00FF0000) | ((szPublicKey[ 6] << 8) & 0x0000FF00) | (szPublicKey[ 7] & 0xFF);
    uint32_t iPublic2 = ((szPublicKey[ 8] << 24) & 0xFF000000) | ((szPublicKey[ 9] << 16) & 0x00FF0000) | ((szPublicKey[10] << 8) & 0x0000FF00) | (szPublicKey[11] & 0xFF);
    uint32_t iPublic3 = ((szPublicKey[12] << 24) & 0xFF000000) | ((szPublicKey[13] << 16) & 0x00FF0000) | ((szPublicKey[14] << 8) & 0x0000FF00) | (szPublicKey[15] & 0xFF);
    uint32_t iPublic4 = ((szPublicKey[16] << 24) & 0xFF000000) | ((szPublicKey[17] << 16) & 0x00FF0000) | ((szPublicKey[18] << 8) & 0x0000FF00) | (szPublicKey[19] & 0xFF);
    uint32_t iPublic5 = ((szPublicKey[20] << 24) & 0xFF000000) | ((szPublicKey[21] << 16) & 0x00FF0000) | ((szPublicKey[22] << 8) & 0x0000FF00) | (szPublicKey[23] & 0xFF);
    uint32_t iPublic6 = ((szPublicKey[24] << 24) & 0xFF000000) | ((szPublicKey[25] << 16) & 0x00FF0000) | ((szPublicKey[26] << 8) & 0x0000FF00) | (szPublicKey[27] & 0xFF);
    uint32_t iPublic7 = ((szPublicKey[28] << 24) & 0xFF000000) | ((szPublicKey[29] << 16) & 0x00FF0000) | ((szPublicKey[30] << 8) & 0x0000FF00) | (szPublicKey[31] & 0xFF);
    uint32_t iPublic8 = ((szPublicKey[32] << 24) & 0xFF000000) | ((szPublicKey[33] << 16) & 0x00FF0000) | ((szPublicKey[34] << 8) & 0x0000FF00) | (szPublicKey[35] & 0xFF);
    uint32_t iPublic9 = ((szPublicKey[36] << 24) & 0xFF000000) | ((szPublicKey[37] << 16) & 0x00FF0000) | ((szPublicKey[38] << 8) & 0x0000FF00) | (szPublicKey[39] & 0xFF);
    uint32_t iPublica = ((szPublicKey[40] << 24) & 0xFF000000) | ((szPublicKey[41] << 16) & 0x00FF0000) | ((szPublicKey[42] << 8) & 0x0000FF00) | (szPublicKey[43] & 0xFF);
    uint32_t iPublicb = ((szPublicKey[44] << 24) & 0xFF000000) | ((szPublicKey[45] << 16) & 0x00FF0000) | ((szPublicKey[46] << 8) & 0x0000FF00) | (szPublicKey[47] & 0xFF);
    uint32_t iPublicc = ((szPublicKey[48] << 24) & 0xFF000000) | ((szPublicKey[49] << 16) & 0x00FF0000) | ((szPublicKey[50] << 8) & 0x0000FF00) | (szPublicKey[51] & 0xFF);
    uint32_t iPublicd = ((szPublicKey[52] << 24) & 0xFF000000) | ((szPublicKey[53] << 16) & 0x00FF0000) | ((szPublicKey[54] << 8) & 0x0000FF00) | (szPublicKey[55] & 0xFF);
    uint32_t iPublice = ((szPublicKey[56] << 24) & 0xFF000000) | ((szPublicKey[57] << 16) & 0x00FF0000) | ((szPublicKey[58] << 8) & 0x0000FF00) | (szPublicKey[59] & 0xFF);
    uint32_t iPublicf = ((szPublicKey[60] << 24) & 0xFF000000) | ((szPublicKey[61] << 16) & 0x00FF0000) | ((szPublicKey[62] << 8) & 0x0000FF00) | (szPublicKey[63] & 0xFF);
    //获取第一个公钥
    secp256k1_ge secp256k1_Public = SECP256K1_GE_CONST(iPublic0, iPublic1, iPublic2, iPublic3, iPublic4, iPublic5, iPublic6, iPublic7, iPublic8, iPublic9, iPublica, iPublicb, iPublicc, iPublicd, iPublice, iPublicf);
    //公钥
    secp256k1_gej oPublicKey;
    secp256k1_gej_set_ge(&oPublicKey, &secp256k1_Public);

    //用于SHA256加密的公钥字符串
    unsigned char szPublicForSha256[66] = { 0 };
    //sha256的输出
    unsigned char szSHA256[33] = { 0 };
    //RMD160的输出
    unsigned char szRMD160[21] = { 0 };

    //循环处理每一个公钥
    for (unsigned int i=0; i < iPrivateCount; i++)
    {
        //将目标公钥转化为字符串
        secp256k1_ge ge;
        secp256k1_ge_set_gej(&ge, &oPublicKey);

        //将产生的公钥转化为字串
        secp256k1_ge_to_buff((szPublicForSha256 + 1), &ge);
        //开头固定为04，标识非压缩形式
        szPublicForSha256[0] = 0x04;

//unsigned char * pOutX = NULL;
//打印每一步的私钥
//unsigned char pPrivateX[33] = { 0 };
//ProcPrivateAddInt(pPrivateX, szPrivateKey, i);
//pOutX = (unsigned char *)pPrivateX;;
//printf("-GPU->");
//for (unsigned int i = 0; i < 32; i++)
//    printf("%02x ", pOutX[i]);
//printf("->");

//打印每一步的公钥
//pOutX = (unsigned char *)szPublicForSha256 + 1;
//printf("-GPU->");
//for (unsigned int i = 0; i < 65; i++)
//    printf("%02x ", pOutX[i]);
//printf("->");

        //计算公钥的SHA265
        sha256(szPublicForSha256, 65, szSHA256);

//打印公钥的Hash值
//pOutX = szSHA256;
//for (unsigned int j = 0; j < 32; j++)
//    printf("%02x ", pOutX[j]);
//printf("->");

        //计算RMD160
        rmd160(szRMD160, szSHA256);

//打印Hash后的RMD160
//pOutX = (unsigned char *)szRMD160;
//for (unsigned int k = 0; k < 20; k++)
//    printf("%02x ", pOutX[k]);
//printf("\r\n");

        //查找计算结果是否OK
        int bResult = wallet_bin_find(szRMD160, pWalletAll, *iWalletCount);
        if (0 == bResult)
        {
            //找到填写的位置
            unsigned char * pDstPrivate = ( unsigned char * )( pOutput + 32 * (*pOutputCount) );
            //在原有私钥的基础上，添加偏移值，然后写入目标地址
            ProcPrivateAddInt( pDstPrivate , szPrivateKey , i );

            //返回目标数量加一
            (*pOutputCount) ++;

			printf("==>wallet_bin_find==%d\r\n" , bResult);
        }

        //使用当前公钥，计算下一个公钥
        secp256k1_gej_add_ge(&oPublicKey, &oPublicKey, &secp256k1_ge_const_g);
    }
}
