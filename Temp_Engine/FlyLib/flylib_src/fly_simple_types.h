#ifndef FLY_SIMPLE_TYPES
#define FLY_SIMPLE_TYPES

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// BIT MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum FLY_BitFlags
{
	BITSET1 = 1 << 0, BITSET17 = 1 << 16,	// BITSET33 = 1 << 32,	BITSET49 = 1 << 48,
	BITSET2 = 1 << 1, BITSET18 = 1 << 17,	// BITSET34 = 1 << 33,	BITSET50 = 1 << 49,
	BITSET3 = 1 << 2, BITSET19 = 1 << 18,	// BITSET35 = 1 << 34,	BITSET51 = 1 << 50,
	BITSET4 = 1 << 3, BITSET20 = 1 << 19,	// BITSET36 = 1 << 35,	BITSET52 = 1 << 51,
	BITSET5 = 1 << 4, BITSET21 = 1 << 20,	// BITSET37 = 1 << 36,	BITSET53 = 1 << 52,
	BITSET6 = 1 << 5, BITSET22 = 1 << 21,	// BITSET38 = 1 << 37,	BITSET54 = 1 << 53,
	BITSET7 = 1 << 6, BITSET23 = 1 << 22,	// BITSET39 = 1 << 38,	BITSET55 = 1 << 54,
	BITSET8 = 1 << 7, BITSET24 = 1 << 23,	// BITSET40 = 1 << 39,	BITSET56 = 1 << 55,
	BITSET9 = 1 << 8, BITSET25 = 1 << 24,	// BITSET41 = 1 << 40,	BITSET57 = 1 << 56,
	BITSET10 = 1 << 9, BITSET26 = 1 << 25,	// BITSET42 = 1 << 41,	BITSET58 = 1 << 57,
	BITSET11 = 1 << 10, BITSET27 = 1 << 26,	// BITSET43 = 1 << 42,	BITSET59 = 1 << 58,
	BITSET12 = 1 << 11, BITSET28 = 1 << 27,	// BITSET44 = 1 << 43,	BITSET60 = 1 << 59,
	BITSET13 = 1 << 12, BITSET29 = 1 << 28,	// BITSET45 = 1 << 44,	BITSET61 = 1 << 60,
	BITSET14 = 1 << 13, BITSET30 = 1 << 29,	// BITSET46 = 1 << 45,	BITSET62 = 1 << 61,
	BITSET15 = 1 << 14, BITSET31 = 1 << 30,	// BITSET47 = 1 << 46,	BITSET63 = 1 << 62,
	BITSET16 = 1 << 15, BITSET32 = 1 << 31	// BITSET48 = 1 << 47,	BITSET64 = 1 << 63

}; // Enums are hard truncated to 32bit by default, just does not throw eror in MSVC
// To get 64bit you have to define the size (enum MyEnum : <integer_type>) but I can' make it work

#define SET_FLAG(n, f) ((n) |= (f)) 
#define CLR_FLAG(n, f) ((n) &= ~(f))
#define APPLY_MASK(n, f) ((n) &= (f))
#define TGL_FLAG(n, f) ((n) ^= (f)) 
#define CHK_FLAG(n, f) (((n) & (f)) > 0)

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFAULT TYPES /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char uchar;

typedef struct float4
{
	float4() : x(0), y(0), z(0), w(0) {};
	float4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {};
	float x, y, z, w;
} float4;
typedef struct float3
{
	float3() : x(0), y(0), z(0) {};
	float3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};
	float x, y, z;
} float3;
typedef struct float2
{
	float2() : x(0), y(0) {};
	float2(float x_, float y_) : x(x_), y(y_) {};
	float x, y;
} float2;

typedef struct int4
{
	int4() : x(0), y(0), z(0), w(0) {};
	int4(int32 x_, int32 y_, int32 z_, int32 w_) : x(x_), y(y_), z(z_), w(w_) {};
	int32 x, y, z, w;
} int4;
typedef struct int3
{
	int3() : x(0), y(0), z(0) {};
	int3(int32 x_, int32 y_, int32 z_) : x(x_), y(y_), z(z_) {};
	int32 x, y, z;
} int3;
typedef struct int2
{
	int2() : x(0), y(0) {};
	int2(int32 x_, int32 y_) : x(x_), y(y_) {};
	int32 x, y;
} int2;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION TYPES ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Core
typedef void(*VoidFun)();

// Input
typedef void(*KeyCallback)(int32 code, int32 state);
typedef void(*MouseFloatCallback)(float x, float y);

#endif