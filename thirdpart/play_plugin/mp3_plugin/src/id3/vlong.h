#ifndef __VLONG_H__
#define __VLONG_H__ 

#ifndef NULL
#define NULL 0
#endif

#ifdef _MSC_VER
#include <Windows.h>
#else
//typedef unsigned long DWORD
#ifndef DWORD
#define DWORD unsigned long
#endif
#endif

// Macros for doing double precision multiply
#define BPU ( 8*sizeof(DWORD) )       // 一个字节位 DWORD
#define lo(x) ( ((DWORD)(x)) & (DWORD)((((DWORD)1)<<(BPU/2))-((DWORD)1)) ) // lower half of DWORD
#define hi(x) ( ((DWORD)(x)) >> (BPU/2) )         // 生 1/2
#define lh(x) ( ((DWORD)(x)) << (BPU/2) )         // 实施

// Provides storage allocation and index checking
class flex_unit
{
public:
	DWORD n; // used units (read-only)
	flex_unit();
	~flex_unit();
	void clear(); // set n to zero
	DWORD get( DWORD i ) const;     // 获取 ith DWORD
	void set( DWORD i, DWORD x );   // 设置 ith DWORD
	void reserve( DWORD x );           // storage hint

	// Time critical routine
	void fast_mul( flex_unit &x, flex_unit &y, DWORD n );
	//private: //lchen modi
	DWORD * a; // array of units
	DWORD z; // units allocated
};

class vlong_value : public flex_unit
{
public:
	DWORD share; // share count, used by vlong to delay physical copying
	long is_zero() const;
	DWORD bit( DWORD i ) const;
	void setbit( DWORD i );
	void clearbit( DWORD i );
	DWORD bits() const;
	long cf( vlong_value& x ) const;
	long product( vlong_value &x ) const;
	void shl();
	long  shr(); // result is carry
	void shr( DWORD n );
	void add( vlong_value &x );
	void _xor( vlong_value &x );
	void _and( vlong_value &x );
	void subtract( vlong_value& x );
	void init( DWORD x );
	void copy( vlong_value &x );
	DWORD to_unsigned(); // Unsafe conversion to DWORD
	vlong_value();
	void mul( vlong_value &x, vlong_value &y );
	void divide( vlong_value &x, vlong_value &y, vlong_value &rem );
};

class vlong // very long integer - can be used like long
{
public:
	// Standard arithmetic operators
	friend vlong operator +( const vlong& x, const vlong& y );
	friend vlong operator -( const vlong& x, const vlong& y );
	friend vlong operator *( const vlong& x, const vlong& y );
	friend vlong operator /( const vlong& x, const vlong& y );
	friend vlong operator %( const vlong& x, const vlong& y );
	friend vlong operator ^( const vlong& x, const vlong& y );
	friend vlong pow2( DWORD n );
	friend vlong operator &( const vlong& x, const vlong& y );

	friend vlong operator <<( const vlong& x, DWORD n );

	vlong& operator +=( const vlong& x );
	vlong& operator -=( const vlong& x );
	vlong& operator >>=( DWORD n );

	// Standard comparison operators
	friend long operator !=( const vlong& x, const vlong& y );
	friend long operator ==( const vlong& x, const vlong& y );
	friend long operator >=( const vlong& x, const vlong& y );
	friend long operator <=( const vlong& x, const vlong& y );
	friend long operator > ( const vlong& x, const vlong& y );
	friend long operator < ( const vlong& x, const vlong& y );

	// Absolute value
	friend vlong abs( const vlong & x );

	// Construction and conversion operations
	vlong ( DWORD x=0 );
	vlong ( const vlong& x );
	~vlong();
	friend DWORD to_unsigned( const vlong &x );
	vlong& operator =(const vlong& x);

	// Bit operations
	DWORD bits() const;
	DWORD bit(DWORD i) const;
	void setbit(DWORD i);
	void clearbit(DWORD i);
	vlong& operator ^=( const vlong& x );
	vlong& operator &=( const vlong& x );
	vlong& ror( DWORD n ); // single  bit rotate
	vlong& rol( DWORD n ); // single bit rotate
	friend long product( const vlong & x, const vlong & y ); // parity of x&y
    
	void load( DWORD * a, DWORD n ); // 读值, a[0]
	void store( DWORD * a, DWORD n ) const; // low level save, a[0] is lsw
	void load( char * a, DWORD n );	//自己添加
	void store( char * a, DWORD n ) const;	//自己添加


	//private:
	class vlong_value * value;
	long negative;
	long cf( const vlong & x ) const;
	void docopy();
	friend class monty;
};

vlong modexp( const vlong & x, const vlong & e, const vlong & m ); // m 必须已添加

vlong gcd( const vlong &X, const vlong &Y ); // greatest common denominator
vlong modinv( const vlong &a, const vlong &m ); // modular inverse

vlong monty_exp( const vlong & x, const vlong & e, const vlong & m );
vlong monty_exp( const vlong & x, const vlong & e, const vlong & m, const vlong &p, const vlong &q );

class rng
{
public:
	virtual vlong next()=0;
};

class vlstr
{
public:
	virtual void put( const vlong & x )=0;
	virtual vlong get()=0;
};

vlong lucas ( vlong P, vlong Z, vlong k, vlong p ); // P^2 - 4Z != 0
vlong sqrt( vlong g, vlong p ); // 平方根的模 p


#endif// __VLONG_H__