// $Id$
// ============================================================================
#ifndef LOKI_DECAYCHAIN_H 
#define LOKI_DECAYCHAIN_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/DecayChainBase.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DecayChain
   *  Utility to print the decays
   *
   *  @author Vanya Belyaev
   *  @date   2011-06-03
   */
  class GAUDI_API DecayChain : public virtual DecayChainBase 
  {
  protected:
    // ========================================================================
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant Accept ;
    // ========================================================================
  public: 
    // ========================================================================
    /** Standard constructor
     *  @param maxDepth  maximal decay depth 
     *  @param vertex    vertex info to be printed 
     *  @param mode      mode for printout of 4-vectors 
     *  @param fg        color for foreground for 'marked' lines 
     *  @param bg        color for background for 'marked' lines 
     *  @param vertexe   end-vertex info for MC particles to be printed 
     */
    DecayChain
    ( const size_t          maxDepth = 5            , 
      const bool            vertex   = true         , 
      const Mode            mode     = LV_WITHPT    ,
      const MSG::Color&     fg       = MSG::YELLOW  ,
      const MSG::Color&     bg       = MSG::RED     ) ;
    /// Standard constructor
    DecayChain ( const LoKi::DecayChainBase& base ) ;
    /// virtual destructor 
    virtual ~DecayChain () ;                         // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================    
    /** print decay chain for sequence of objects 
     *  @param first    begin of sequence of objects 
     *  @param last     end   of sequence of objects 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class PARTICLE    ,
               class STREAM      ,
               class TERMINATOR  ,
               class ACCEPT      ,
               class MARK        > 
    STREAM& print ( PARTICLE           first   ,
                    PARTICLE           last    ,
                    STREAM&            stream  , 
                    TERMINATOR         term    ,
                    const ACCEPT&      accept  ,
                    const MARK&        mark    ,
                    const std::string& prefix  ,
                    const size_t       depth   ) const ;    
    // ========================================================================
  public:
    // ========================================================================
    /** print the decay chain for Particle
     * 
     *  @code 
     *
     *    DecayChain dc ;
     *    const LHCb::Particle* head = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( head , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gausdi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( head , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     , 
               class MARK       > 
    STREAM& print ( const LHCb::Particle* particle       , 
                    STREAM&               stream         , 
                    TERMINATOR            term           ,
                    const ACCEPT&         accept         ,
                    const MARK&           mark           ,
                    const std::string&    prefix  = " "  ,
                    const size_t          depth   = 0    ) const 
    {
      // invalid particle 
      if ( 0 == particle ) 
      {
        Error ( " LHCb::Particle* points to NULL" ) ;
        return stream ; 
      }
      //
      stream << term ;
      //
      const bool marked = mark ( particle ) ;
      //
      size_t _indent = 0 ;
      {
        // use colors :-))
        MarkStream<STREAM> ms ( stream , marked , fg() , bg() ) ;
        //
        const std::string   p_name  = name ( particle->particleID () ) ;
        const unsigned long l_depth = std::max    ( 3 * ( std::min ( maxDepth() , size_t(20) ) - depth ) , size_t(0) ) ;
        const std::string   l_blank = blank ( 1 + l_depth ) ;
        //
        stream << ( marked ? '*' : ' ' )
               << depth 
               << prefix 
               << "|->" 
               << p_name  
               << l_blank  ;
        //
        _indent += 2 ;
        _indent += prefix  . length () ;
        _indent += 2 ;
        _indent += p_name  . length () ;
        _indent += l_blank . length () ; 
        //
        if ( depth >   9 ) { _indent += 1 ; }
        if ( depth >  99 ) { _indent += 1 ; }
        if ( depth > 999 ) { _indent += 1 ; }
        //
      }
      //
      if ( !marked && maxDepth() < depth   ) 
      { stream << " ... (max depth)     " ; return stream ; }
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " ; return stream ; }
      //
      stream << toString( particle->momentum() ) ;
      if ( particle -> hasKey() )  
      { stream << " #" << toString ( particle->key() ) ; }
      ///
      const LHCb::Vertex* vertex = particle->endVertex ();
      if ( m_vertex && 0 != vertex ) 
      { 
        // use the terminator 
        stream << term     ;  
        stream << blank ( _indent + 5 ) ;
        stream << " EndVertex " << toString ( vertex->position() ) ; 
        stream << " Chi2/nDoF " 
               << toString ( vertex->chi2() )
               << "/" << vertex->nDoF() ;
        if ( vertex -> hasKey() )  
        { stream << " #" << toString ( vertex -> key() ) ; }
      }
      //
      typedef SmartRefVector<LHCb::Particle> Products     ;
      const Products& products = particle->daughters() ;
      print ( products.begin() , products.end() , 
              stream           , term           , 
              accept           , mark           ,              
              prefix + "   "   , depth + 1      ) ;     // RECURSION 
      //
      return stream ;
    }
    // ========================================================================
  public:
    // ========================================================================    
    /// predefined printout    
    std::string print_ 
    ( const LHCb::Particle*       p                         , 
      const LoKi::Types::Cuts&    accept = Accept ( true  ) , 
      const LoKi::Types::Cuts&    mark   = Accept ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::Particle::Container* p                    , 
      const LoKi::Types::Cuts&    accept = Accept ( true  ) , 
      const LoKi::Types::Cuts&    mark   = Accept ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::Particle::ConstVector& p                  , 
      const LoKi::Types::Cuts&    accept = Accept ( true  ) , 
      const LoKi::Types::Cuts&    mark   = Accept ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::Particle::Range& p                        , 
      const LoKi::Types::Cuts&    accept = Accept ( true  ) , 
      const LoKi::Types::Cuts&    mark   = Accept ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::Particle::Selection* p                    , 
      const LoKi::Types::Cuts&    accept = Accept ( true  ) , 
      const LoKi::Types::Cuts&    mark   = Accept ( false ) ) const ;
    // ========================================================================    
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
/*  print decay chain for sequence of objects 
 *  @param first    begin of sequence of objects 
 *  @param last     end   of sequence of objects 
 *  @param stream   reference to output stream 
 *  @param term     stream terminator 
 */
// ============================================================================
template < class PARTICLE    ,
           class STREAM      ,
           class TERMINATOR  ,
           class ACCEPT      ,
           class MARK        > 
STREAM& LoKi::DecayChain::print
( PARTICLE           first   ,
  PARTICLE           last    ,
  STREAM&            stream  , 
  TERMINATOR         term    ,
  const ACCEPT&      accept  ,
  const MARK&        mark    ,
  const std::string& prefix  ,
  const size_t       depth   ) const    
{
  for ( ; first != last ; ++first ) 
  { print ( *first , stream , term , accept , mark , prefix , depth ) ; }
  return stream ;
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_DECAYCHAIN_H
// ============================================================================
