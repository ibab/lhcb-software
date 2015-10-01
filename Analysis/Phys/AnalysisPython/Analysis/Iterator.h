// $Id$ 
// ============================================================================
#ifndef ANALYSIS_ITERATOR_H 
#define ANALYSIS_ITERATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Forward deslaration 
// ============================================================================
class TIterator        ;  // ROOT 
class RooAbsCollection ;  // RooFit
// ============================================================================
namespace Analysis
{
  // ==========================================================================
  /** @class Iterator 
   *  helper class to simplify manipualitons with ROOT-iterator
   *  @see TIterator 
   *  @author Vanay BELYAEV  Ivan.Belyaev@=itep.ru
   *  @date 2015-02-015
   */
  class GAUDI_API Iterator 
  {
  public: 
    // =========================================================================
    /// standard constructor: create and keep the ietrator 
    Iterator  ( const RooAbsCollection& collection ) ;    
    /// defautl constructor
    Iterator  () ;    
    /// destructor 
    ~Iterator ();
    // =========================================================================    
  public:
    // ========================================================================
    /// invoke TIterator::Next
    TObject* next  () const ;                      // invoke TIterator::Next
    /// invoke TIterator::Reset 
    bool     reset () const ;                      // invoke TIterator::Reset 
    // =========================================================================    
  public: // aliases
    // ========================================================================
    // alias:
    TObject* Next  () const { return next  () ; }  // invoke TIterator::Next    
    // alias:
    bool     Reset () const { return reset () ; }  // invoke TIterator::Next    
    // =========================================================================    
  public: // some pointer alchemistry 
    // ========================================================================
    /// valid iterator 
    bool valid ()                     const { return 0 != m_iterator  ; }
    /// invalid ?
    bool operator!                 () const { return !valid()         ; }
    /// conversion to underlying type 
    inline TIterator* operator->   () const { return m_iterator       ; }
    // =========================================================================    
  private:
    // ========================================================================
    /// copy constructor is disabled 
    Iterator           ( const Iterator& ) ; // copy constructor is disabled 
    /// assignement is disabled 
    Iterator& operator=( const Iterator& ) ; // assignement is disabled
    // =========================================================================    
  private:
    // ========================================================================
    /// iterator itself 
    TIterator* m_iterator ; // iterator itself 
    // ========================================================================    
  };
  // ==========================================================================    
} // end of namespace Analysis
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_ITERATOR_H
// ============================================================================
