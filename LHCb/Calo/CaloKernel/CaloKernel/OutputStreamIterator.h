// $Id: OutputStreamIterator.h,v 1.2 2002-03-18 18:16:21 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Nane:$ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.4  2001/06/23 14:39:32  ibelyaev
//  fix CVS-keywords and remove long lines
// ============================================================================
#ifndef    CALOKERNEL_OUTPUTSTREAMITERATOR_H 
#define    CALOKERNEL_OUTPUTSTREAMITERATOR_H  1 
// ============================================================================
// STL 
#include <iterator>

/** @class OutputStreamIterator OutputStreamIterator.h 
 *                   CaloKernel/OutputStreamIterator.h
 *  
 *  a simple helpful class for maintain the notion  
 *  of output iterator to arbitrary stream. 
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru  
 *  @date    26/11/1999
 */

template <class TYPE, class OSTREAM, class TERMINATOR = const char* >
class OutputStreamIterator 
{
  ///
protected:
  ///  
  OSTREAM*    stream          ;
  TERMINATOR  terminator      ;
  bool        has_terminator  ;
  ///
 public:
  ///
  typedef std::output_iterator_tag            iterator_category ;
  typedef void                                value_type        ;
  typedef void                                difference_type   ;
  typedef void                                pointer           ;
  typedef void                                reference         ;
  ///
  typedef OutputStreamIterator<TYPE,OSTREAM,TERMINATOR>  MyType ;
  ///
  OutputStreamIterator( OSTREAM & s                 ) 
    : stream         ( &s    )  
    , terminator     (       ) 
    , has_terminator ( false )  {} ;  
  OutputStreamIterator( OSTREAM & s , TERMINATOR c ) 
    : stream         ( &s    )
    , terminator     ( c     ) 
    , has_terminator ( true  )  {} ;
  ///
  MyType& operator=(const TYPE& value) 
  { 
    *stream << value;
    if (has_terminator) *stream << terminator ;
    return *this; 
  }
  ///
  MyType & operator*()     { return *this; }
  MyType & operator++()    { return *this; }   
 /// NB !!! It returns the reference also!!1
  MyType & operator++(int) { return *this; }  
  ///
};

template <class TYPE , class OSTREAM, class TERMINATOR >
inline std::output_iterator_tag 
iterator_category( const OutputStreamIterator<TYPE,OSTREAM,TERMINATOR> & ) 
{ return std::output_iterator_tag(); }

template <class TYPE , class OSTREAM, class TERMINATOR >
inline OutputStreamIterator<TYPE,OSTREAM,TERMINATOR>
output_stream_iterator( OSTREAM& s , TERMINATOR c ) 
{ return OutputStreamIterator<TYPE,OSTREAM,TERMINATOR>( s , c ); }

template <class TYPE , class OSTREAM>
inline OutputStreamIterator<TYPE,OSTREAM>
output_stream_iterator( OSTREAM& s) 
{ return OutputStreamIterator<TYPE,OSTREAM>( s ); }

// ============================================================================
// The End 
// ============================================================================
#endif ///<  CALOKERNEL_OUTPUTSTREAMITERATOR_H 
// ============================================================================

