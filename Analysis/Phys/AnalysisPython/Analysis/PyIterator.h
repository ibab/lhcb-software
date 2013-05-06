// $If:$ 
// ============================================================================
#ifndef ANALYSIS_PYITERATOR_H 
#define ANALYSIS_PYITERATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <limits>
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/integer_traits.hpp"
// ============================================================================
// forward declarations 
// ============================================================================
class TTree ; // from ROOT 
// ============================================================================
namespace Analysis
{
  // ==========================================================================
  class Formula ;
  // ==========================================================================
  /** @class PyIterator Analysis/PyIterator.h
   *  
   *  Helper class for fast iteratior over TTree in python.
   *  Iteration in python for large tree canbe very time consuming, 
   *  for such cases this "iterator-with-cuts" is much faster
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2013-05-06
   */
  class GAUDI_API PyIterator 
  {
  public:
    // ========================================================================
    /// constructor 
    PyIterator 
    ( TTree*              tree      , 
      const std::string&  cuts      , 
      const unsigned long first = 0 , 
      const unsigned long last  = boost::integer_traits<unsigned long>::const_max );
    /// destructor 
    virtual ~PyIterator() ;  // destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// go to next item 
    TTree* next () const ;                    // go to next item 
    /// get the tree 
    TTree* tree () const { return m_tree ; }  // get the tree 
    /// check if formula is ok 
    bool   ok   () const ;
    // ========================================================================
  private:
    // ========================================================================
    PyIterator () ;
    PyIterator ( const PyIterator& ) ;
    PyIterator& operator=( const PyIterator& ) ;    
    // ========================================================================
  private:
    // ========================================================================
    TTree*                            m_tree    ;
    Analysis::Formula*                m_formula ;
    mutable unsigned long             m_current ;
    unsigned long                     m_last    ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                  end of namespace Analysis 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_PYITERATOR_H
// ============================================================================
