// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rich/RichKernel/RichKernel/OutputStreamIterator.h,v 1.1.1.1 2002-04-04 14:33:51 jonrob Exp $ 
#ifndef    DDEXAMPLE_OUTPUTSTREAMITERATOR_H 
#define    DDEXAMPLE_OUTPUTSTREAMITERATOR_H  1 
// STL 
#include <iterator> 


/** @class OutputStreamIterator OutputStreamIterator.h CaloGen/OutputStreamIterato.h
    
    a simple helpful class for maintain the notion  
    of output iterator to arbitrary stream. 
    
    @author  Vanya Belyaev 
    @date    26/11/1999
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
  OutputStreamIterator( OSTREAM & s                 ) : stream(&s), string(0) {} ;  
  OutputStreamIterator( OSTREAM & s , const char* c ) : stream(&s), string(c) {} ;
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
  MyType & operator++(int) { return *this; }   /// NB !!! It returns the reference also!!1
  ///
};
///
template <class TYPE , class OSTREAM >
inline std::output_iterator_tag iterator_category( const OutputStreamIterator<TYPE,OSTREAM> & ) 
{ return std::output_iterator_tag(); }
///

#endif //  DDEXAMPLE_OUTPUTSTREAMITERATOR_H 






