#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

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

	typedef signed char        int8_t;
	typedef short              int16_t;
	typedef int                int32_t;
	typedef long long          int64_t;
	typedef unsigned char      uint8_t;
	typedef unsigned short     uint16_t;
	typedef unsigned int       uint32_t;
	typedef unsigned long long uint64_t;

	#define NULL 0

	#define SECP256K1_INLINE __inline
	#define SIZE_MAX_NUM ((uint32_t)0xFFFFFFFFUL)

	/** A scalar modulo the group order of the secp256k1 curve. */
	typedef struct
	{
		uint32_t d[8];
	} secp256k1_scalar;

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

	typedef struct
	{
		secp256k1_ge_storage prec[64][16]; /* prec[j][i] = 16^j * i * G + U_i */
		secp256k1_scalar blind;
		secp256k1_gej initial;
	} secp256k1_ecmult_gen_context;

	void secp256k1_gej_set_ge(secp256k1_gej *r, const secp256k1_ge *a);
	void secp256k1_gej_add_ge(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b);
	void secp256k1_ge_set_gej(secp256k1_ge *r, secp256k1_gej *a);
	void secp256k1_ge_to_storage(secp256k1_ge_storage *r, const secp256k1_ge *a);
	void secp256k1_ge_to_buff(unsigned char *out, const secp256k1_ge *a);

	void secp256k1_ecmult_gen(const secp256k1_ecmult_gen_context *ctx, unsigned char * pubkey, const unsigned char * seckey);
	void secp256k1_ecmult_gen_context_build(secp256k1_ecmult_gen_context *ctx);

	extern const secp256k1_ge secp256k1_ge_const_g;
#ifdef __cplusplus
}
#endif
