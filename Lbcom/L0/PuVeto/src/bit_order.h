// 2009/07/13 by Roel Aaij
// function to reverse the bit order

// Include files 
#include <iostream>
#include <limits>

template< typename T >
void print_bit( const T& number );

template< typename T >
void reverse_bit( T& number );

using namespace std;

//===========================
/*
int main() {

   unsigned int a = 19;
   print_bit( a );
   reverse_bit( a );

   print_bit( a );
   
   return 0;
}
*/
//===========================

template< typename T >
void reverse_bit( T& number ) {

   T mask = 1;
   T reverse = 0;

   const int bits = numeric_limits< int >::digits;

   for ( int i = 0; i < bits; ++i ) {
      reverse = reverse | ( mask & number );
      reverse = reverse << 1;
      number = number >> 1;
   }
   number = reverse;
}

template< typename T >
void print_bit( const T& number ) {

   const int bits = numeric_limits< int >::digits;

   T mask = 1;
   mask = mask << bits;

   for ( unsigned int i = 0; i < bits + 1; ++i ) {
      T t = number & mask;
      cout << ( t == mask );
      mask = mask >> 1;
      if ( i % 4 == 3 ) cout << " ";
   }
   cout << endl;
}
//===========================
