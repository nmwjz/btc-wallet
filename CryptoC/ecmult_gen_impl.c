
#ifndef SECP256K1_ECMULT_GEN_IMPL_H
#define SECP256K1_ECMULT_GEN_IMPL_H

#include "ecmult_gen_impl.h"



/* Limbs of the secp256k1 order. */
#define SECP256K1_N_0 ((uint32_t)0xD0364141UL)
#define SECP256K1_N_1 ((uint32_t)0xBFD25E8CUL)
#define SECP256K1_N_2 ((uint32_t)0xAF48A03BUL)
#define SECP256K1_N_3 ((uint32_t)0xBAAEDCE6UL)
#define SECP256K1_N_4 ((uint32_t)0xFFFFFFFEUL)
#define SECP256K1_N_5 ((uint32_t)0xFFFFFFFFUL)
#define SECP256K1_N_6 ((uint32_t)0xFFFFFFFFUL)
#define SECP256K1_N_7 ((uint32_t)0xFFFFFFFFUL)

/* Limbs of 2^256 minus the secp256k1 order. */
#define SECP256K1_N_C_0 (~SECP256K1_N_0 + 1)
#define SECP256K1_N_C_1 (~SECP256K1_N_1)
#define SECP256K1_N_C_2 (~SECP256K1_N_2)
#define SECP256K1_N_C_3 (~SECP256K1_N_3)
#define SECP256K1_N_C_4 (1)

/* Limbs of half the secp256k1 order. */
#define SECP256K1_N_H_0 ((uint32_t)0x681B20A0UL)
#define SECP256K1_N_H_1 ((uint32_t)0xDFE92F46UL)
#define SECP256K1_N_H_2 ((uint32_t)0x57A4501DUL)
#define SECP256K1_N_H_3 ((uint32_t)0x5D576E73UL)
#define SECP256K1_N_H_4 ((uint32_t)0xFFFFFFFFUL)
#define SECP256K1_N_H_5 ((uint32_t)0xFFFFFFFFUL)
#define SECP256K1_N_H_6 ((uint32_t)0xFFFFFFFFUL)
#define SECP256K1_N_H_7 ((uint32_t)0x7FFFFFFFUL)

SECP256K1_INLINE static void secp256k1_scalar_clear(secp256k1_scalar *r)
{
    r->d[0] = 0;
    r->d[1] = 0;
    r->d[2] = 0;
    r->d[3] = 0;
    r->d[4] = 0;
    r->d[5] = 0;
    r->d[6] = 0;
    r->d[7] = 0;
}

SECP256K1_INLINE static void secp256k1_scalar_set_int(secp256k1_scalar *r, unsigned int v)
{
    r->d[0] = v;
    r->d[1] = 0;
    r->d[2] = 0;
    r->d[3] = 0;
    r->d[4] = 0;
    r->d[5] = 0;
    r->d[6] = 0;
    r->d[7] = 0;
}

SECP256K1_INLINE static unsigned int secp256k1_scalar_get_bits(const secp256k1_scalar *a, unsigned int offset, unsigned int count)
{
    return (a->d[offset >> 5] >> (offset & 0x1F)) & ((1 << count) - 1);
}

SECP256K1_INLINE static int secp256k1_scalar_check_overflow(const secp256k1_scalar *a)
{
    int yes = 0;
    int no = 0;
    no  |= (a->d[7] < SECP256K1_N_7); /* No need for a > check. */
    no  |= (a->d[6] < SECP256K1_N_6); /* No need for a > check. */
    no  |= (a->d[5] < SECP256K1_N_5); /* No need for a > check. */
    no  |= (a->d[4] < SECP256K1_N_4);
    yes |= (a->d[4] > SECP256K1_N_4) & ~no;
    no  |= (a->d[3] < SECP256K1_N_3) & ~yes;
    yes |= (a->d[3] > SECP256K1_N_3) & ~no;
    no  |= (a->d[2] < SECP256K1_N_2) & ~yes;
    yes |= (a->d[2] > SECP256K1_N_2) & ~no;
    no  |= (a->d[1] < SECP256K1_N_1) & ~yes;
    yes |= (a->d[1] > SECP256K1_N_1) & ~no;
    yes |= (a->d[0] >= SECP256K1_N_0) & ~no;
    return yes;
}

SECP256K1_INLINE static int secp256k1_scalar_reduce(secp256k1_scalar *r, uint32_t overflow)
{
    uint64_t t;

    t  = (uint64_t)r->d[0] + overflow * SECP256K1_N_C_0;    r->d[0] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[1] + overflow * SECP256K1_N_C_1;    r->d[1] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[2] + overflow * SECP256K1_N_C_2;    r->d[2] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[3] + overflow * SECP256K1_N_C_3;    r->d[3] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[4] + overflow * SECP256K1_N_C_4;    r->d[4] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[5];                                 r->d[5] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[6];                                 r->d[6] = t & 0xFFFFFFFFUL; t >>= 32;
	t += (uint64_t)r->d[7];                                 r->d[7] = t & 0xFFFFFFFFUL;
    return overflow;
}

SECP256K1_INLINE static int secp256k1_scalar_add(secp256k1_scalar *r, const secp256k1_scalar *a, const secp256k1_scalar *b)
{
    int overflow;
    uint64_t t = (uint64_t)a->d[0] + b->d[0];
    r->d[0] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[1] + b->d[1];
    r->d[1] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[2] + b->d[2];
    r->d[2] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[3] + b->d[3];
    r->d[3] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[4] + b->d[4];
    r->d[4] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[5] + b->d[5];
    r->d[5] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[6] + b->d[6];
    r->d[6] = t & 0xFFFFFFFFULL; t >>= 32;    t += (uint64_t)a->d[7] + b->d[7];
    r->d[7] = t & 0xFFFFFFFFULL; t >>= 32;
    overflow = t + secp256k1_scalar_check_overflow(r);

    secp256k1_scalar_reduce(r, overflow);
    return overflow;
}

SECP256K1_INLINE static void secp256k1_scalar_set_b32(secp256k1_scalar *r, const unsigned char *b32, int *overflow)
{
    int over;
    r->d[0] = (uint32_t)b32[31] | (uint32_t)b32[30] << 8 | (uint32_t)b32[29] << 16 | (uint32_t)b32[28] << 24;
    r->d[1] = (uint32_t)b32[27] | (uint32_t)b32[26] << 8 | (uint32_t)b32[25] << 16 | (uint32_t)b32[24] << 24;
    r->d[2] = (uint32_t)b32[23] | (uint32_t)b32[22] << 8 | (uint32_t)b32[21] << 16 | (uint32_t)b32[20] << 24;
    r->d[3] = (uint32_t)b32[19] | (uint32_t)b32[18] << 8 | (uint32_t)b32[17] << 16 | (uint32_t)b32[16] << 24;
    r->d[4] = (uint32_t)b32[15] | (uint32_t)b32[14] << 8 | (uint32_t)b32[13] << 16 | (uint32_t)b32[12] << 24;
    r->d[5] = (uint32_t)b32[11] | (uint32_t)b32[10] << 8 | (uint32_t)b32[9] << 16 | (uint32_t)b32[8] << 24;
    r->d[6] = (uint32_t)b32[7] | (uint32_t)b32[6] << 8 | (uint32_t)b32[5] << 16 | (uint32_t)b32[4] << 24;
    r->d[7] = (uint32_t)b32[3] | (uint32_t)b32[2] << 8 | (uint32_t)b32[1] << 16 | (uint32_t)b32[0] << 24;
    over = secp256k1_scalar_reduce(r, secp256k1_scalar_check_overflow(r));
    if (overflow)
    {
        *overflow = over;
    }
}

SECP256K1_INLINE static int secp256k1_scalar_is_zero(const secp256k1_scalar *a)
{
    return (a->d[0] | a->d[1] | a->d[2] | a->d[3] | a->d[4] | a->d[5] | a->d[6] | a->d[7]) == 0;
}

SECP256K1_INLINE static void secp256k1_scalar_negate(secp256k1_scalar *r, const secp256k1_scalar *a)
{
    uint32_t nonzero = 0xFFFFFFFFUL * (secp256k1_scalar_is_zero(a) == 0);
	uint64_t t = 0;
	t  = (uint64_t)(~a->d[0]) + SECP256K1_N_0 + 1;    r->d[0] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[1]) + SECP256K1_N_1;        r->d[1] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[2]) + SECP256K1_N_2;        r->d[2] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[3]) + SECP256K1_N_3;        r->d[3] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[4]) + SECP256K1_N_4;        r->d[4] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[5]) + SECP256K1_N_5;        r->d[5] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[6]) + SECP256K1_N_6;        r->d[6] = t & nonzero; t >>= 32;
	t += (uint64_t)(~a->d[7]) + SECP256K1_N_7;        r->d[7] = t & nonzero;
}


//===================================================//

static void secp256k1_fe_normalize(secp256k1_fe *r)
{
    uint32_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4],
             t5 = r->n[5], t6 = r->n[6], t7 = r->n[7], t8 = r->n[8], t9 = r->n[9];

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    uint32_t m;
    uint32_t x = t9 >> 22; t9 &= 0x03FFFFFUL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x3D1UL; t1 += (x << 6);
    t1 += (t0 >> 26); t0 &= 0x3FFFFFFUL;
    t2 += (t1 >> 26); t1 &= 0x3FFFFFFUL;
    t3 += (t2 >> 26); t2 &= 0x3FFFFFFUL; m = t2;
    t4 += (t3 >> 26); t3 &= 0x3FFFFFFUL; m &= t3;
    t5 += (t4 >> 26); t4 &= 0x3FFFFFFUL; m &= t4;
    t6 += (t5 >> 26); t5 &= 0x3FFFFFFUL; m &= t5;
    t7 += (t6 >> 26); t6 &= 0x3FFFFFFUL; m &= t6;
    t8 += (t7 >> 26); t7 &= 0x3FFFFFFUL; m &= t7;
    t9 += (t8 >> 26); t8 &= 0x3FFFFFFUL; m &= t8;

    /* At most a single final reduction is needed; check if the value is >= the field characteristic */
    x = (t9 >> 22) | ((t9 == 0x03FFFFFUL) & (m == 0x3FFFFFFUL) & ((t1 + 0x40UL + ((t0 + 0x3D1UL) >> 26)) > 0x3FFFFFFUL));

    /* Apply the final reduction (for constant-time behaviour, we do it always) */
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

    /* Mask off the possible multiple of 2^256 from the final reduction */
    t9 &= 0x03FFFFFUL;

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;
    r->n[5] = t5; r->n[6] = t6; r->n[7] = t7; r->n[8] = t8; r->n[9] = t9;
}

static void secp256k1_fe_normalize_weak(secp256k1_fe *r)
{
    uint32_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4],
             t5 = r->n[5], t6 = r->n[6], t7 = r->n[7], t8 = r->n[8], t9 = r->n[9];

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    uint32_t x = t9 >> 22; t9 &= 0x03FFFFFUL;

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

static void secp256k1_fe_normalize_var(secp256k1_fe *r)
{
    uint32_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4],
             t5 = r->n[5], t6 = r->n[6], t7 = r->n[7], t8 = r->n[8], t9 = r->n[9];

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    uint32_t m;
    uint32_t x = t9 >> 22; t9 &= 0x03FFFFFUL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x3D1UL; t1 += (x << 6);
    t1 += (t0 >> 26); t0 &= 0x3FFFFFFUL;
    t2 += (t1 >> 26); t1 &= 0x3FFFFFFUL;
    t3 += (t2 >> 26); t2 &= 0x3FFFFFFUL; m = t2;
    t4 += (t3 >> 26); t3 &= 0x3FFFFFFUL; m &= t3;
    t5 += (t4 >> 26); t4 &= 0x3FFFFFFUL; m &= t4;
    t6 += (t5 >> 26); t5 &= 0x3FFFFFFUL; m &= t5;
    t7 += (t6 >> 26); t6 &= 0x3FFFFFFUL; m &= t6;
    t8 += (t7 >> 26); t7 &= 0x3FFFFFFUL; m &= t7;
    t9 += (t8 >> 26); t8 &= 0x3FFFFFFUL; m &= t8;

    /* At most a single final reduction is needed; check if the value is >= the field characteristic */
    x = (t9 >> 22) | ((t9 == 0x03FFFFFUL) & (m == 0x3FFFFFFUL) & ((t1 + 0x40UL + ((t0 + 0x3D1UL) >> 26)) > 0x3FFFFFFUL));

    if (x)
    {
        t0 += 0x3D1UL; t1 += (x << 6);
        t1 += (t0 >> 26); t0 &= 0x3FFFFFFUL;
        t2 += (t1 >> 26); t1 &= 0x3FFFFFFUL;
        t3 += (t2 >> 26); t2 &= 0x3FFFFFFUL;
        t4 += (t3 >> 26); t3 &= 0x3FFFFFFUL;
        t5 += (t4 >> 26); t4 &= 0x3FFFFFFUL;
        t6 += (t5 >> 26); t5 &= 0x3FFFFFFUL;
        t7 += (t6 >> 26); t6 &= 0x3FFFFFFUL;
        t8 += (t7 >> 26); t7 &= 0x3FFFFFFUL;
        t9 += (t8 >> 26); t8 &= 0x3FFFFFFUL;

        /* Mask off the possible multiple of 2^256 from the final reduction */
        t9 &= 0x03FFFFFUL;
    }

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;
    r->n[5] = t5; r->n[6] = t6; r->n[7] = t7; r->n[8] = t8; r->n[9] = t9;
}

static int secp256k1_fe_normalizes_to_zero(secp256k1_fe *r)
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

static int secp256k1_fe_normalizes_to_zero_var(secp256k1_fe *r)
{
    uint32_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
    uint32_t z0, z1;
    uint32_t x;

    t0 = r->n[0];
    t9 = r->n[9];

    /* Reduce t9 at the start so there will be at most a single carry from the first pass */
    x = t9 >> 22;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x3D1UL;

    /* z0 tracks a possible raw value of 0, z1 tracks a possible raw value of P */
    z0 = t0 & 0x3FFFFFFUL;
    z1 = z0 ^ 0x3D0UL;

    /* Fast return path should catch the majority of cases */
    if ((z0 != 0UL) & (z1 != 0x3FFFFFFUL)) {
        return 0;
    }

    t1 = r->n[1];
    t2 = r->n[2];
    t3 = r->n[3];
    t4 = r->n[4];
    t5 = r->n[5];
    t6 = r->n[6];
    t7 = r->n[7];
    t8 = r->n[8];

    t9 &= 0x03FFFFFUL;
    t1 += (x << 6);

    t1 += (t0 >> 26);
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

SECP256K1_INLINE static void secp256k1_fe_set_int(secp256k1_fe *r, int a)
{
    r->n[0] = a;
    r->n[1] = r->n[2] = r->n[3] = r->n[4] = r->n[5] = r->n[6] = r->n[7] = r->n[8] = r->n[9] = 0;
}

SECP256K1_INLINE static int secp256k1_fe_is_zero(const secp256k1_fe *a)
{
    const uint32_t *t = a->n;
    return (t[0] | t[1] | t[2] | t[3] | t[4] | t[5] | t[6] | t[7] | t[8] | t[9]) == 0;
}

SECP256K1_INLINE static int secp256k1_fe_is_odd(const secp256k1_fe *a)
{
    return a->n[0] & 1;
}

SECP256K1_INLINE static void secp256k1_fe_clear(secp256k1_fe *a)
{
	a->n[0] = 0;
	a->n[1] = 0;
	a->n[2] = 0;
	a->n[3] = 0;
	a->n[4] = 0;
	a->n[5] = 0;
	a->n[6] = 0;
	a->n[7] = 0;
	a->n[8] = 0;
	a->n[9] = 0;
}

static int secp256k1_fe_set_b32(secp256k1_fe *r, const unsigned char *a)
{
    r->n[0] = (uint32_t)(a[31]) | ((uint32_t)a[30] << 8) | ((uint32_t)a[29] << 16) | ((uint32_t)(a[28] & 0x3) << 24);
    r->n[1] = (uint32_t)((a[28] >> 2) & 0x3f) | ((uint32_t)a[27] << 6) | ((uint32_t)a[26] << 14) | ((uint32_t)(a[25] & 0xf) << 22);
    r->n[2] = (uint32_t)((a[25] >> 4) & 0xf) | ((uint32_t)a[24] << 4) | ((uint32_t)a[23] << 12) | ((uint32_t)(a[22] & 0x3f) << 20);
    r->n[3] = (uint32_t)((a[22] >> 6) & 0x3) | ((uint32_t)a[21] << 2) | ((uint32_t)a[20] << 10) | ((uint32_t)(a[19]) << 18);
    r->n[4] = (uint32_t)(a[18]) | ((uint32_t)a[17] << 8) | ((uint32_t)a[16] << 16) | ((uint32_t)(a[15] & 0x3) << 24);
    r->n[5] = (uint32_t)((a[15] >> 2) & 0x3f) | ((uint32_t)a[14] << 6) | ((uint32_t)a[13] << 14) | ((uint32_t)(a[12] & 0xf) << 22);
    r->n[6] = (uint32_t)((a[12] >> 4) & 0xf) | ((uint32_t)a[11] << 4) | ((uint32_t)a[10] << 12) | ((uint32_t)(a[9] & 0x3f) << 20);
    r->n[7] = (uint32_t)((a[9] >> 6) & 0x3) | ((uint32_t)a[8] << 2) | ((uint32_t)a[7] << 10) | ((uint32_t)(a[6]) << 18);
    r->n[8] = (uint32_t)(a[5]) | ((uint32_t)a[4] << 8) | ((uint32_t)a[3] << 16) | ((uint32_t)(a[2] & 0x3) << 24);
    r->n[9] = (uint32_t)((a[2] >> 2) & 0x3f) | ((uint32_t)a[1] << 6) | ((uint32_t)a[0] << 14);

    if (r->n[9] == 0x3FFFFFUL && (r->n[8] & r->n[7] & r->n[6] & r->n[5] & r->n[4] & r->n[3] & r->n[2]) == 0x3FFFFFFUL && (r->n[1] + 0x40UL + ((r->n[0] + 0x3D1UL) >> 26)) > 0x3FFFFFFUL)
    {
        return 0;
    }
    return 1;
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

SECP256K1_INLINE static void secp256k1_fe_mul_inner(uint32_t *r, const uint32_t *a, const uint32_t * b)
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

SECP256K1_INLINE static void secp256k1_fe_sqr_inner(uint32_t *r, const uint32_t *a)
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
}

static void secp256k1_fe_mul(secp256k1_fe *r, const secp256k1_fe *a, const secp256k1_fe * b)
{
    secp256k1_fe_mul_inner(r->n, a->n, b->n);
}

static void secp256k1_fe_sqr(secp256k1_fe *r, const secp256k1_fe *a)
{
    secp256k1_fe_sqr_inner(r->n, a->n);
}

static SECP256K1_INLINE void secp256k1_fe_cmov2(secp256k1_fe *r, const secp256k1_fe *a, int flag)
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

static SECP256K1_INLINE void secp256k1_fe_cmov(secp256k1_fe *r, const secp256k1_fe *a, int flag)
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

static SECP256K1_INLINE void secp256k1_fe_storage_cmov(secp256k1_fe_storage *r, const secp256k1_fe_storage *a, int flag)
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
}

static void secp256k1_fe_to_storage(secp256k1_fe_storage *r, const secp256k1_fe *a)
{
    r->n[0] = a->n[0] | a->n[1] << 26;
    r->n[1] = a->n[1] >> 6 | a->n[2] << 20;
    r->n[2] = a->n[2] >> 12 | a->n[3] << 14;
    r->n[3] = a->n[3] >> 18 | a->n[4] << 8;
    r->n[4] = a->n[4] >> 24 | a->n[5] << 2 | a->n[6] << 28;
    r->n[5] = a->n[6] >> 4 | a->n[7] << 22;
    r->n[6] = a->n[7] >> 10 | a->n[8] << 16;
    r->n[7] = a->n[8] >> 16 | a->n[9] << 10;
}

static SECP256K1_INLINE void secp256k1_fe_from_storage(secp256k1_fe *r, const secp256k1_fe_storage *a)
{
    r->n[0] = a->n[0] & 0x3FFFFFFUL;
    r->n[1] = a->n[0] >> 26 | ((a->n[1] << 6) & 0x3FFFFFFUL);
    r->n[2] = a->n[1] >> 20 | ((a->n[2] << 12) & 0x3FFFFFFUL);
    r->n[3] = a->n[2] >> 14 | ((a->n[3] << 18) & 0x3FFFFFFUL);
    r->n[4] = a->n[3] >> 8 | ((a->n[4] << 24) & 0x3FFFFFFUL);
    r->n[5] = (a->n[4] >> 2) & 0x3FFFFFFUL;
    r->n[6] = a->n[4] >> 28 | ((a->n[5] << 4) & 0x3FFFFFFUL);
    r->n[7] = a->n[5] >> 22 | ((a->n[6] << 10) & 0x3FFFFFFUL);
    r->n[8] = a->n[6] >> 16 | ((a->n[7] << 16) & 0x3FFFFFFUL);
    r->n[9] = a->n[7] >> 10;
}

SECP256K1_INLINE static int secp256k1_fe_equal(const secp256k1_fe *a, const secp256k1_fe *b)
{
    secp256k1_fe na;
    secp256k1_fe_negate(&na, a, 1);
    secp256k1_fe_add(&na, b);
    return secp256k1_fe_normalizes_to_zero(&na);
}

static int secp256k1_fe_sqrt(secp256k1_fe *r, const secp256k1_fe *a)
{
    /** Given that p is congruent to 3 mod 4, we can compute the square root of
     *  a mod p as the (p+1)/4'th power of a.
     *
     *  As (p+1)/4 is an even number, it will have the same result for a and for
     *  (-a). Only one of these two numbers actually has a square root however,
     *  so we test at the end by squaring and comparing to the input.
     *  Also because (p+1)/4 is an even number, the computed square root is
     *  itself always a square (a ** ((p+1)/4) is the square of a ** ((p+1)/8)).
     */
    secp256k1_fe x2, x3, x6, x9, x11, x22, x44, x88, x176, x220, x223, t1;
    int j;

    /** The binary representation of (p + 1)/4 has 3 blocks of 1s, with lengths in
     *  { 2, 22, 223 }. Use an addition chain to calculate 2^n - 1 for each block:
     *  1, [2], 3, 6, 9, 11, [22], 44, 88, 176, 220, [223]
     */

    secp256k1_fe_sqr(&x2, a);
    secp256k1_fe_mul(&x2, &x2, a);

    secp256k1_fe_sqr(&x3, &x2);
    secp256k1_fe_mul(&x3, &x3, a);

    x6 = x3;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x6, &x6);
    }
    secp256k1_fe_mul(&x6, &x6, &x3);

    x9 = x6;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x9, &x9);
    }
    secp256k1_fe_mul(&x9, &x9, &x3);

    x11 = x9;
    for (j = 0; j < 2; j++) {
        secp256k1_fe_sqr(&x11, &x11);
    }
    secp256k1_fe_mul(&x11, &x11, &x2);

    x22 = x11;
    for (j = 0; j < 11; j++) {
        secp256k1_fe_sqr(&x22, &x22);
    }
    secp256k1_fe_mul(&x22, &x22, &x11);

    x44 = x22;
    for (j = 0; j < 22; j++) {
        secp256k1_fe_sqr(&x44, &x44);
    }
    secp256k1_fe_mul(&x44, &x44, &x22);

    x88 = x44;
    for (j = 0; j < 44; j++) {
        secp256k1_fe_sqr(&x88, &x88);
    }
    secp256k1_fe_mul(&x88, &x88, &x44);

    x176 = x88;
    for (j = 0; j < 88; j++) {
        secp256k1_fe_sqr(&x176, &x176);
    }
    secp256k1_fe_mul(&x176, &x176, &x88);

    x220 = x176;
    for (j = 0; j < 44; j++) {
        secp256k1_fe_sqr(&x220, &x220);
    }
    secp256k1_fe_mul(&x220, &x220, &x44);

    x223 = x220;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x223, &x223);
    }
    secp256k1_fe_mul(&x223, &x223, &x3);

    /* The final result is then assembled using a sliding window over the blocks. */

    t1 = x223;
    for (j = 0; j < 23; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x22);
    for (j = 0; j < 6; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x2);
    secp256k1_fe_sqr(&t1, &t1);
    secp256k1_fe_sqr(r, &t1);

    /* Check that a square root was actually calculated */

    secp256k1_fe_sqr(&t1, r);
    return secp256k1_fe_equal(&t1, a);
}

static void secp256k1_fe_inv(secp256k1_fe *r, const secp256k1_fe *a)
{
    secp256k1_fe x2, x3, x6, x9, x11, x22, x44, x88, x176, x220, x223, t1;
    int j;

    /** The binary representation of (p - 2) has 5 blocks of 1s, with lengths in
     *  { 1, 2, 22, 223 }. Use an addition chain to calculate 2^n - 1 for each block:
     *  [1], [2], 3, 6, 9, 11, [22], 44, 88, 176, 220, [223]
     */

    secp256k1_fe_sqr(&x2, a);
    secp256k1_fe_mul(&x2, &x2, a);

    secp256k1_fe_sqr(&x3, &x2);
    secp256k1_fe_mul(&x3, &x3, a);

    x6 = x3;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x6, &x6);
    }
    secp256k1_fe_mul(&x6, &x6, &x3);

    x9 = x6;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x9, &x9);
    }
    secp256k1_fe_mul(&x9, &x9, &x3);

    x11 = x9;
    for (j = 0; j < 2; j++) {
        secp256k1_fe_sqr(&x11, &x11);
    }
    secp256k1_fe_mul(&x11, &x11, &x2);

    x22 = x11;
    for (j = 0; j < 11; j++) {
        secp256k1_fe_sqr(&x22, &x22);
    }
    secp256k1_fe_mul(&x22, &x22, &x11);

    x44 = x22;
    for (j = 0; j < 22; j++) {
        secp256k1_fe_sqr(&x44, &x44);
    }
    secp256k1_fe_mul(&x44, &x44, &x22);

    x88 = x44;
    for (j = 0; j < 44; j++) {
        secp256k1_fe_sqr(&x88, &x88);
    }
    secp256k1_fe_mul(&x88, &x88, &x44);

    x176 = x88;
    for (j = 0; j < 88; j++) {
        secp256k1_fe_sqr(&x176, &x176);
    }
    secp256k1_fe_mul(&x176, &x176, &x88);

    x220 = x176;
    for (j = 0; j < 44; j++) {
        secp256k1_fe_sqr(&x220, &x220);
    }
    secp256k1_fe_mul(&x220, &x220, &x44);

    x223 = x220;
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&x223, &x223);
    }
    secp256k1_fe_mul(&x223, &x223, &x3);

    /* The final result is then assembled using a sliding window over the blocks. */

    t1 = x223;
    for (j = 0; j < 23; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x22);
    for (j = 0; j < 5; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, a);
    for (j = 0; j < 3; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x2);
    for (j = 0; j < 2; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(r, a, &t1);
}

//===================================================//




/** Generator for secp256k1, value 'g' defined in
 *  "Standards for Efficient Cryptography" (SEC2) 2.7.1.
 */

const secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
    0x79BE667EUL, 0xF9DCBBACUL, 0x55A06295UL, 0xCE870B07UL,
    0x029BFCDBUL, 0x2DCE28D9UL, 0x59F2815BUL, 0x16F81798UL,
    0x483ADA77UL, 0x26A3C465UL, 0x5DA4FBFCUL, 0x0E1108A8UL,
    0xFD17B448UL, 0xA6855419UL, 0x9C47D08FUL, 0xFB10D4B8UL
);


#define CURVE_B (7)

void secp256k1_ge_set_gej_zinv(secp256k1_ge *r, const secp256k1_gej *a, const secp256k1_fe *zi)
{
    secp256k1_fe zi2;
    secp256k1_fe zi3;
    secp256k1_fe_sqr(&zi2, zi);
    secp256k1_fe_mul(&zi3, &zi2, zi);
    secp256k1_fe_mul(&r->x, &a->x, &zi2);
    secp256k1_fe_mul(&r->y, &a->y, &zi3);
    r->infinity = a->infinity;
}

void secp256k1_ge_set_gej(secp256k1_ge *r, secp256k1_gej *a)
{
    secp256k1_fe z2, z3;
    r->infinity = a->infinity;
    secp256k1_fe_inv(&a->z, &a->z);
    secp256k1_fe_sqr(&z2, &a->z);
    secp256k1_fe_mul(&z3, &a->z, &z2);
    secp256k1_fe_mul(&a->x, &a->x, &z2);
    secp256k1_fe_mul(&a->y, &a->y, &z3);
    secp256k1_fe_set_int(&a->z, 1);
    r->x = a->x;
    r->y = a->y;
}

void secp256k1_ge_set_all_gej_var(secp256k1_ge *r, const secp256k1_gej *a, unsigned int len)
{
    secp256k1_fe u;
    unsigned int i;
	unsigned int last_i = SIZE_MAX_NUM;

    for (i = 0; i < len; i++)
    {
        if (!a[i].infinity)
        {
            /* Use destination's x coordinates as scratch space */
            if (last_i == SIZE_MAX_NUM) {
                r[i].x = a[i].z;
            }
            else {
                secp256k1_fe_mul(&r[i].x, &r[last_i].x, &a[i].z);
            }
            last_i = i;
        }
    }
    if (last_i == SIZE_MAX_NUM)
        return;

    secp256k1_fe_inv(&u, &r[last_i].x);

    i = last_i;
    while (i > 0)
    {
        i--;
        if (!a[i].infinity)
        {
            secp256k1_fe_mul(&r[last_i].x, &r[i].x, &u);
            secp256k1_fe_mul(&u, &u, &a[last_i].z);
            last_i = i;
        }
    }

    r[last_i].x = u;

    for (i = 0; i < len; i++)
    {
        r[i].infinity = a[i].infinity;
        if (!a[i].infinity)
            secp256k1_ge_set_gej_zinv(&r[i], &a[i], &r[i].x);
    }
}

static void secp256k1_gej_clear(secp256k1_gej *r)
{
    r->infinity = 0;
}

static void secp256k1_ge_clear(secp256k1_ge *r)
{
    r->infinity = 0;
}

static int secp256k1_ge_set_xquad(secp256k1_ge *r, const secp256k1_fe *x)
{
    secp256k1_fe x2, x3, c;
    r->x = *x;
    secp256k1_fe_sqr(&x2, x);
    secp256k1_fe_mul(&x3, x, &x2);
    r->infinity = 0;
    secp256k1_fe_set_int(&c, CURVE_B);
    secp256k1_fe_add(&c, &x3);
    return secp256k1_fe_sqrt(&r->y, &c);
}

int secp256k1_ge_set_xo_var(secp256k1_ge *r, const secp256k1_fe *x, int odd)
{
    if (!secp256k1_ge_set_xquad(r, x)) {
        return 0;
    }
    secp256k1_fe_normalize_var(&r->y);
    if (secp256k1_fe_is_odd(&r->y) != odd) {
        secp256k1_fe_negate(&r->y, &r->y, 1);
    }
    return 1;
}

void secp256k1_gej_set_ge(secp256k1_gej *r, const secp256k1_ge *a)
{
    r->infinity = a->infinity;
    r->x = a->x;
    r->y = a->y;
    secp256k1_fe_set_int(&r->z, 1);
}

void secp256k1_gej_neg(secp256k1_gej *r, const secp256k1_gej *a)
{
    r->infinity = a->infinity;
    r->x = a->x;
    r->y = a->y;
    r->z = a->z;
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_negate(&r->y, &r->y, 1);
}

static void secp256k1_gej_double_var(secp256k1_gej *r, const secp256k1_gej *a, secp256k1_fe *rzr)
{
    secp256k1_fe t1, t2, t3, t4;

    r->infinity = a->infinity;
    if (r->infinity)
    {
        if (rzr != NULL)
        {
            secp256k1_fe_set_int(rzr, 1);
        }
        return;
    }

    if (rzr != NULL)
    {
        *rzr = a->y;
        secp256k1_fe_normalize_weak(rzr);
        secp256k1_fe_mul_int(rzr, 2);
    }

    secp256k1_fe_mul(&r->z, &a->z, &a->y);
    secp256k1_fe_mul_int(&r->z, 2);       /* Z' = 2*Y*Z (2) */
    secp256k1_fe_sqr(&t1, &a->x);
    secp256k1_fe_mul_int(&t1, 3);         /* T1 = 3*X^2 (3) */
    secp256k1_fe_sqr(&t2, &t1);           /* T2 = 9*X^4 (1) */
    secp256k1_fe_sqr(&t3, &a->y);
    secp256k1_fe_mul_int(&t3, 2);         /* T3 = 2*Y^2 (2) */
    secp256k1_fe_sqr(&t4, &t3);
    secp256k1_fe_mul_int(&t4, 2);         /* T4 = 8*Y^4 (2) */
    secp256k1_fe_mul(&t3, &t3, &a->x);    /* T3 = 2*X*Y^2 (1) */
    r->x = t3;
    secp256k1_fe_mul_int(&r->x, 4);       /* X' = 8*X*Y^2 (4) */
    secp256k1_fe_negate(&r->x, &r->x, 4); /* X' = -8*X*Y^2 (5) */
    secp256k1_fe_add(&r->x, &t2);         /* X' = 9*X^4 - 8*X*Y^2 (6) */
    secp256k1_fe_negate(&t2, &t2, 1);     /* T2 = -9*X^4 (2) */
    secp256k1_fe_mul_int(&t3, 6);         /* T3 = 12*X*Y^2 (6) */
    secp256k1_fe_add(&t3, &t2);           /* T3 = 12*X*Y^2 - 9*X^4 (8) */
    secp256k1_fe_mul(&r->y, &t1, &t3);    /* Y' = 36*X^3*Y^2 - 27*X^6 (1) */
    secp256k1_fe_negate(&t2, &t4, 2);     /* T2 = -8*Y^4 (3) */
    secp256k1_fe_add(&r->y, &t2);         /* Y' = 36*X^3*Y^2 - 27*X^6 - 8*Y^4 (4) */
}

static void secp256k1_gej_add_var(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_gej *b, secp256k1_fe *rzr)
{
    /* Operations: 12 mul, 4 sqr, 2 normalize, 12 mul_int/add/negate */
    secp256k1_fe z22, z12, u1, u2, s1, s2, h, i, i2, h2, h3, t;

    if (a->infinity)
    {
        *r = *b;
        return;
    }

    if (b->infinity) {
        if (rzr != NULL) {
            secp256k1_fe_set_int(rzr, 1);
        }
        *r = *a;
        return;
    }

    r->infinity = 0;
    secp256k1_fe_sqr(&z22, &b->z);
    secp256k1_fe_sqr(&z12, &a->z);
    secp256k1_fe_mul(&u1, &a->x, &z22);
    secp256k1_fe_mul(&u2, &b->x, &z12);
    secp256k1_fe_mul(&s1, &a->y, &z22); secp256k1_fe_mul(&s1, &s1, &b->z);
    secp256k1_fe_mul(&s2, &b->y, &z12); secp256k1_fe_mul(&s2, &s2, &a->z);
    secp256k1_fe_negate(&h, &u1, 1); secp256k1_fe_add(&h, &u2);
    secp256k1_fe_negate(&i, &s1, 1); secp256k1_fe_add(&i, &s2);
    if (secp256k1_fe_normalizes_to_zero_var(&h)) {
        if (secp256k1_fe_normalizes_to_zero_var(&i)) {
            secp256k1_gej_double_var(r, a, rzr);
        }
        else {
            if (rzr != NULL) {
                secp256k1_fe_set_int(rzr, 0);
            }
            r->infinity = 1;
        }
        return;
    }
    secp256k1_fe_sqr(&i2, &i);
    secp256k1_fe_sqr(&h2, &h);
    secp256k1_fe_mul(&h3, &h, &h2);
    secp256k1_fe_mul(&h, &h, &b->z);
    if (rzr != NULL) {
        *rzr = h;
    }
    secp256k1_fe_mul(&r->z, &a->z, &h);
    secp256k1_fe_mul(&t, &u1, &h2);
    r->x = t; secp256k1_fe_mul_int(&r->x, 2); secp256k1_fe_add(&r->x, &h3); secp256k1_fe_negate(&r->x, &r->x, 3); secp256k1_fe_add(&r->x, &i2);
    secp256k1_fe_negate(&r->y, &r->x, 5); secp256k1_fe_add(&r->y, &t); secp256k1_fe_mul(&r->y, &r->y, &i);
    secp256k1_fe_mul(&h3, &h3, &s1); secp256k1_fe_negate(&h3, &h3, 1);
    secp256k1_fe_add(&r->y, &h3);
}

void secp256k1_gej_add_ge_var(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b, secp256k1_fe *rzr)
{
    /* 8 mul, 3 sqr, 4 normalize, 12 mul_int/add/negate */
    secp256k1_fe z12, u1, u2, s1, s2, h, i, i2, h2, h3, t;
    if (a->infinity)
    {
        secp256k1_gej_set_ge(r, b);
        return;
    }
    if (b->infinity) {
        if (rzr != NULL) {
            secp256k1_fe_set_int(rzr, 1);
        }
        *r = *a;
        return;
    }
    r->infinity = 0;

    secp256k1_fe_sqr(&z12, &a->z);
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);
    secp256k1_fe_mul(&u2, &b->x, &z12);
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);
    secp256k1_fe_mul(&s2, &b->y, &z12); secp256k1_fe_mul(&s2, &s2, &a->z);
    secp256k1_fe_negate(&h, &u1, 1); secp256k1_fe_add(&h, &u2);
    secp256k1_fe_negate(&i, &s1, 1); secp256k1_fe_add(&i, &s2);
    if (secp256k1_fe_normalizes_to_zero_var(&h)) {
        if (secp256k1_fe_normalizes_to_zero_var(&i)) {
            secp256k1_gej_double_var(r, a, rzr);
        }
        else {
            if (rzr != NULL) {
                secp256k1_fe_set_int(rzr, 0);
            }
            r->infinity = 1;
        }
        return;
    }
    secp256k1_fe_sqr(&i2, &i);
    secp256k1_fe_sqr(&h2, &h);
    secp256k1_fe_mul(&h3, &h, &h2);
    if (rzr != NULL) {
        *rzr = h;
    }
    secp256k1_fe_mul(&r->z, &a->z, &h);
    secp256k1_fe_mul(&t, &u1, &h2);
    r->x = t; secp256k1_fe_mul_int(&r->x, 2); secp256k1_fe_add(&r->x, &h3); secp256k1_fe_negate(&r->x, &r->x, 3); secp256k1_fe_add(&r->x, &i2);
    secp256k1_fe_negate(&r->y, &r->x, 5); secp256k1_fe_add(&r->y, &t); secp256k1_fe_mul(&r->y, &r->y, &i);
    secp256k1_fe_mul(&h3, &h3, &s1); secp256k1_fe_negate(&h3, &h3, 1);
    secp256k1_fe_add(&r->y, &h3);
}

void secp256k1_gej_add_ge(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b)
{
    /* Operations: 7 mul, 5 sqr, 4 normalize, 21 mul_int/add/negate/cmov */
    //static const secp256k1_fe fe_1 = SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 1);
    static const secp256k1_fe fe_1 = { 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    secp256k1_fe zz, u1, u2, s1, s2, t, tt, m, n, q, rr;
    secp256k1_fe m_alt, rr_alt;
    int infinity, degenerate;

    //secp256k1_fe_sqr(&zz, &a->z);                     /* z = Z1^2 */
    secp256k1_fe_sqr_inner(zz.n, a->z.n);               /* z = Z1^2 */
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);        /* u1 = U1 = X1*Z2^2 (1) */
    secp256k1_fe_mul(&u2, &b->x, &zz);                  /* u2 = U2 = X2*Z1^2 (1) */
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);        /* s1 = S1 = Y1*Z2^3 (1) */
    secp256k1_fe_mul(&s2, &b->y, &zz);                  /* s2 = Y2*Z1^2 (1) */
    secp256k1_fe_mul(&s2, &s2, &a->z);                  /* s2 = S2 = Y2*Z1^3 (1) */
    t = u1; secp256k1_fe_add(&t, &u2);                  /* t = T = U1+U2 (2) */
    m = s1; secp256k1_fe_add(&m, &s2);                  /* m = M = S1+S2 (2) */
    secp256k1_fe_sqr(&rr, &t);                          /* rr = T^2 (1) */
    secp256k1_fe_negate(&m_alt, &u2, 1);                /* Malt = -X2*Z1^2 */
    secp256k1_fe_mul(&tt, &u1, &m_alt);                 /* tt = -U1*U2 (2) */
    secp256k1_fe_add(&rr, &tt);                         /* rr = R = T^2-U1*U2 (3) */

    degenerate = secp256k1_fe_normalizes_to_zero(&m) & secp256k1_fe_normalizes_to_zero(&rr);

    rr_alt = s1;
    secp256k1_fe_mul_int(&rr_alt, 2);       /* rr = Y1*Z2^3 - Y2*Z1^3 (2) */
    secp256k1_fe_add(&m_alt, &u1);          /* Malt = X1*Z2^2 - X2*Z1^2 */

    secp256k1_fe_cmov(&rr_alt, &rr, !degenerate);
    secp256k1_fe_cmov(&m_alt, &m, !degenerate);

    secp256k1_fe_sqr(&n, &m_alt);                       /* n = Malt^2 (1) */
    secp256k1_fe_mul(&q, &n, &t);                       /* q = Q = T*Malt^2 (1) */

    secp256k1_fe_sqr(&n, &n);
    secp256k1_fe_cmov(&n, &m, degenerate);              /* n = M^3 * Malt (2) */
    secp256k1_fe_sqr(&t, &rr_alt);                      /* t = Ralt^2 (1) */
    secp256k1_fe_mul(&r->z, &a->z, &m_alt);             /* r->z = Malt*Z (1) */
    infinity = secp256k1_fe_normalizes_to_zero(&r->z) * (1 - a->infinity);
    secp256k1_fe_mul_int(&r->z, 2);                     /* r->z = Z3 = 2*Malt*Z (2) */
    secp256k1_fe_negate(&q, &q, 1);                     /* q = -Q (2) */
    secp256k1_fe_add(&t, &q);                           /* t = Ralt^2-Q (3) */
    secp256k1_fe_normalize_weak(&t);
    r->x = t;                                           /* r->x = Ralt^2-Q (1) */
    secp256k1_fe_mul_int(&t, 2);                        /* t = 2*x3 (2) */
    secp256k1_fe_add(&t, &q);                           /* t = 2*x3 - Q: (4) */
    secp256k1_fe_mul(&t, &t, &rr_alt);                  /* t = Ralt*(2*x3 - Q) (1) */
    secp256k1_fe_add(&t, &n);                           /* t = Ralt*(2*x3 - Q) + M^3*Malt (3) */
    secp256k1_fe_negate(&r->y, &t, 3);                  /* r->y = Ralt*(Q - 2x3) - M^3*Malt (4) */
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_mul_int(&r->x, 4);                     /* r->x = X3 = 4*(Ralt^2-Q) */
    secp256k1_fe_mul_int(&r->y, 4);                     /* r->y = Y3 = 4*Ralt*(Q - 2x3) - 4*M^3*Malt (4) */

    /** In case a->infinity == 1, replace r with (b->x, b->y, 1). */
    secp256k1_fe_cmov2(&r->x, &b->x, a->infinity);
    secp256k1_fe_cmov2(&r->y, &b->y, a->infinity);
    secp256k1_fe_cmov2(&r->z, &fe_1, a->infinity);
    r->infinity = infinity;
}

void secp256k1_gej_rescale(secp256k1_gej *r, const secp256k1_fe *s)
{
    /* Operations: 4 mul, 1 sqr */
    secp256k1_fe zz;

    secp256k1_fe_sqr(&zz, s);
    secp256k1_fe_mul(&r->x, &r->x, &zz);                /* r->x *= s^2 */
    secp256k1_fe_mul(&r->y, &r->y, &zz);
    secp256k1_fe_mul(&r->y, &r->y, s);                  /* r->y *= s^3 */
    secp256k1_fe_mul(&r->z, &r->z, s);                  /* r->z *= s   */
}

void secp256k1_ge_to_storage(secp256k1_ge_storage *r, const secp256k1_ge *a)
{
    secp256k1_fe x, y;

    x = a->x;    secp256k1_fe_normalize(&x);
    y = a->y;    secp256k1_fe_normalize(&y);
    secp256k1_fe_to_storage(&r->x, &x);
    secp256k1_fe_to_storage(&r->y, &y);
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
static void secp256k1_ge_from_storage(secp256k1_ge *r, const secp256k1_ge_storage *a)
{
    secp256k1_fe_from_storage(&r->x, &a->x);
    secp256k1_fe_from_storage(&r->y, &a->y);
    r->infinity = 0;
}

static SECP256K1_INLINE void secp256k1_ge_storage_cmov(secp256k1_ge_storage *r, const secp256k1_ge_storage *a, int flag)
{
    secp256k1_fe_storage_cmov(&r->x, &a->x, flag);
    secp256k1_fe_storage_cmov(&r->y, &a->y, flag);
}


//===================================================//

void secp256k1_ecmult_gen(const secp256k1_ecmult_gen_context *ctx, unsigned char * pubkey, const unsigned char * seckey)
{
    secp256k1_scalar gn;
    int overflow = 0;
    secp256k1_scalar_set_b32(&gn, seckey, &overflow);

    secp256k1_ge add;
    secp256k1_ge_storage adds;
    secp256k1_scalar gnb;
    int bits;
    int i, j;

    adds.x.n[0] = 0x0;
    adds.x.n[1] = 0x0;
    adds.x.n[2] = 0x0;
    adds.x.n[3] = 0x0;
    adds.x.n[4] = 0x0;
    adds.x.n[5] = 0x0;
    adds.x.n[6] = 0x0;
    adds.x.n[7] = 0x0;

    adds.y.n[0] = 0x0;
    adds.y.n[1] = 0x0;
    adds.y.n[2] = 0x0;
    adds.y.n[3] = 0x0;
    adds.y.n[4] = 0x0;
    adds.y.n[5] = 0x0;
    adds.y.n[6] = 0x0;
    adds.y.n[7] = 0x0;
    secp256k1_gej pj = ctx->initial;

    /* Blind scalar/point multiplication by computing (n-b)G + bG instead of nG. */
    secp256k1_scalar_add(&gnb, &gn, &ctx->blind);
    add.infinity = 0;
    for (j = 0; j < 64; j++)
    {
        bits = secp256k1_scalar_get_bits(&gnb, j * 4, 4);
        for (i = 0; i < 16; i++)
        {
            secp256k1_ge_storage_cmov(&adds, &ctx->prec[j][i], i == bits);
        }
        secp256k1_ge_from_storage(&add, &adds);
        secp256k1_gej_add_ge(&pj, &pj, &add);
    }

    secp256k1_ge p;
    secp256k1_ge_set_gej(&p, &pj);
    secp256k1_ge_to_storage((secp256k1_ge_storage *)pubkey, &p);
}

/* Setup blinding values for secp256k1_ecmult_gen. */
static void secp256k1_ecmult_gen_blind(secp256k1_ecmult_gen_context *ctx)
{
    //下面计算由常量secp256k1_ge_const_g得到，中间过程去掉
    //盲点设为固定值1

    secp256k1_scalar b;
    secp256k1_gej gb;

    b.d[0] = 0xd0364141;
    b.d[1] = 0xbfd25e8c;
    b.d[2] = 0xaf48a03b;
    b.d[3] = 0xbaaedce6;
    b.d[4] = 0xfffffffe;
    b.d[5] = 0xffffffff;
    b.d[6] = 0xffffffff;
    b.d[7] = 0xfeffffff;

    gb.x.n[0] = 0x092292dc;
    gb.x.n[1] = 0x04ad2dac;
    gb.x.n[2] = 0x03eb6e0c;
    gb.x.n[3] = 0x07500a44;
    gb.x.n[4] = 0x064400b0;
    gb.x.n[5] = 0x05623ab0;
    gb.x.n[6] = 0x0213323c;
    gb.x.n[7] = 0x0286704c;
    gb.x.n[8] = 0x0b7d4b18;
    gb.x.n[9] = 0x008d497c;

    gb.y.n[0] = 0x097e12c4;
    gb.y.n[1] = 0x04f46a4c;
    gb.y.n[2] = 0x0ab59790;
    gb.y.n[3] = 0x0aacfee0;
    gb.y.n[4] = 0x04d09868;
    gb.y.n[5] = 0x0dc8247c;
    gb.y.n[6] = 0x03bc80f4;
    gb.y.n[7] = 0x014beea4;
    gb.y.n[8] = 0x06d08768;
    gb.y.n[9] = 0x00951f1c;

    gb.z.n[0] = 0x032974b6;
    gb.z.n[1] = 0x06695f8a;
    gb.z.n[2] = 0x03cc840c;
    gb.z.n[3] = 0x000c8998;
    gb.z.n[4] = 0x04f06406;
    gb.z.n[5] = 0x0438d148;
    gb.z.n[6] = 0x029e81a2;
    gb.z.n[7] = 0x0784c484;
    gb.z.n[8] = 0x06ad6038;
    gb.z.n[9] = 0x001383ee;
    gb.infinity = 0;

    ctx->blind = b;
    ctx->initial = gb;
}

void secp256k1_ecmult_gen_context_build(secp256k1_ecmult_gen_context *ctx)
{
    secp256k1_ge prec[1024];
    secp256k1_gej gj;
    secp256k1_gej nums_gej;
    int i, j;

    /* get the generator */
    //secp256k1_gej_set_ge(&gj, &secp256k1_ge_const_g);

    gj.x.n[0] = 0x02f81798;
    gj.x.n[1] = 0x00a056c5;
    gj.x.n[2] = 0x028d959f;
    gj.x.n[3] = 0x036cb738;
    gj.x.n[4] = 0x03029bfc;
    gj.x.n[5] = 0x03a1c2c1;
    gj.x.n[6] = 0x0206295c;
    gj.x.n[7] = 0x02eeb156;
    gj.x.n[8] = 0x027ef9dc;
    gj.x.n[9] = 0x001e6f99;

    gj.y.n[0] = 0x0310d4b8;
    gj.y.n[1] = 0x01f423fe;
    gj.y.n[2] = 0x014199c4;
    gj.y.n[3] = 0x01229a15;
    gj.y.n[4] = 0x00fd17b4;
    gj.y.n[5] = 0x0384422a;
    gj.y.n[6] = 0x024fbfc0;
    gj.y.n[7] = 0x03119576;
    gj.y.n[8] = 0x027726a3;
    gj.y.n[9] = 0x00120eb6;

    gj.z.n[0] = 1;
    gj.z.n[1] = 0;
    gj.z.n[2] = 0;
    gj.z.n[3] = 0;
    gj.z.n[4] = 0;
    gj.z.n[5] = 0;
    gj.z.n[6] = 0;
    gj.z.n[7] = 0;
    gj.z.n[8] = 0;
    gj.z.n[9] = 0;
    gj.infinity = 0;

    nums_gej.x.n[0] = 0x1953405e;
    nums_gej.x.n[1] = 0x18b34eec;
    nums_gej.x.n[2] = 0x1eaec383;
    nums_gej.x.n[3] = 0x1804c97c;
    nums_gej.x.n[4] = 0x1a6f3a61;
    nums_gej.x.n[5] = 0x187b44dd;
    nums_gej.x.n[6] = 0x1a2ffba0;
    nums_gej.x.n[7] = 0x1e467d9e;
    nums_gej.x.n[8] = 0x1efdcea7;
    nums_gej.x.n[9] = 0x01e67980;

    nums_gej.y.n[0] = 0x0e2a3255;
    nums_gej.y.n[1] = 0x0f979780;
    nums_gej.y.n[2] = 0x0d96fb06;
    nums_gej.y.n[3] = 0x0c758427;
    nums_gej.y.n[4] = 0x1075b520;
    nums_gej.y.n[5] = 0x0cccc65c;
    nums_gej.y.n[6] = 0x0f2d4724;
    nums_gej.y.n[7] = 0x0fb02ea5;
    nums_gej.y.n[8] = 0x127222f2;
    nums_gej.y.n[9] = 0x00f1b07f;

    nums_gej.z.n[0] = 0x0088a02a;
    nums_gej.z.n[1] = 0x0344bbea;
    nums_gej.z.n[2] = 0x03f66397;
    nums_gej.z.n[3] = 0x02ead6b6;
    nums_gej.z.n[4] = 0x029a3289;
    nums_gej.z.n[5] = 0x03c53aa4;
    nums_gej.z.n[6] = 0x02e422f5;
    nums_gej.z.n[7] = 0x0168ffd0;
    nums_gej.z.n[8] = 0x015e8679;
    nums_gej.z.n[9] = 0x00095580;
    nums_gej.infinity = 0;

    /* compute prec. */
    {
        secp256k1_gej precj[1024]; /* Jacobian versions of prec. */
        secp256k1_gej gbase;
        secp256k1_gej numsbase;
        gbase = gj; /* 16^j * G */
        numsbase = nums_gej; /* 2^j * nums. */
        for (j = 0; j < 64; j++)
        {
            /* Set precj[j*16 .. j*16+15] to (numsbase, numsbase + gbase, ..., numsbase + 15*gbase). */
            precj[j*16] = numsbase;
            for (i = 1; i < 16; i++)
            {
                secp256k1_gej_add_var(&precj[j*16 + i], &precj[j*16 + i - 1], &gbase, NULL);
            }
            /* Multiply gbase by 16. */
            for (i = 0; i < 4; i++)
            {
                secp256k1_gej_double_var(&gbase, &gbase, NULL);
            }
            /* Multiply numbase by 2. */
            secp256k1_gej_double_var(&numsbase, &numsbase, NULL);
            if (j == 62)
            {
                /* In the last iteration, numsbase is (1 - 2^j) * nums instead. */
                secp256k1_gej_neg(&numsbase, &numsbase);
                secp256k1_gej_add_var(&numsbase, &numsbase, &nums_gej, NULL);
            }
        }
        secp256k1_ge_set_all_gej_var(prec, precj, 1024);
    }
    for (j = 0; j < 64; j++)
    {
        for (i = 0; i < 16; i++)
        {
            secp256k1_ge_to_storage(&ctx->prec[j][i], &prec[j*16 + i]);
        }
    }

    secp256k1_ecmult_gen_blind(ctx);
}

#endif /* SECP256K1_ECMULT_GEN_IMPL_H */
