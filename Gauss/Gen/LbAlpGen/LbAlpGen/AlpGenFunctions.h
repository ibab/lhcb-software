#ifndef LBALPGEN_ALPGENFUNCTIONS_H 
#define LBALPGEN_ALPGENFUNCTIONS_H 1

/** @class AlpGenFunctions AlpGenFunctions.h LbAlpGen/AlpGenFunctions.h
 *
 * Utility class to access AlpGen FORTRAN functions
 *
 * @author Patrick Robbe
 * @author Stephane Tourneur
 * @date   2012-07-13
 */
class AlpGenFunctions {
public:
  static void AlSprc( ) ; ///< alsprc function
  static void AlSdef( ) ;
  static void AlHset( ) ;
  static void AlInit( ) ;
  static void AlSbkk( ) ;
  static void AlShis( ) ;
  static void AlSgrd( ) ;
  static void AlIgrd( ) ;
  static void AlEgen( ) ;
  static void AlFhis( ) ;
  static void AlFbkk( ) ;
  static void AlSfin( ) ;
} ;
#endif
