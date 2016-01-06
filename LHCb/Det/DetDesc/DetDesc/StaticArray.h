#ifndef DetDesc_StaticArray_h
#define DetDesc_StaticArray_h

/** Includes */
#include "GaudiKernel/GaudiException.h"
#include <algorithm> // For std::swap_ranges
#include <array>

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
class StaticArray final
{
public:
  ///@{
  /** types */
  typedef typename std::array<T, N>::value_type          value_type;
  typedef typename std::array<T, N>::iterator            iterator;
  typedef typename std::array<T, N>::const_iterator      const_iterator;
  typedef std::reverse_iterator<iterator>                reverse_iterator;
  typedef std::reverse_iterator<const_iterator>          const_reverse_iterator;
  typedef typename std::array<T, N>::reference           reference;
  typedef typename std::array<T, N>::const_reference     const_reference;
  typedef typename std::array<T, N>::size_type           size_type;
  typedef typename std::array<T, N>::difference_type     difference_type;
  ///@}

  ///@{
  /** enums */
  enum { MaxSize=N } ;
  ///@}

  /** constructor */
  StaticArray() = default;

  ///@{
  /** iterator support */
  iterator begin() noexcept             { return m_data.begin(); }
  const_iterator begin() const noexcept { return m_data.begin() ; }
  iterator end() noexcept               { return m_data.begin() + m_size ; }
  const_iterator end() const noexcept   { return m_data.begin() + m_size ; }
  ///@}

  ///@{
  /** reverse iterator support */
  reverse_iterator rbegin() noexcept             { return reverse_iterator(end()) ; }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()) ; }
  reverse_iterator rend() noexcept               { return reverse_iterator(begin()) ; }
  const_reverse_iterator rend() const noexcept   { return const_reverse_iterator(begin()) ; }
  ///@}

  ///@{
  /** size and capacity */
  size_type size() const noexcept { return m_size ; }
  bool empty()     const noexcept { return 0 == m_size; }
  constexpr size_type max_size() const noexcept { return N ; }
  constexpr size_type capacity() const noexcept { return N ; }
  void reserve(size_type n) { testcapacity(n) ; }
  ///@}

  ///@{
  /** element access */
  reference operator[](size_type i) noexcept             { return m_data[i] ; }
  const_reference operator[](size_type i) const noexcept { return m_data[i] ; }
  reference at(size_type i) noexcept                     { return m_data[i] ; }
  const_reference at(size_type i) const noexcept         { return m_data[i] ; }
  reference front() noexcept                             { return m_data[0] ; }
  const_reference front() const noexcept                 { return m_data[0] ; }
  reference back() noexcept                              { return m_data[m_size-1] ; }
  const_reference back() const noexcept                  { return m_data[m_size-1] ; }
  ///@}

  /** add a single element */
  void push_back(const_reference x) { testcapacity(m_size+1) ; at(m_size++) = x ; }

  /** clear all entries. note: no desctrutors called. */
  void clear() noexcept { m_size = 0 ; }

  /** erase a range */
  iterator erase(iterator first, iterator last) noexcept {
    if (last > first) {
      if (last < end()) {
	size_type n = static_cast<size_type>(end() - last);
	for(size_type i=0; i<n; ++i) *(first + i) = *(last+i);
      }
      m_size -= last - first;
    }
    return first;
  }

  /** erase a single element */
  iterator erase(iterator pos) noexcept { return erase(pos,pos+1) ; }

  /** insert a single element */
  void insert(iterator pos, const_reference x) {
    testcapacity(m_size+1) ;
    for(iterator next = pos; next<end(); ++next) *(next+1) = *(next) ;
    ++m_size ;
    *pos = x ;
  }

  /** swap the contents of this array with that of another one */
  void swap( StaticArray<T, N>& rhs) noexcept(noexcept(std::swap(rhs.m_size, rhs.m_size)) && noexcept(std::swap(rhs.front(), rhs.front()))) {
    std::swap_ranges(begin(),begin()+std::max(m_size,rhs.m_size),rhs.begin());
    std::swap( m_size, rhs.m_size ) ;
  }

private:
  /** throw an exception if the capacity is not large enough to hold 'newsize' elements */
  void testcapacity( size_type newsize ) const {
    if( newsize > N )
      throw GaudiException("DetDesc/StaticArray: trying to add data beyond maximum capacity of this container.\n Please increase the max_size template parameter.","DetDesc/StaticArray overflow", StatusCode::FAILURE) ;
  }

private:
  std::array<T, N> m_data;       /// elements
  size_type m_size = 0 ; /// size
};

#endif
