#ifndef DetDesc_StaticArray_h
#define DetDesc_StaticArray_h

/** Includes */
#include "GaudiKernel/GaudiException.h"
#include <algorithm> // For std::swap_ranges

/** @class StaticArray StaticArray.h "DetDesc/StaticArray.h"
 *
 *  Simple implementation of an array with static allocation. It has
 *  a bit the look and feel of a std::vector, but with fixed capacity. Note
 *  that it does not call destructors or placement new, so it probably
 *  only works for basic types. The interface is also not entirely
 *  complete (e.g. no assignment, insert, or resize), but that can
 *  easily be changed when needed.
 *  
 *  Note that there is also a more official implementation of this
 *  idea, namely the "alps/fixed_capacity_vector.h". I hope that that
 *  will make it to boost one day.
 *
 *  @author Wouter Hulsbergen
 *  @date 20/09/2007
 */

template<typename T, std::size_t N> 
class StaticArray
{
public:
  ///@{
  /** types */
  typedef T                                     value_type;            
  typedef T*                                    iterator;              
  typedef const T*                              const_iterator;        
  typedef std::reverse_iterator<iterator>       reverse_iterator;      
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef T&                                    reference;             
  typedef const T&                              const_reference;       
  typedef std::size_t                           size_type;             
  typedef std::ptrdiff_t                        difference_type;       
  ///@}

  /** constructor */
  StaticArray() : m_size(0) {}

  //template<typename U> StaticArray& operator=(const StaticArray<U, N>&) ;

  ///@{
  /** iterator support */
  iterator begin()             { return m_data ; }
  const_iterator begin() const { return m_data ; }
  iterator end()               { return m_data + m_size ; }
  const_iterator end() const   { return m_data + m_size ; }
  ///@}

  ///@{
  /** reverse iterator support */
  reverse_iterator rbegin()             { return reverse_iterator(end()) ; }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()) ; }
  reverse_iterator rend()               { return reverse_iterator(begin()) ; }
  const_reverse_iterator rend() const   { return const_reverse_iterator(begin()) ; }
  ///@}

  ///@{
  /** size and capacity */
  size_type size() const { return m_size ; }
  bool empty()     const { return m_size==0 ; }
  size_type max_size() const { return N ; }
  size_type capacity() const { return N ; }
  void reserve(size_type n) { testcapacity(n) ; }
  ///@}

  ///@{
  /** element access */
  reference operator[](size_type i)             { return m_data[i] ; }
  const_reference operator[](size_type i) const { return m_data[i] ; }
  reference at(size_type i)                     { return m_data[i] ; }
  const_reference at(size_type i) const         { return m_data[i] ; }
  reference front()                             { return m_data[0] ; }
  const_reference front() const                 { return m_data[0] ; }
  reference back()                              { return m_data[m_size-1] ; }
  const_reference back() const                  { return m_data[m_size-1] ; }
  ///@}

  /** add a single element */
  void push_back(const_reference x) { testcapacity(m_size+1) ; at(m_size++) = x ; }

  /** clear all entries. note: no desctrutors called. */
  void clear() { m_size = 0 ; }

  /** erase a range */
  void erase(iterator first, iterator last) {
    if( last>first ) {
      if(last < end() ) {
	int n = end() - last ;
	for(int i=0; i<n; ++i) *(first + i) = *(last+i) ;
      }
      m_size -= last - first ;
    }
  }

  /** erase a single element */
  void erase(iterator pos) { return erase(pos,pos+1) ; }

  /** insert a single element */
  void insert(iterator pos, const_reference x) {
    testcapacity(m_size+1) ;
    for(iterator next = pos; next<end(); ++next) *(next+1) = *(next) ;
    ++m_size ;
    *pos = x ;
  }

  /** swap the contents of this array with that of another one */
  void swap( StaticArray<T, N>& rhs) { 
    std::swap_ranges(m_data,m_data+std::max(m_size,rhs.m_size),rhs.m_data) ; 
    std::swap( m_size, rhs.m_size ) ;
  }
  
private:
  /** throw an exception if the capacity is not large enough to hold 'newsize' elements */
  void testcapacity( size_type newsize ) const {
    if( newsize > N ) 
      throw GaudiException("DetDesc/StaticArray: trying to add data beyond maximum capacity of this container.\n Please increase the max_size template parameter.","DetDesc/StaticArray overflow", StatusCode::FAILURE) ;
  }
  
private:
  T m_data[N];       /// elements
  size_type m_size ; /// size
};

#endif
