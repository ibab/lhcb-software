// $Id: OutputStreamIterator.h,v 1.1.1.1 2001-11-25 14:07:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Nane:$ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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


template <class TYPE, class OSTREAM >
class OutputStreamIterator 
{
  ///
 protected:
  ///  
  OSTREAM*    stream ;
  const char* string ;
  ///
 public:
  ///
  typedef std::output_iterator_tag            iterator_category ;
  typedef void                                value_type        ;
  typedef void                                difference_type   ;
  typedef void                                pointer           ;
  typedef void                                reference         ;
  ///
  typedef OutputStreamIterator<TYPE,OSTREAM>  MyType            ;
  ///
  OutputStreamIterator( OSTREAM & s                 ) 
    : stream(&s), string(0) {} ;  
  OutputStreamIterator( OSTREAM & s , const char* c ) 
    : stream(&s), string(c) {} ;
  ///
  MyType& operator=(const TYPE& value) 
  { 
    *stream << value;
    if (string) *stream << string;
    return *this; 
  }
  ///
  MyType & operator*()     { return *this; }
  MyType & operator++()    { return *this; }   
 /// NB !!! It returns the reference also!!1
  MyType & operator++(int) { return *this; }  
  ///
};

template <class TYPE , class OSTREAM >
inline std::output_iterator_tag 
iterator_category( const OutputStreamIterator<TYPE,OSTREAM> & ) 
{ return std::output_iterator_tag(); }

// ============================================================================
// The End 
// ============================================================================
#endif ///<  CALOKERNEL_OUTPUTSTREAMITERATOR_H 
// ============================================================================

