// $Id: DecayDescriptor.cpp,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayDescriptor.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from the file LoKi/DecayDescriptor.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */
// ============================================================================
/*  simple function to extract "decay-only" flag from the arrow 
 *  @param   arrow the arrow
 *  @return  "decay-only" flag 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
bool Decays::Trees::decayOnly ( const Decays::Trees::Arrow& arrow ) 
{
  switch ( arrow ) 
  {
  case Decays::Trees::SingleX     : return false ;  // X 
  case Decays::Trees::LongSingleX : return false ;  // X 
  case Decays::Trees::DoubleX     : return false ;  // X 
  case Decays::Trees::LongDoubleX : return false ;  // X 
  default                         : return true  ;
  }
  return true ;
}
// ============================================================================
/*  simple function to extract "algorithm" flag from the arrow 
 *  @param   arrow the arrow
 *  @return  "algorithm" flag 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
Decays::Trees::Alg  Decays::Trees::algorithm 
( const Decays::Trees::Arrow& arrow )
{
  switch ( arrow ) 
  {
  case Decays::Trees::LongSingle      : return Decays::Trees::Sections  ;
  case Decays::Trees::LongDouble      : return Decays::Trees::Sections  ;
  case Decays::Trees::LongSingleX     : return Decays::Trees::Sections  ;
  case Decays::Trees::LongDoubleX     : return Decays::Trees::Sections  ;
  default                             : return Decays::Trees::Daughters ;
  }
  return Decays::Trees::Daughters ;
}
// ============================================================================
/*  simple function to extract "photos" flag from the arrow 
 *  @param   arrow the arrow
 *  @return  "photos" flag 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
bool Decays::Trees::photos    ( const Decays::Trees::Arrow& arrow ) 
{
  switch ( arrow ) 
  {
  case Decays::Trees::Double          : return true  ;
  case Decays::Trees::LongDouble      : return true  ;
  case Decays::Trees::DoubleX         : return true  ;
  case Decays::Trees::LongDoubleX     : return true  ;
  default                             : return false ;
  }
  return false ;
}
// ============================================================================
/*  get the string representation of the arrow 
 *  @param    arr the arrow
 *  @return  strnig representation of the arrow
 *  @see Decays::Trees:Arrow 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
std::string Decays::Trees::arrow ( const Decays::Trees::Arrow& arr ) 
{
  switch ( arr ) 
  {
  case Decays::Trees::Single              : return   "->"  ;
  case Decays::Trees::LongSingle          : return  "-->"  ;
  case Decays::Trees::Double              : return   "=>"  ;
  case Decays::Trees::LongDouble          : return  "==>"  ;
  case Decays::Trees::SingleX             : return  "-x>"  ;
  case Decays::Trees::LongSingleX         : return "--x>"  ;
  case Decays::Trees::DoubleX             : return  "=x>"  ;
  case Decays::Trees::LongDoubleX         : return "==x>"  ;
  default                                 : return "<unknown arrow>" ;
  }
  return "<unknown arrow>" ;
}
// ============================================================================
/*  valid arrow? 
 *  @param   arrow the arrow
 *  @return  true for valid arrow 
 *  @see Decays::Trees:Arrow 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
bool Decays::Trees::valid ( const Decays::Trees::Arrow& arrow ) 
{
  switch ( arrow ) 
  {
  case Decays::Trees::Single              : return true  ;
  case Decays::Trees::LongSingle          : return true  ;
  case Decays::Trees::Double              : return true  ;
  case Decays::Trees::LongDouble          : return true  ;
  case Decays::Trees::SingleX             : return true  ;
  case Decays::Trees::LongSingleX         : return true  ;
  case Decays::Trees::DoubleX             : return true  ;
  case Decays::Trees::LongDoubleX         : return true  ;
  default                                 : return false ;
  }
  return false ;
}
// ============================================================================
/*  valid arrow? 
 *  @param   arrow the arrow
 *  @return  true for valid arrow 
 *  @see Decays::Trees:Arrow 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06 
 */     
// ============================================================================
bool Decays::Trees::valid ( const std::string& arrow ) 
{
  return 
    "->"   == arrow || 
    "-->"  == arrow || 
    "=>"   == arrow || 
    "==>"  == arrow || 
    "-x>"  == arrow || 
    "--x>" == arrow || 
    "=x>"  == arrow || 
    "==x>" == arrow ;  
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
