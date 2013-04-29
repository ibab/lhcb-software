// $Id$
#ifndef LOKI_MCDECAYCHAIN_H 
#define LOKI_MCDECAYCHAIN_H 1
// ============================================================================
// Include files
// ============================================================================
// MCEvent
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/DecayChainBase.h"
#include "LoKi/BuildMCTrees.h"
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
  /** @class MCDecayChain
   *  Utiulity to print the decays
   *
   *  @author Vanya Belyaev
   *  @date   2011-06-03
   */
  class GAUDI_API MCDecayChain : public virtual DecayChainBase 
  {
    // ========================================================================
  protected:
    // ========================================================================
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant AcceptMC ;
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
    MCDecayChain
    ( const size_t          maxDepth = 5            , 
      const bool            vertex   = false        , 
      const Mode            mode     = LV_WITHPT    ,
      const MSG::Color&     fg       = MSG::YELLOW  ,
      const MSG::Color&     bg       = MSG::RED     ,
      const bool            vertexe  = true         ,
      const bool            vertexd  = true         ) ;
    /** Standard constructor
     *  @param vertexe   end-vertex info for MC particles to be printed 
     *  @param vertexs   end-vertex info for MC particles to be printed 
     */
    MCDecayChain
    ( const LoKi::DecayChainBase& base              , 
      const bool                  vertexe  = true   , 
      const bool                  vertexd  = true   ) ;
    /// virtual destructor 
    virtual ~MCDecayChain () ;                         // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// print end-vertex info ?
    bool              vertexe     () const { return m_vertexe  ; }
    void              setVertexE  (  const bool value ) 
    { m_vertexe    = value ; }
    /// print end-vertex info ?
    bool              vertexd     () const { return m_vertexd  ; }
    void              setVertexD  (  const bool value ) 
    { m_vertexd    = value ; }
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
    /** print the decay chain for MCParticle
     * 
     *  @code 
     *
     *    MCDecayChain dc ;
     *    const LHCb::MCParticle* head = ... ; 
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
    STREAM& print ( const LHCb::MCParticle*  particle       , 
                    STREAM&                  stream         , 
                    TERMINATOR               term           ,
                    const ACCEPT&            accept         ,
                    const MARK&              mark           ,
                    const std::string&       prefix  = " "  ,
                    const size_t             depth   = 0    ) const    
    {
      //
      // invalid particle 
      if ( 0 == particle ) 
      { 
        Error ( " LHCb::MCParticle* points to NULL" ) ;
        return stream ;                                  // RETURN 
      }
      //
      stream << term ;
      //
      const bool marked = mark( particle ) ;
      //
      size_t _indent = 0 ;
      {
        // use colors :-))
        MarkStream<STREAM> ms ( stream , marked , fg() , bg() ) ;
        //
        const std::string  p_name  = name ( particle->particleID() ) ;
        const long         l_depth = std::max    ( 3 * ( std::min ( maxDepth() , size_t(20) ) - depth ) , size_t(0) ) ;
        const std::string  l_blank = blank ( 1 + l_depth ) ;
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
      { stream << " ... (max depth)     " ; return stream ; } // RETURN 
      //
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " ; return stream ; } // REUTRN 
      //
      stream << toString( particle->momentum() ) ;
      if ( particle -> hasKey() )  
      { stream << " #" << toString ( particle->key() ) ; }
      //
      const LHCb::MCVertex* vertex = particle->originVertex ();
      if     ( m_vertex && 0 != vertex ) 
      {
        // use the terminator 
        stream << term      ;  
        stream << blank ( _indent + 5 );
        stream << " Origin    " << toString ( vertex -> position() ) ;
        if ( vertex -> hasKey() )  
        { stream << " #" << toString ( vertex -> key() ) ; }
        stream << " "   << vertex->type()  ;
      }
      //
      typedef SmartRefVector<LHCb::MCVertex>   EndVertices ;
      const EndVertices& endVertices = particle->endVertices() ;
      //
      if ( endVertices.empty() ) { return stream ; }
      //
      // print all end-vertices 
      for ( EndVertices::const_iterator ev = endVertices.begin() ; 
            endVertices.end() != ev ; ++ev ) 
      {
        const LHCb::MCVertex* evertex = *ev ;
        if ( 0 == evertex )                     { continue ; } // CONTINUE 
        // keep only "decay"-endvertices  
        if ( m_vertexd && !evertex->isDecay() ) { continue ; } // CONTINUE 
        //
        if ( m_vertex && m_vertexe )  
        {
          // use the terminator 
          stream << term     ;
          stream << blank ( _indent + 5 );
          stream << " EndVtx[" << ( ev - endVertices.begin() ) << "] " ;
          stream << toString ( evertex -> position() ) ; 
          if ( evertex -> hasKey() )  
          { stream << " #" << toString ( evertex -> key() ) ; }
          stream << " "    << evertex->type()  ;
        }
        //
        print ( evertex -> products () . begin ()  ,
                evertex -> products () . end   ()  ,  
                stream            ,   term         , 
                accept            ,   mark         ,
                prefix + "   "    ,   depth + 1    ) ;   // RECURSION 
      }
      //
      return stream ;
    }
    // ========================================================================    
    /** print all independent MC-trees from LHCb::MCParticle::ConstVector
     * 
     *  @code 
     *
     *    MCDecayChain dc ;
     *    const LHCb::MCParticle::ConstVector particles = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( particles , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gaudi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( particles , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::MCTrees::buildTrees 
     *  @param particles pointer to the container of particles 
     *  @param stream    reference to output stream 
     *  @param term      stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const LHCb::MCParticle::ConstVector& particles , 
                    STREAM&                              stream         , 
                    TERMINATOR                           term           ,
                    const ACCEPT&                        accept         ,
                    const MARK&                          mark           ,
                    const std::string&                   prefix  = " "  ,
                    const size_t                         depth   = 0    ) const    
    {
      //
      // temporary storage of particles 
      LHCb::MCParticle::ConstVector trees ;
      trees.reserve ( particles.size() ) ;
      // get the independent MC-trees 
      LoKi::MCTrees::buildTrees 
        ( particles.begin () , 
          particles.end   () , std::back_inserter( trees ) ) ;
      return print 
        ( trees.begin () , 
          trees.end   () , stream , term , accept , mark , prefix , depth ) ;
    } 
    // ========================================================================    
    /** print all independent MC-trees from LHCb::MCParticle::Container
     * 
     *  @code 
     *
     *    MCDecayChain dc ;
     *    const LHCb::MCParticle::Container* particles = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( particles , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gaudi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( particles , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::MCTrees::buildTrees 
     *  @param particles pointer to the container of particles 
     *  @param stream    reference to output stream 
     *  @param term      stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const LHCb::MCParticle::Container* particles , 
                    STREAM&                              stream         , 
                    TERMINATOR                           term           ,
                    const ACCEPT&                        accept         ,
                    const MARK&                          mark           ,
                    const std::string&                   prefix  = " "  ,
                    const size_t                         depth   = 0    ) const    
    {
      // invalid particle 
      if ( 0 == particles ) 
      { 
        Error ( " LHCb::MCParticle::Container* points to NULL" ) ; 
        return stream ;                                         // RETURN 
      }
      //
      // temporary storage of particles 
      LHCb::MCParticle::ConstVector trees ;
      trees.reserve ( particles->size() ) ;
      //
      // build independent MC-trees 
      LoKi::MCTrees::buildTrees 
        ( particles -> begin () , 
          particles -> end   () , std::back_inserter( trees ) ) ;
      return print 
        ( trees.begin () , 
          trees.end   () , stream , term , accept , mark , prefix , depth ) ;
    } 
    // ======================================================================== 
  public:
    // ========================================================================    
    /// predefined printout    
    std::string print_ 
    ( const LHCb::MCParticle*      p                           , 
      const LoKi::Types::MCCuts&   accept = AcceptMC ( true  ) , 
      const LoKi::Types::MCCuts&   mark   = AcceptMC ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LoKi::Types::MCRange&  p                           , 
      const LoKi::Types::MCCuts&   accept = AcceptMC ( true  ) , 
      const LoKi::Types::MCCuts&   mark   = AcceptMC ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::MCParticle::ConstVector& p                   , 
      const LoKi::Types::MCCuts&   accept = AcceptMC ( true  ) , 
      const LoKi::Types::MCCuts&   mark   = AcceptMC ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::MCParticle::Container* p                     , 
      const LoKi::Types::MCCuts&   accept = AcceptMC ( true  ) , 
      const LoKi::Types::MCCuts&   mark   = AcceptMC ( false ) ) const ;
    // ========================================================================
  private:
    // ========================================================================    
    /// print all end-vertices information 
    bool                  m_vertexe ; // print all end-vertices  information  
    /// print all end-vertices information 
    bool                  m_vertexd ; // print all end-vertices  information  
    // ========================================================================
  } ;
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
STREAM& LoKi::MCDecayChain::print
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
// The END 
// ============================================================================
#endif // LOKI_MCDECAYCHAIN_H
// ============================================================================
