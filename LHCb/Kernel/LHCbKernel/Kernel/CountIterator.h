// $Id:$ 
// ============================================================================
#ifndef KERNEL_COUNTITERATOR_H 
#define KERNEL_COUNTITERATOR_H 1
// ============================================================================
// STD & STL 
// ============================================================================
#include <iterator>
// ============================================================================
/** @class CountIterator CountIterator.h Kernel/CountIterator.h
 *  Trivial "count"-iterator: output iterator that only counts 
 *
 *  @author Vanya Belyaev
 *  @date   2015-03-17
 */
template<typename TYPE>
class count_iterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
  // ========================================================================
  /// Constructor 
  count_iterator () : m_count ( 0 ) {}
  //
  count_iterator& operator = ( const TYPE& /* value */ )
  {
    ++m_count ;
    return *this ;
  }
  //
  count_iterator& operator*  ()    { return *this; }
  count_iterator& operator++ ()    { return *this; }
  count_iterator& operator++ (int) { return *this; }
  // 
  // ========================================================================
public:
  // ==========================================================================
  /// the the counter 
  unsigned long count() const { return m_count ; }
  // ==========================================================================  
private:
  ///
  unsigned long m_count ;
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_COUNTITERATOR_H
// ============================================================================
