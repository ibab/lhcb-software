// $Id: InputStreamIterator.h,v 1.1.1.1 2001-11-25 14:07:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/06/23 14:39:32  ibelyaev
//  fix CVS-keywords and remove long lines
// ============================================================================
#ifndef    CALOKERNEL_INPUTSTREAMITERATOR_H 
#define    CALOKERNEL_INPUTSTREAMITERATOR_H  1 
// STL 
#include <iterator>

/** @class InputStreamIterator InputStreamIterator.h 
 *                  CaloKernel/InputStreamIterator.h
 *  
 *  a simple helpful class for maintain the notion  
 *  of Input iterator to arbitrary stream. 
 *   
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    26/11/1999
 */

template <class TYPE , class ISTREAM ,  class Distance = std::ptrdiff_t> 
class InputStreamIterator 
{
  ///  __STL_NULL_TMPL_ARGS

  public:
  ///
  typedef std::input_iterator_tag iterator_category;
  typedef TYPE                    value_type;
  typedef Distance                difference_type;
  typedef const TYPE*             pointer;
  typedef const TYPE&             reference;
  ///
  typedef InputStreamIterator<TYPE,ISTREAM,Distance> MyType ; 
  ///
  InputStreamIterator( ISTREAM& s) : stream(&s) { read(); } ;
  ///
  reference operator* () const { return value          ; } ;
  /// ??? what is it ??? 
  pointer   operator->() const { return &(operator*()) ; } ;
  ///
  MyType& operator++() 
  { 
    read(); 
    return *this;
  };
  ///
  MyType operator++(int)  
  {
    MyType tmp = *this;
    read();
    return tmp;
  };
  ///
  bool operator== ( const InputStreamIterator<TYPE,ISTREAM,Distance>& y )
  {
    return 
    stream     == y.stream && end_marker   == y.end_marker ||
    end_marker == false    && y.end_marker == false;
  }
  ///  
  protected:
  ///
  ISTREAM* stream      ;
  TYPE     value       ;
  bool     end_marker  ;
  ///
  void read() 
  {
    end_marker =   (*stream) ? true : false;
    if (end_marker) *stream >> value;
    end_marker =   (*stream) ? true : false;
  }
  ///
};

template <class T, class S, class Distance>
inline std::input_iterator_tag 
iterator_category(const InputStreamIterator<T, S, Distance>&) {
  return std::input_iterator_tag();
}

template <class T, class S, class Distance>
inline T* value_type(const InputStreamIterator<T, S, Distance>&) 
{ return (T*) 0; }

template <class T, class S, class Distance>
inline Distance* distance_type(const InputStreamIterator<T,S, Distance>&) {
  return (Distance*) 0;
}

// ============================================================================
// The End 
// ============================================================================
#endif ///<  CALOKERNEL_INPUTSTREAMITERATOR_H 
// ============================================================================








