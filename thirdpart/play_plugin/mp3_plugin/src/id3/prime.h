#ifndef __PRIME_H__
#define __PRIME_H__ 

#include "vlong.h"

class prime_factory
{
public:
	DWORD np;
	DWORD *pl;
	prime_factory( DWORD MP = 2000 ); // sieve size
	~prime_factory();
	vlong find_prime( vlong &start );
	long make_prime( vlong & r, vlong &k, const vlong & rmin );
};

long is_probable_prime( const vlong &p );

#endif