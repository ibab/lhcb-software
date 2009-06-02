// $Id: DecayFinder.h,v 1.1 2009-06-02 16:47:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DECAYFINDER_H 
#define LOKI_DECAYFINDER_H 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
// ============================================================================
namespace Decays
{
  // ==========================================================================
  /** @class Finder LoKi/DecayFinder.h
   *  
   *  Helper class to find/select the decays
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-06-02
   */
  template <class PARTICLE>
  class Finder_ 
  {
  protected:
    // ========================================================================
    /// the type for collection 
    typedef typename Decays::iTree_<PARTICLE>::Collection          Collection ;    
    typedef typename Collection::const_iterator                    iterator   ;
    // ========================================================================
  public: 
    // ========================================================================
    /// constructor from the tree 
    Finder_ ( const iTree_<PARTICLE>& tree ) : m_tree ( tree ) {}
    // ========================================================================
  public:
    // ======================================================================== 
    /** check the presence of the decay in the input sequence
     *
     *  @code
     *   
     *   SEQUENCE input = ... ;
     * 
     *   const iTree_<PARTICLE>& tree = ... ;
     * 
     *   // create the selector/fidner:
     *   Finder_<PARTICLE> finder (  tree ) ;
     *
     *   // use the selector 
     *   const bool ok = finder.hasDecay( input.begin( ), input.end() ) ;
     *
     *  @endcode
     *
     *  @param first (INPUT) begin-iterator for the input sequence 
     *  @param last  (INPUT) end-iterator   for the input sequence 
     *  @return true if the specified decay is in the sequnce 
     */
    template <class ITERATOR>
    bool hasDecay 
    ( ITERATOR first , 
      ITERATOR last  ) const 
    { 
      return last != std::find_if ( first , last , m_tree ) ; 
    }
    // ======================================================================== 
    /** find the specified decay in the input sequence
     *
     *  @code
     *   
     *   SEQUENCE input = ... ;
     * 
     *   const iTree_<PARTICLE>& tree = ... ;
     * 
     *   // create the selectot:
     *   Finder_<PARTICLE> finder (  tree ) ;
     *
     *   std::vector<PARTICLE>  output ;
     *
     *   // use the selector 
     *   finder.findDecay  ( input.begin( ), input.end() , output ) ;
     *
     *  @endcode
     *
     *  @param first  (INPUT)  begin-iterator for the input sequence 
     *  @param last   (INPUT)  end-iterator   for the input sequence 
     *  @param output (OUTPUT) vector of selected particles 
     *  @return number of found particles 
     */
    template <class ITERATOR, class OUTPUT> 
    size_t findDecay 
    ( ITERATOR             first  , 
      ITERATOR             last   , 
      std::vector<OUTPUT>& output ) const 
    {
      const size_t old_size = output.size() ;
      (*this)( first , last , std::back_inserter( output ) ) ;
      return output.size() - old_size ;
    }
    // ========================================================================
  public:
    // ======================================================================== 
    /** the major method for selection of particles from some sequence
     *  
     *  @code
     *   
     *   SEQUENCE input = ... ;
     * 
     *   const iTree_<PARTICLE>& tree = ... ;
     * 
     *   // create the selector/finder:
     *   Finder_<PARTICLE> finder (  tree ) ;
     *
     *   std::vector<PARTICLE>  output ;
     *
     *   // use the selector/finder: 
     *   finder  ( input.begin( ), input.end() , 
     *             std::back_inserter ( output ) ) ;
     *
     *  @endcode
     *
     *  @param  first  (INPUT)  the begin  operator of the sequence 
     *  @param  last   (INPUT)  the end    operator of the sequence 
     *  @param  output (OUTPUT) the output iterator for the result
     *  @return advanced output operator 
     */
    template <class ITERATOR,class OUTPUT > 
    OUTPUT operator()  ( ITERATOR first  , 
                         ITERATOR last   , 
                         OUTPUT   output ) const 
    {
      const bool valid  = m_tree.valid  () ;
      if ( !valid ) { return output ; }   // No action for Invalid tree, RETURN
      const bool marked = m_tree.marked () ;
      Collection _mark ;      
      for ( ; first != last ; ++first ) 
      {
        if ( marked ) { m_tree.reset() ; }
        // OK ? 
        if ( !m_tree.eval ( *first ) ) { continue ; }               // CONTINUE
        // the regular non-marked  descriptor
        if ( !marked ) 
        { 
          *output = *first ; // assign 
          ++output         ; // advance 
          continue         ;                                        // CONTINUE 
        }
        // the descriptor with marked elements
        _mark.clear() ;
        m_tree.collect ( _mark ) ;
        for ( iterator imark = _mark.begin() ; _mark.end() != imark ; ++imark ) 
        {
          *output = *imark ;  // assign 
          ++output         ;  // advance 
        }
        _mark.clear() ;
      }
      return output ;                                                 // RETURN 
    }
    // ========================================================================
    /** the major method for selection of particles from some sequence
     *  
     *  @code
     *   
     *   std:vector<PARTICLE> input = ... ;
     * 
     *   const iTree_<PARTICLE>& tree = ... ;
     * 
     *   // create the selector/finder:
     *   Finder_<PARTICLE> finder (  tree ) ;
     *
     *   std::vector<PARTICLE>  output ;
     *
     *   // use the selector 
     *   finder  ( input , output ) ;
     *
     *  @endcode
     *
     *  @param  first  (INPUT)  the input  container 
     *  @param  output (OUTPUT) the output container 
     *  @return number of added elements 
     */
    template <class INPUT,class OUTPUT> 
    size_t operator()  ( const INPUT&         input  , 
                         std::vector<OUTPUT>& output ) const
    {
      const size_t old_size = output.size() ;
      // regirect to another method 
      (*this) ( input.begin() , input.end() , std::back_inserter ( output ) ) ;
      return output.size() - old_size ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// cast operator to the underlyiong tree 
    operator const iTree_<PARTICLE>& () const { return m_tree.tree() ; }// cast
    /// invalid tree? 
    bool operator! () const { return !m_tree.valid() ; }       // invalid tree? 
    // ========================================================================    
  private:
    // ========================================================================
    // no default constructor 
    Finder_ () ;                                      // no default constructor
    // ========================================================================
  private:
    // ========================================================================
    /// the actual selection tree 
    Tree_<PARTICLE> m_tree ;                       // the actual selection tree 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYFINDER_H
// ============================================================================
