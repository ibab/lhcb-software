// $Id$
// ============================================================================
#ifndef LOKI_GENDECAYCHAIN_H 
#define LOKI_GENDECAYCHAIN_H 1
// ============================================================================
// Include files
// ============================================================================
// GenEvent
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/DecayChainBase.h"
#include "LoKi/BuildGenTrees.h"
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
  /** @class GenDecayChain
   *  Utility to print the decays
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2011-06-03
   */
  class GAUDI_API GenDecayChain : public virtual DecayChainBase 
  {
  protected:
    // ========================================================================
   typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant AcceptGen ;
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
    GenDecayChain
    ( const size_t          maxDepth = 10           , 
      const bool            vertex   = false        , 
      const Mode            mode     = LV_WITHPT    ,
      const MSG::Color&     fg       = MSG::YELLOW  ,
      const MSG::Color&     bg       = MSG::RED     ,
      const bool            vertexe  = true         ) ;
    /** Standard constructor
     *  @param vertexe   end-vertex info for MC particles to be printed 
     */
    GenDecayChain
    ( const LoKi::DecayChainBase& base , 
      const bool                  vertexe  = true   ) ;
    /// virtual destructor 
    virtual ~GenDecayChain () ;                         // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// print end-vertex info ?
    bool              vertexe     () const { return m_vertexe  ; }
    void              setVertexE  (  const bool value ) 
    { m_vertexe    = value ; }
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
    /** print the decay chain for HepMC::GenParticle
     * 
     *  @code 
     *
     *    GenDecayChain dc ;
     *    const HepMC::GenParticle* head = ... ; 
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
    STREAM& print ( const HepMC::GenParticle*  particle       , 
                    STREAM&                    stream         , 
                    TERMINATOR                 term           ,
                    const ACCEPT&              accept         ,
                    const MARK&                mark           ,
                    const std::string&         prefix  = " "  ,
                    const size_t               depth   = 0    ) const    
    {
      // invalid particle 
      if ( 0 == particle ) 
      { 
        Error ( " HepMC::GenParticle* points to NULL" ) ; 
        return stream ;                             // RETURN 
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
        const std::string   p_name  = name ( LHCb::ParticleID ( particle->pdg_id() ) ) ;
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
      { stream << " ... (max depth)     " ; return stream ; } // RETURN
      //
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " ; return stream ; } // RETURN 
      //
      stream << toString( LoKi::LorentzVector( particle->momentum() ) ) ;
      stream << " #" << toString ( particle->barcode () ) 
             << "/"  << toString ( particle->status  () )  ;
      //
      HepMC::GenVertex* vertex = particle->production_vertex ();
      if     ( m_vertex && 0 != vertex ) 
      {
        // use the terminator 
        stream << term     ;  
        stream << blank ( _indent + 5 ) ;
        stream << " Origin    " 
               << toString ( LoKi::Point3D( vertex -> point3d() ) ) ;
        stream << " #" << toString ( vertex -> barcode() ) ; 
      }
      // print all end-vertices 
      HepMC::GenVertex* evertex = particle->end_vertex() ;
      if ( m_vertex && m_vertexe && 0 != evertex ) 
      { 
        // use terminator 
        stream << term      ;
        stream << blank ( _indent + 5 ) ;
        stream << " EndVertex " 
               << toString( LoKi::Point3D ( evertex -> point3d() ) ) ; 
        stream << " #" << toString ( evertex -> barcode() ) ; 
      }
      //
      if ( 0 != evertex ) 
      {
        HepMC::GenVertex::particle_iterator begin = 
          evertex->particles_begin ( HepMC::children ) ;
        HepMC::GenVertex::particle_iterator end   = 
          evertex->particles_end   ( HepMC::children ) ;
        print ( begin  , end   , 
                stream , term  , 
                accept , mark  , 
                prefix + "   " , 
                depth  + 1     ) ; 
      }
      //
      return stream ;
    }
    // ========================================================================    
    /** print the decay chain for HepMC::GenEvent
     * 
     *  Essentially it prints the decay tree from "signal_vertex" 
     *  or vertex with barcode equal to -1 
     *
     *  @code 
     *
     *    GenDecayChain dc ;
     *    const HepMC::GenEvent* event = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( event , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gaudi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( event , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @param event    pointer to the event  
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const HepMC::GenEvent*     event          , 
                    STREAM&                    stream         , 
                    TERMINATOR                 term           ,
                    const ACCEPT&              accept         ,
                    const MARK&                mark           ,
                    const std::string&         prefix  = " "  ,
                    const size_t               depth   = 0    ) const    
    {
      // invalid event 
      if ( 0 == event ) 
      { 
        Error ( " HepMC::GenEvent* points to NULL" ) ; 
        return stream ; 
      }
      // select the trees 
      LoKi::GenTypes::GenContainer trees ;
      trees.reserve( event->particles_size() ) ;
      LoKi::GenTrees::buildTrees 
        ( event->particles_begin () ,
          event->particles_end   () , 
          std::back_inserter( trees ) );
      // print the trees 
      return print 
        ( trees.begin () , 
          trees.end   () , 
          stream , term , accept , mark , prefix , depth ) ;
    }
    // ========================================================================
    /** print the decay chain for LHCb::HepMCEvent
     * 
     *  Essentially it prints the underlying HepMC::GenEvent 
     *
     *  @code 
     *
     *    GenDecayChain dc ;
     *    const LHCb::HepMCEvent* event = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( event , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gaudi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( event , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @param event    pointer to the event  
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const LHCb::HepMCEvent*    event          , 
                    STREAM&                    stream         , 
                    TERMINATOR                 term           ,
                    const ACCEPT&              accept         ,
                    const MARK&                mark           ,
                    const std::string&         prefix  = " "  ,
                    const size_t               depth   = 0    ) const    
    {
      // invalid event 
      if ( 0 == event ) 
      { 
        Error ( " LHCb::HepMCEvent* points to NULL" ) ;
        return stream ; 
      }
      // print HepMC::GenEvent
      return print 
        ( event->pGenEvt() , stream , term , accept , mark , prefix , depth ) ;
    }
    // ========================================================================
    /** print the decay chain for LHCb::HepMCEvent::Container
     * 
     *  Essentially it prints the underlying HepMC::GenEvent 
     *
     *  @code 
     *
     *    GenDecayChain dc ;
     *    const LHCb::HepMCEvent:Container* events = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( events , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gaudi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( events , log , endmsg          ) ;
     *
     *  @endcode 
     * 
     *  @param event    pointer to the events  
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const LHCb::HepMCEvent::Container* events , 
                    STREAM&                    stream         , 
                    TERMINATOR                 term           ,
                    const ACCEPT&              accept         ,
                    const MARK&                mark           ,
                    const std::string&         prefix  = " "  ,
                    const size_t               depth   = 0    ) const    
    {
      // invalid event 
      if ( 0 == events ) 
      { 
        Error ( " LHCb::HepMCEvent::Container* points to NULL" ) ; 
        return stream ; 
      }
      // print HepMC::GenEvent
      return print 
        ( events -> begin () ,
          events -> end   () , 
          stream , term , accept , mark , prefix , depth ) ;
    }
    // ========================================================================   
  public:
    // ========================================================================    
    /// predefined printout    
    std::string print_ 
    ( const HepMC::GenParticle*   p                            , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LoKi::Types::GRange&  e                     , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LoKi::GenTypes::GenContainer&  e                   , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const HepMC::GenEvent*      e                            , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::HepMCEvent*     e                            , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    /// predefined printout    
    std::string print_ 
    ( const LHCb::HepMCEvent::Container* e                     , 
      const LoKi::Types::GCuts&   accept = AcceptGen ( true  ) , 
      const LoKi::Types::GCuts&   mark   = AcceptGen ( false ) ) const ;
    // ========================================================================    
  private:
    // ========================================================================    
    /// print all end-vertices information 
    bool                  m_vertexe ; // print all end-vertices  information  
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
STREAM& LoKi::GenDecayChain::print
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
#endif // LOKI_GENDECAYCHAIN_H
// ============================================================================
