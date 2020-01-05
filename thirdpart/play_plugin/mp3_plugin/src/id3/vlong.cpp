//#include "stdafx.h"
#include "vlong.h"
static void assert ( long x )
{
	if (!x)
	{
		char * z=0;
		*z=0;
	}
}

unsigned char bittab[256] =
{
	0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

flex_unit::flex_unit()
{
	z = 0;
	a = 0;
	n = 0;
}

DWORD flex_unit::get( DWORD i ) const
{
	if ( i >= n ) return 0;
	return a[i];
}

void flex_unit::clear()
{
	n = 0;
}



flex_unit::~flex_unit()
{
	DWORD i=z;
	while (i) { i-=1; a[i] = 0; } // burn
	delete [] a;
}

void flex_unit::reserve( DWORD x )
{
	if (x > z)
	{
		DWORD * na = new DWORD[x];
		for (DWORD i=0; i<n; i+=1) na[i] = a[i];
		if(a != NULL)
			delete [] a;
		a = na;
		z = x;
	}
}

void flex_unit::set( DWORD i, DWORD x )
{
	if ( i < n )
	{
		a[i] = x;
		if (x==0)
		{
			while (n && (a[n-1]==0))
				n-=1; // normalise
		}
	}
	else if ( x )
	{
		reserve(i+1);
		for (DWORD j=n;j<i;j+=1)
			a[j] = 0;
		a[i] = x;
		n = i+1;
	}
}

void flex_unit::fast_mul( flex_unit &x, flex_unit &y, DWORD keep )
{
	// *this = (x*y) % (2**keep)
	DWORD i,limit = (keep+BPU-1)/BPU; // size of result in words
	reserve(limit); for (i=0; i<limit; i+=1) a[i] = 0;
	DWORD min = x.n; if (min>limit) min = limit;
	for (i=0; i<min; i+=1)
	{
		DWORD m = x.a[i];
		DWORD min = i+y.n; if (min>limit) min = limit;
		DWORD c = 0; // carry
		DWORD j;
		for ( j=i; j<min; j+=1 )
		{
			// This is the critical loop
			// Machine dependent code could help here
			// c:a[j] = a[j] + c + m*y.a[j-i];
			DWORD w, v = a[j], p = y.a[j-i];
			v += c; c = ( v < c );
			w = lo(p)*lo(m); v += w; c += ( v < w );
			w = lo(p)*hi(m); c += hi(w); w = lh(w); v += w; c += ( v < w );
			w = hi(p)*lo(m); c += hi(w); w = lh(w); v += w; c += ( v < w );
			c += hi(p) * hi(m);
			a[j] = v;
		}
		while ( c && (j<limit ) )
		{
			a[j] += c;
			c = a[j] < c;
			j += 1;
		}
	}

	// eliminate unwanted bits
	keep %= BPU;
	if (keep)
		a[limit-1] &= (( (DWORD)1 << keep)-1);

	// calculate n
	while (limit && (a[limit-1]==0))
		limit-=1;
	n = limit;
};

DWORD vlong_value::to_unsigned()
{
	return get(0);
}

long vlong_value::is_zero() const
{
	return n==0;
}

DWORD vlong_value::bit( DWORD i ) const
{ return ( get(i/BPU) & ((DWORD)1<<(i%BPU)) ) != 0; }

void vlong_value::setbit(DWORD i)
{
	set( i/BPU, get(i/BPU) | ((DWORD)1<<i%BPU) );
}

void vlong_value::clearbit(DWORD i)
{
	set( i/BPU, get(i/BPU) & ~((DWORD)1<<i%BPU) );
}

DWORD vlong_value::bits() const
{
	// DWORD x = n*BPU;
	// while (x && bit(x-1)==0) x -= 1;

	// slightly more efficient version
	DWORD x = n;
	if (x)
	{
		DWORD msw = get(x-1);
		x = (x-1)*BPU;
		DWORD w = BPU;
		do
		{
			w >>= 1;
			if ( msw>= (1u<<w) )
			{
				x += w;
				msw >>= w;
			}
		} while (w>8);
		x += bittab[msw];
	}
	return x;
}

long vlong_value::cf( vlong_value& x ) const
{
	if ( n > x.n ) return +1;
	if ( n < x.n ) return -1;
	DWORD i = n;
	while (i)
	{
		i -= 1;
		if ( get(i) > x.get(i) ) return +1;
		if ( get(i) < x.get(i) ) return -1;
	}
	return 0;
}

void vlong_value::shl()
{
	DWORD carry = 0;
	DWORD N = n; // necessary, since n can change
	for (DWORD i=0;i<=N;i+=1)
	{
		DWORD u = get(i);
		set(i,(u<<1)+carry);
		carry = u>>(BPU-1);
	}
}

long vlong_value::shr()
{
	DWORD carry = 0;
	DWORD i=n;
	while (i)
	{
		i -= 1;
		DWORD u = get(i);
		set(i,(u>>1)+carry);
		carry = u<<(BPU-1);
	}
	return carry != 0;
}

void vlong_value::shr( DWORD x )
{
	DWORD delta = x/BPU; x %= BPU;
	for (DWORD i=0;i<n;i+=1)
	{
		DWORD u = get(i+delta);
		if (x)
		{
			u >>= x;
			u += (get(i+delta+1) << (BPU-x));
		}
		set(i,u);
	}
}

void vlong_value::add( vlong_value & x )
{
	DWORD carry = 0;
	DWORD max = n; if (max<x.n) max = x.n;
	reserve(max);
	for (DWORD i=0;i<max+1;i+=1)
	{
		DWORD u = get(i);
		u = u + carry; carry = ( u < carry );
		DWORD ux = x.get(i);
		u = u + ux; carry += ( u < ux );
		set(i,u);
	}
}

void vlong_value::_xor( vlong_value & x )
{
	DWORD max = n; if (max<x.n) max = x.n;
	reserve(max);
	for (DWORD i=0;i<max;i+=1)
	{
		set(i,get(i)^x.get(i));
	}
}

void vlong_value::_and( vlong_value & x )
{
	DWORD max = n; if (max<x.n) max = x.n;
	reserve(max);
	for (DWORD i=0;i<max;i+=1)
	{
		set(i,get(i)&x.get(i));
	}
}

long vlong_value::product( vlong_value &x ) const
{
	DWORD max = n; if (max<x.n) max = x.n;
	DWORD tmp = 0;
	for (DWORD i=0;i<max;i+=1)
	{
		tmp ^= (get(i)&x.get(i));
	}
	DWORD count = 0;
	while (tmp)
	{
		if (tmp&1) count += 1;
		tmp >>= 1;
	}
	return count&1;
}

void vlong_value::subtract( vlong_value & x )
{
	DWORD carry = 0;
	DWORD N = n;
	for (DWORD i=0;i<N;i+=1)
	{
		DWORD ux = x.get(i);
		ux += carry;
		if ( ux >= carry )
		{
			DWORD u = get(i);
			DWORD nu = u - ux;
			carry = nu > u;
			set(i,nu);
		}
	}
}

void vlong_value::init( DWORD x )
{
	clear();
	set(0,x);
}

void vlong_value::copy( vlong_value& x )
{
	clear();
	DWORD i=x.n;
	while (i) { i -= 1; set( i, x.get(i) ); }
}

vlong_value::vlong_value()
{
	share = 0;
}

void vlong_value::mul( vlong_value& x, vlong_value& y )
{
	fast_mul( x, y, x.bits()+y.bits() );
}

void vlong_value::divide( vlong_value& x, vlong_value& y, vlong_value& rem )
{
	init(0);
	rem.copy(x);
	vlong_value m,s;
	m.copy(y);
	s.init(1);
	while ( rem.cf(m) > 0 )
	{
		m.shl();
		s.shl();
	}
	while ( rem.cf(y) >= 0 )
	{
		while ( rem.cf(m) < 0 )
		{
			m.shr();
			s.shr();
		}
		rem.subtract( m );
		add( s );
	}
}

// Implementation of vlong

long operator !=( const vlong& x, const vlong& y ){ return x.cf( y ) != 0; }
long operator ==( const vlong& x, const vlong& y ){ return x.cf( y ) == 0; }
long operator >=( const vlong& x, const vlong& y ){ return x.cf( y ) >= 0; }
long operator <=( const vlong& x, const vlong& y ){ return x.cf( y ) <= 0; }
long operator > ( const vlong& x, const vlong& y ){ return x.cf( y ) > 0; }
long operator < ( const vlong& x, const vlong& y ){ return x.cf( y ) < 0; }

void vlong::load( DWORD * a, DWORD n )
{
	docopy();
	value->clear();
	for (DWORD i=0;i<n;i+=1)
		value->set(i,a[i]);
}

void vlong::store( DWORD * a, DWORD n ) const
{
	for (DWORD i=0;i<n;i+=1)
		a[i] = value->get(i);
}

void vlong::load( char * a, DWORD n )	//自己添加
{
	if( (n % 2) == 1)
	{
		n++;
	}	//加多一个字节，应该还是能还原的
	load( (DWORD *)a, n / 2);
}

void vlong::store( char * a, DWORD n ) const	//自己添加
{
	store( (DWORD *)a, n / 2);
}


void vlong::docopy()
{
	if ( value->share )
	{
		value->share -= 1;
		vlong_value * nv = new vlong_value;
		nv->copy(*value);
		value = nv;
	}
}

DWORD vlong::bits() const
{
	return value->bits();
}

DWORD vlong::bit(DWORD i) const
{
	return value->bit(i);
}

void vlong::setbit(DWORD i)
{
	docopy();
	value->setbit(i);
}

void vlong::clearbit(DWORD i)
{
	docopy();
	value->clearbit(i);
}

long vlong::cf( const vlong & x ) const
{
	long neg = negative && (!value->is_zero());
	if ( neg == (x.negative && !x.value->is_zero()) )
		return value->cf( *x.value );
	else if ( neg ) return -1;
	else return +1;
}

vlong::vlong (DWORD x)
{
	value = new vlong_value;
	negative = 0;
	value->init(x);
}

vlong::vlong ( const vlong& x ) // copy constructor
{
	negative = x.negative;
	value = x.value;
	value->share += 1;
}

vlong& vlong::operator =(const vlong& x)
{
	if ( value->share ) value->share -=1; else delete value;
	value = x.value;
	value->share += 1;
	negative = x.negative;
	return *this;
}

vlong::~vlong()
{
	if ( value->share ) value->share -=1; else delete value;
}

DWORD to_unsigned ( const vlong & x ) // conversion to DWORD
{
	return x.value->to_unsigned();
}

vlong& vlong::operator ^=(const vlong& x)
{
	docopy();
	value->_xor( *x.value );
	return *this;
}

vlong& vlong::operator &=(const vlong& x)
{
	docopy();
	value->_and( *x.value );
	return *this;
}

vlong& vlong::ror(DWORD n)
{
	docopy();
	long carry = value->shr();
	if (carry)
		value->setbit(n-1);
	return *this;
}

vlong& vlong::rol(DWORD n)
{
	docopy();
	long carry = value->bit(n-1);
	if (carry) value->clearbit(n-1);
	value->shl();
	if (carry) value->setbit(0);
	return *this;
}

vlong& vlong::operator >>=( DWORD n ) // divide by 2**n
{
	docopy();
	value->shr(n);
	return *this;
}

vlong& vlong::operator +=(const vlong& x)
{
	if ( negative == x.negative )
	{
		docopy();
		value->add( *x.value );
	}
	else if ( value->cf( *x.value ) >= 0 )
	{
		docopy();
		value->subtract( *x.value );
	}
	else
	{
		vlong tmp = *this;
		*this = x;
		*this += tmp;
	}
	return *this;
}

vlong& vlong::operator -=(const vlong& x)
{
	if ( negative != x.negative )
	{
		docopy();
		value->add( *x.value );
	}
	else if ( value->cf( *x.value ) >= 0 )
	{
		docopy();
		value->subtract( *x.value );
	}
	else
	{
		vlong tmp = *this;
		*this = x;
		*this -= tmp;
		negative = 1 - negative;
	}
	return *this;
}

vlong operator +( const vlong& x, const vlong& y )
{
	vlong result = x;
	result += y;
	return result;
}

vlong operator -( const vlong& x, const vlong& y )
{
	vlong result = x;
	result -= y;
	return result;
}

vlong operator *( const vlong& x, const vlong& y )
{
	vlong result;
	result.value->mul( *x.value, *y.value );
	result.negative = x.negative ^ y.negative;
	return result;
}

vlong operator /( const vlong& x, const vlong& y )
{
	vlong result;
	vlong_value rem;
	result.value->divide( *x.value, *y.value, rem );
	result.negative = x.negative ^ y.negative;
	return result;
}

vlong operator %( const vlong& x, const vlong& y )
{
	vlong result;
	vlong_value divide;
	divide.divide( *x.value, *y.value, *result.value );
	result.negative = x.negative; // not sure about this?
	return result;
}

vlong operator ^( const vlong& x, const vlong& y )
{
	vlong result = x;
	result ^= y;
	return result;
}

vlong operator &( const vlong& x, const vlong& y )
{
	vlong result = x;
	result &= y;
	return result;
}

vlong operator <<( const vlong& x, DWORD n ) // multiply by 2**n
{
	vlong result = x;
	while (n)
	{
		n -= 1;
		result += result;
	}
	return result;
}


vlong abs( const vlong & x )
{
	vlong result = x;
	result.negative = 0;
	return result;
}

long product ( const vlong &X, const vlong &Y )
{
	return X.value->product( *Y.value );
}

vlong pow2( DWORD n )
{
	vlong result;
	result.setbit(n);
	return result;
}

vlong gcd( const vlong &X, const vlong &Y )
{
	vlong x=X, y=Y;
	while (1)
	{
		if ( y == 0 ) return x;
		x = x % y;
		if ( x == 0 ) return y;
		y = y % x;
	}
}

vlong modinv( const vlong &a, const vlong &m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
	vlong j=1,i=0,b=m,c=a,x,y;
	while ( c != 0 )
	{
		x = b / c;
		y = b - x*c;
		b = c;
		c = y;
		y = j;
		j = i - j*x;
		i = y;
	}
	if ( i < 0 )
		i += m;
	return i;
}

class monty // class for montgomery modular exponentiation
{
	vlong m,n1;
	vlong T,k;   // work registers
	DWORD N;  // bits for R
	void mul( vlong &x, const vlong &y );
public:
	vlong R,R1;
	vlong exp( const vlong &x, const vlong &e );
	vlong monty_exp( const vlong &x, const vlong &e );
	monty( const vlong &M );
};

monty::monty( const vlong &M )
{
	m = M;
	N = 0; R = 1;
	while ( R < M )
	{
		R += R;
		N += 1;
	}
	R1 = modinv( R-m, m );
	n1 = R - modinv( m, R );
}

void monty::mul( vlong &x, const vlong &y )
{
	// T = x*y;
	T.value->fast_mul( *x.value, *y.value, N*2 );

	// k = ( T * n1 ) % R;
	k.value->fast_mul( *T.value, *n1.value, N );

	// x = ( T + k*m ) / R;
	x.value->fast_mul( *k.value, *m.value, N*2 );
	x += T;
	x.value->shr( N );

	if (x>=m) x -= m;
}

vlong monty::monty_exp( const vlong &x, const vlong &e )
{
	vlong result = R-m, t = x; // don't convert input
	t.docopy(); // careful not to modify input
	DWORD bits = e.value->bits();
	DWORD i = 0;
	while (1)
	{
		if ( e.value->bit(i) )
			mul( result, t);
		i += 1;
		if ( i == bits ) break;
		mul( t, t );
	}
	return result; // don't convert output
}

vlong monty::exp( const vlong &x, const vlong &e )
{
	return ( monty_exp( (x*R)%m, e ) * R1 ) % m;
}

//// 求 x的e次幂，再对m求模
vlong modexp( const vlong & x, const vlong & e, const vlong & m )
{
	monty me(m);
	return me.exp( x,e );
}

vlong monty_exp( const vlong & x, const vlong & e, const vlong & m )
{
	monty me(m);
	return me.monty_exp( x,e );
}

vlong monty_exp( const vlong & x, const vlong & d, const vlong & m, const vlong &p, const vlong &q )
{
	monty me(m);
	vlong x1 = ( x * me.R1 ) % m; // Transform input

	vlong u = modinv( p, q );
	vlong dp = d % (p-1);
	vlong dq = d % (q-1);

	// Apply chinese remainder theorem
	vlong a = modexp( x1 % p, dp, p );
	vlong b = modexp( x1 % q, dq, q );
	if ( b < a ) b += q;
	vlong result = a + p * ( ((b-a)*u) % q );

	// Transform result
	return ( result * me.R ) % m;
}

static vlong half(const vlong &a, vlong p)
{
	if (a.bit(0))
		return (a+p)/2;
	return a/2;
}

vlong lucas ( vlong P, vlong Z, vlong k, vlong p ) // P^2 - 4Z != 0
{
	vlong D = P*P - 4*Z, U = 1, V = P, U1,V1;
	DWORD i=k.bits()-1;
	while (i)
	{
		i -= 1;
		U1 = U*V; V1 = V*V + D*U*U; U = U1%p; V = half(V1%p,p);
		if ( k.bit(i) )
		{
			U1 = P*U+V; V1 = P*V+D*U; U = half(U1%p,p); V = half(V1%p,p);
		}
	}
	return V;
}

vlong sqrt( vlong g, vlong p )
{
	vlong result;
	if (p%4==3)
		result = modexp( g, p/4+1, p );
	else if (p%8==5)
	{
		vlong gamma = modexp( 2*g, p/8, p);
		vlong i = 2*g*gamma*gamma - 1;
		result = g*gamma*i;
	}
	else
	{
		vlong Z = g;
		vlong P = 1;
		while (1)
		{
			vlong D = (P*P-4*g)%p; if (D<0) D += p;
			if ( D==0 )
			{
				result = half(P,p);
				break;
			}
			if ( modexp( D, (p-1)/2, p ) != 1 )
			{
				result = half( lucas( P, Z, (p+1)/2, p ), p );
				break;
			}
			P += 1; // Is this ok (efficient?)
		}
	}
	result = result % p;
	if ( result < 0 ) result += p;

	return result;
}