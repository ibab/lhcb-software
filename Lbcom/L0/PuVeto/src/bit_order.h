// 2010/02/02 Serena Oggero: bug fixed 
// 2009/07/13 by Roel Aaij

// function to reverse the bit order

// Include files 
#include <iostream>
#include <limits>

template< typename T >
void print_bit( T& number );

template< typename T >
void reverse_bit( T& number );

using namespace std;

//===========================

template< typename T >
void reverse_bit( T& number ) {

   T mask = 1;
   T reverse = 0;
   T numberTmp = number;
   
   const int bits = numeric_limits< int >::digits;
   // bits = 31
   
   for ( int i = 0; i < bits; ++i ) {
      reverse = reverse | ( mask & numberTmp );
      reverse = reverse << 1;
      numberTmp = numberTmp >> 1;
   }
   if ( reverse != 0 ) number = reverse | numberTmp;
   else number = numberTmp;
   //number = reverse;
}


template< typename T >
void print_bit( T& number ) {

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
