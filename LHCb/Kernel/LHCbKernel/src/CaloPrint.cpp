// $Id: CaloPrint.cpp,v 1.2 2005-01-26 07:15:08 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// Include files
#include "Kernel/CaloPrint.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

// ============================================================================
/** @file CaloPrint.cpp
 *
 *  Implementation file for class : CaloPrint
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 04/11/2001 
 */
// ============================================================================

// ============================================================================
/** return the pointer to default static storage 
 */ 
// ============================================================================
char* CaloPrint::storage() 
{
  // static storage 
  static char s_tmp[256];
  // 
  return s_tmp;
};

// ============================================================================
/** initialize static members 
 */
// ============================================================================
std::string CaloPrint::s_formatDouble  = "%9.3g" ;
std::string CaloPrint::s_formatInteger = "%d"     ;
	
// ============================================================================
/**	accessor to format for integers 
  *  @return format for integers 
  */
// ============================================================================
const char* CaloPrint::formatInteger () { return s_formatInteger.c_str() ; }
  
// ============================================================================
/** accessor to format for doubles 
 *  @return format for doubles 
 */
// ============================================================================
const char* CaloPrint::formatDouble  () { return s_formatDouble.c_str()  ; }

// ============================================================================
/** set/reset defualt format for integers 
 *  @param format new default format for integers 
 *  @return value of  default format for integers 
 */
// ============================================================================
const char* CaloPrint::setFormatInteger ( const std::string& format ) 
{
  s_formatInteger = format ;
  return formatInteger();
};

// ============================================================================
/** set/reset defualt format for doubles 
 *  @param format new default format for doubles 
 *  @return value of  default format for doubles 
 */
// ============================================================================
const char* CaloPrint::setFormatDouble  ( const std::string& format ) 
{
  s_formatDouble = format ;
  return formatDouble();
};

// ============================================================================
/** print of Hep3Vector object
 *  @param value   vector to be printed
 *  @return string representation of Hep3Vector object
 */
// ============================================================================
std::string CaloPrint::print ( const Hep3Vector& value  ) const
{
  return 
    0 == value.mag2()                                  ? 
    std::string( " ( null 3D-vector ) " )              :
    std::string( " ( " + print( value.x() ) + 
                 ", "  + print( value.y() ) + 
                 ", "  + print( value.z() ) + " ) " )  ;
};

// ============================================================================
/** print of HepLorentzVector object
 *  @param value   vector to be printed
 *  @return string representation of HepLorentzVector object
 */
// ============================================================================
std::string CaloPrint::print ( const HepLorentzVector& value  ) const
{
  return 
    ( 0 == value.e() && 0 == value.vect().mag2() )    ?  
    std::string( " { null 4-vector } " )              :
    std::string( " {" + print( value.vect() ) +  
                 "/ " + print( value.e   () ) + " } " ) ;
};

// ============================================================================
/** print of HepRotation object
 *  @param value   rotation to be printed
 *  @return string representation of HepRotation object
 */
// ============================================================================
std::string CaloPrint::print ( const HepRotation&      value  ) const
{
  /// check the trivial case 
  if( value.isIdentity() )
    { return std::string( " Rotation:{ identity } ") ;}
  ///  
  Hep3Vector vector;
  double     angle ;
  value.getAngleAxis( angle , vector );
  /// print the spatial components and than the e/t component
  return std::string( " Rotation:{ Angle:" + print( angle  ) +  
                      " Axis:"             + print( vector ) + "} " );
};

// ============================================================================
/** print of HepTransform3D object
 *  @param value   transformation to be printed
 *  @return string representation of HepTransform3D object
 */
// ============================================================================
std::string CaloPrint::print ( const HepTransform3D&   value  ) const 
{
  ///
  return 
    HepTransform3D::Identity == value              ? 
    std::string( " Transformation:{ identity } " ) :
    std::string( " Transformation:{ Translation:" 
                 + print( value.getTranslation () ) 
                 + ","
                 + print( value.getRotation    () )
                 + "} " );
};

// ============================================================================
/** print of HepVector object
 *  @param value   vector to be printed
 *  @return string representation of Hep3Vector object
 */
// ============================================================================
std::string CaloPrint::print ( const HepVector&        value  ) const 
{
  ///
  const unsigned int nRow = value.num_row() ;
  std::string tmp( " ["+print(nRow)+"]{ " );
  /// special case 
  if( 0 == value.normsq() ) 
    { return tmp += "null vector } ";}
  ///
  for( unsigned int nrow = 1 ; nrow <= nRow ; ++nrow )
    {
      tmp += print( value(nrow) ) ;
      if( nrow != nRow ) { tmp += ", " ; }
    }
  ///
  return tmp+=" } ";
};

// ============================================================================
/** print of Diagonal Matrix object
 *  @param value   diagonal  matrix to be printed
 *  @return string representation of HepDiagMatrix object
 */
// ============================================================================
std::string CaloPrint::print ( const HepDiagMatrix&    value  ) const 
{
  ///
  const unsigned int nRow = value.num_row() ;
  std::string tmp( " [" + print( nRow ) + "x" + print( nRow ) + "]{ " );
  /// check for 2 special cases 
  if      ( HepDiagMatrix( nRow , 1 ) == value ) 
    { return tmp += "unit matrix } "; }
  else if ( HepDiagMatrix( nRow , 0 ) == value ) 
    { return tmp += "null matrix } "; }  
  ///
  for(  unsigned int nrow = 1 ; nrow <= nRow ; ++nrow )
    {
      tmp += 
        "[" + print( nrow ) + 
        "," + print( nrow ) + "]:" +
        print( value.fast( nrow , nrow ) ) ;
      if( nrow != nRow ) { tmp += ", " ; }
    }
  ///
  return tmp+=" } ";
};

// ============================================================================
/** print of Symmetric Matrix object
 *  @param value   symmetric  matrix to be printed
 *  @return string representation of HepSymMatrix object
 */
// ============================================================================
std::string CaloPrint::print ( const HepSymMatrix&    value  ) const 
{
  ///
  const unsigned int nRow = value.num_row() ;
  std::string tmp( " [" + print( nRow ) + "x" + print( nRow ) + "]{ " );
  /// check for 2 special cases 
  if      ( HepSymMatrix( nRow , 1 ) == value ) 
    { return tmp += "unit matrix } "; }
  else if ( HepSymMatrix( nRow , 0 ) == value ) 
    { return tmp += "null matrix } "; }  
  ///
  for (  unsigned int nrow = 1 ; nrow <= nRow ; ++nrow )
    for( unsigned int ncol = 1 ; ncol <= nrow ; ++ncol )
      {
        tmp += 
          "[" + print( nrow ) + "," + print( ncol ) + "]:" +
          print( value.fast( nrow , ncol ) ) ;
        if( nrow != nRow || ncol != nRow ) { tmp += "," ; }
      }
  ///
  return tmp+=" } ";
};

// ============================================================================
/** print of General Matrix object
 *  @param value   general  matrix to be printed
 *  @return string representation of HepGenMatrix object
 */
// ============================================================================
std::string CaloPrint::print ( const HepGenMatrix&    value  ) const 
{
  ///
  const unsigned int nRow = value.num_row() ;
  const unsigned int nCol = value.num_col() ;
  std::string tmp( " [" + print( nRow ) + "x" + print( nCol ) + "]{ " );
  /// check 2 special cases 
  if          ( HepMatrix( nRow , nCol , 1 ) == value ) 
    { 
      if( nCol != nRow ) 
        { return tmp += "'pseudo-unit' matrix } " ; }
      else 
        { return tmp +=         "unit  matrix } " ; }
    }
  else if ( HepMatrix( nRow , nCol , 0 ) == value )
    {     return tmp +=         "null  matrix } " ; }
  ///
  for (  unsigned int nrow = 1 ; nrow <= nRow ; ++nrow )
    for( unsigned int ncol = 1 ; ncol <= nCol ; ++ncol )
      {
        tmp += 
          "[" + print( nrow ) + "," + print( ncol ) + "]:"
          + print( value( nrow , nrow ) ) ;
        if( nrow != nRow || ncol != nCol ) { tmp += "," ; }
      }
  ///
  return tmp+=" } ";
};

// ============================================================================
// The End 
// ============================================================================

