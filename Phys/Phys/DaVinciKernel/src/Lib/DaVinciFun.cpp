
// Gaudi
#include "GaudiKernel/SmartIF.h"

// DaVinciInterfaces
#include "Kernel/ISetInputParticles.h"

// local
#include "Kernel/DaVinciFun.h"

//-----------------------------------------------------------------------------
// Implementation file for DaVinciFun
//
// 2009-08-02 : Juan Palacios
//-----------------------------------------------------------------------------
namespace DaVinci
{

  const LHCb::VertexBase* bestVertexBase( const Particle2Vertex::Table::Range& range )
  {
    return ( range.empty() ? NULL : range.back().to() );
  }

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Table::Range& range)
  {
    return ( range.empty() ? NULL : 
             dynamic_cast<const LHCb::RecVertex*>( range.back().to() ) );
  }

  namespace Utils
  {
    // ========================================================================
    /*  set the input particles for some component
     *  @param component the component inself
     *  @param input the intut data
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-07-11
     */
    // ========================================================================
    StatusCode setInput
    ( IInterface*                        component ,
      const LHCb::Particle::ConstVector& input     )
    {
      if ( !component     ) { return StatusCode ( 300 ) ; }        // RETURN
      // get the interface
      SmartIF<ISetInputParticles> alg ( component ) ;
      // valid ?
      if ( !alg           ) { return StatusCode ( 301 ) ; }        // RETURN
      // use the valid inyterface
      return alg->setInput ( input ) ;                             // RETURN
    }

    // ========================================================================
    void findDecayTree(const LHCb::Particle*        head,
                       LHCb::Particle::ConstVector& particles,
                       LHCb::Vertex::ConstVector&   vertices,
                       DaVinci::Utils::IParticlePredicate* truncate)
    {

      if ( truncate && (*truncate)(head) ) return;

      if ( particles.end() == std::find ( particles.begin() ,
                                          particles.end() ,
                                          head ) ) 
      {
        particles.push_back ( head ) ;
      }
      //
      if ( head->endVertex() &&
           vertices.end() == std::find ( vertices.begin() ,
                                         vertices.end() ,
                                         head->endVertex() ) )
      {
        vertices.push_back( head->endVertex() ); // save Vertex
      }

      // Loop on daughters
      for ( SmartRefVector<LHCb::Particle>::const_iterator iDaughter = 
              head->daughters().begin(); 
            iDaughter != head->daughters().end(); ++iDaughter )
      {
        findDecayTree( *iDaughter, particles, vertices, truncate );
      }

      // loop over outgoing particles
      if ( head->endVertex() ) 
      {
        for ( SmartRefVector<LHCb::Particle>::const_iterator iPart = 
                head->endVertex()->outgoingParticles().begin();
              iPart != head->endVertex()->outgoingParticles().end(); ++iPart )
        {
          findDecayTree( *iPart, particles, vertices, truncate );
        }
      }

      return;

    }

    bool decayTreeInTES( const LHCb::Particle* head )
    {
      if (!DaVinci::Utils::inTES(head)) return false;
      // Loop on daughters
      for ( SmartRefVector<LHCb::Particle>::const_iterator iDaughter =
              head->daughters().begin(); 
            iDaughter != head->daughters().end(); ++iDaughter )
      {
        if ( !decayTreeInTES(*iDaughter) ) return false;
      }

      // loop over outgoing particles
      if ( 0 != head->endVertex() ) 
      {
        if ( !DaVinci::Utils::inTES(head->endVertex()) ) return false;
        for ( SmartRefVector<LHCb::Particle>::const_iterator iPart =
                head->endVertex()->outgoingParticles().begin();
              iPart != head->endVertex()->outgoingParticles().end(); ++iPart )
        {
          if ( !decayTreeInTES(*iPart) ) return false;
        }
      }

      return true;
    }

  } // namespace Utils

} // namespace DaVinci
