#ifndef SQUE_SIMPLE_TYPES
#define SQUE_SIMPLE_TYPES

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// BIT MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SQUE_BitFlags
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

#define EXPAND_AS_ENUM(a,b) a,
#define EXPAND_AS_VALUE(a,b) b,
// In corner cases need more than 2 values, it is pushing a bit really but it should work
#define X3_EXPAND_1(a,b,c) a,
#define X3_EXPAND_2(a,b,c) b,
#define X3_EXPAND_3(a,b,c) c,

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFAULT ENUMS /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SQUE_LogType
{
	LT_INFO = 4,
	LT_WARNING,
	LT_ERROR,
	LT_CRITICAL

};

enum SQUE_INIT_Flags
{
	SQ_INIT_DEFAULTS = BITSET1,
	SQ_INIT_OPENWINDOW = BITSET2,
	SQ_INIT_MAX_RENDER_VER = BITSET3,
	SQ_INIT_MIN_RENDER_VER = BITSET4,
	SQ_INIT_DEBUG_RENDER = BITSET5,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFAULT TYPES /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION TYPES ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Core
typedef void(*VoidFun)();
typedef void VoidFun2();
#define VoidFun_Empty [](){}

// Display
typedef void(*ResizeCallback)(int32_t width, int height);
typedef void(*ViewportSizeCallback)(int32_t* width, int32_t* height);
// Input
typedef void(*KeyCallback)(int32_t code, int32_t state);
typedef void(*MouseFloatCallback)(float x, float y);

typedef void(HandleDropFileFun)(const char* location);

#endif