// $Id: CaloPrint.h,v 1.1 2002-04-22 16:49:51 ibelyaev Exp $
// ============================================================================
// CV stag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef KERNEL_CALOPRINT_H
#define KERNEL_CALOPRINT_H 1
// Include  files
// STD & STL
#include <stdio.h>
#include <string>

class Hep3Vector       ;  ///< CLHEP/Vector
class HepTransform3D   ;  ///< CLHEP/Geometry
class HepRotation      ;  ///< CLHEP/Vector
class HepLorentzVector ;  ///< CLHEP/Vector
class HepVector        ;  ///< CLHEP/Matrix 
class HepGenMatrix     ;  ///< CLHEP/Matrix
class HepSymMatrix     ;  ///< CLHEP/Matrix
class HepDiagMatrix    ;  ///< CLHEP/Matrix


/** @class CaloPrint CaloPrint.h Kernel/CaloPrint.h
 *
 *  Trivial utility class to perform formatted output to the string
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *
 *  @date   04/11/2001
 */
class CaloPrint
{
  
public:
  
  /** general printout interface
   *  @param  value  value to be printed
   *  @return string representation of the type
   */
  template <class TYPE>
  std::string operator() ( const TYPE& value ) const
  { return print( value ); }

  /** the main method for integers 
   *  @param value  value to be printed
   *  @param format format
   *  @return string representation of the value according to format
   */
  inline std::string print ( const int            value      ,
                             const char*          format = 0 ) const
  {
    char* aux = storage() ;
    if( 0 == format ) { format = formatInteger ()         ; } 
    else              { setFormatInteger       ( format ) ; }
    return std::string( aux , aux + sprintf( aux , format , value ) );
  };
  
  /** the main method for unsigned integers  
   *  @param value  value to be printed
   *  @param format format
   *  @return string representation of the value according to format
   */
  inline std::string print ( const unsigned int   value       ,
                             const char*          format = 0  ) const
  {
    char* aux = storage() ;
    if( 0 == format ) { format = formatInteger ()         ; } 
    else              { setFormatInteger       ( format ) ; }
    return std::string( aux , aux + sprintf( aux , format , value ) );
  };
  
  /** the main method for doubles 
   *  @param value  value to be printed
   *  @param format format
   *  @return string representation of the value according to format
   */
  inline std::string print ( const double    value ,
                             const char*     format = 0  ) const
  {
    char* aux = storage() ;
    if( 0 == format ) { format = formatDouble  ()         ; } 
    else              { setFormatDouble        ( format ) ; }
    return std::string( aux , aux + sprintf( aux , format , value ) );
  };
  
  /** print of Hep3Vector object
   *  @param value   vector to be printed
   *  @return string representation of Hep3Vector object
   */
  std::string print ( const Hep3Vector&       value ) const ;
  
  /** print of HepLorentzVector object
   *  @param value   vector to be printed
   *  @return string representation of HepLorentzVector object
   */
  std::string print ( const HepLorentzVector& value ) const ;
  
  /** print of HepRotation object
   *  @param value   rotation to be printed
   *  @return string representation of HepRotation object
   */
  std::string print ( const HepRotation&      value ) const ;
  
  /** print of HepTransform3D object
   *  @param value   transformation to be printed
   *  @return string representation of HepTransform3D object
   */
  std::string print ( const HepTransform3D&   value ) const ;
  
  /** print of HepVector object
   *  @param value   vector to be printed
   *  @return string representation of Hep3Vector object
   */
  std::string print ( const HepVector&        value ) const ;

  /** print of Symmetric Matrix object
   *  @param value   symmetric  matrix to be printed
   *  @return string representation of HepSymMatrix object
   */
  std::string print ( const HepSymMatrix&     value ) const ;

  /** print of Diagonal Matrix object
   *  @param value   symmetric  matrix to be printed
   *  @return string representation of HepDiagMatrix object
   */
  std::string print ( const HepDiagMatrix&    value ) const ;

  /** print of GenMatrix object
   *  @param value   general matrix to be printed
   *  @return string representation of HepGenMatrix object
   */
  std::string print ( const HepGenMatrix&     value ) const ;
  
public:

  /** set/reset defualt format for integers 
   *  @param format new default format for integers 
   *  @return value of  default format for integers 
   */
  static const char* setFormatInteger ( const std::string& format = "%d"     ) ;
  
  /** set/reset defualt format for doubles 
   *  @param format new default format for doubles 
   *  @return value of  default format for doubles 
   */
  static const char* setFormatDouble  ( const std::string& format = "%9.3g" ) ;
  
  /** accessor to format for integers 
   *  @return format for integers 
   */
  static const char* formatInteger    () ;
  
  /** accessor to format for doubles 
   *  @return format for doubles 
   */
  static const char* formatDouble     () ;
  
private:
  
  /** accessor to static storage
   *  @retunr pointer to internal static storage 
   */
  static char* storage()        ;
  
private:

  static std::string s_formatDouble   ;
  static std::string s_formatInteger  ;
  
};

// ============================================================================
// The End
// ============================================================================
#endif /// CKERNEL_CALOPRINT_H
// ============================================================================
