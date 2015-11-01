// $Id: HepMCJets2HepMCJets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IJets2Jets.h"            // Interface
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"

#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"



#include "LoKi/ILoKiSvc.h"


#include "LoKi/ParticleCuts.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenParticleCuts.h"
#include "LoKi/GenExtract.h"
#include "LoKi/AlgoMC.h"
// ============================================================================

namespace LoKi
{
  class HepMCJets2HepMCJets
    : public virtual IJets2Jets
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::HepMCJets2HepMCJets> ;
  public:

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    StatusCode finalize() { return GaudiTool::finalize ();};
    void makeRelation( const IJets2Jets::Jets& StdPrimaryJets
                       , const IJets2Jets::Jets& StdSecondaryJets ,
                       IJets2Jets::Table &table) const ;
  protected:
    // ========================================================================
    /// standard constructor
    HepMCJets2HepMCJets
    ( const std::string& type   , ///< tool type ???
      const std::string& name   , ///< tool name
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
    {
      //
      declareInterface<IJets2Jets>(this);
      //
    }
    /// virtual protected destructor
    virtual ~HepMCJets2HepMCJets() {}
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled
    HepMCJets2HepMCJets() ;
    // copy constructor is disabled
    HepMCJets2HepMCJets( const HepMCJets2HepMCJets& ) ;
    // assignement operator is disabled
    HepMCJets2HepMCJets& operator=( const HepMCJets2HepMCJets& ) ;
    // ========================================================================
  private:
    // ========================================================================
    std::string m_output;///< Output relation table (1D) from partonics jet to StdHepMCJets to use as reference

    // ========================================================================
  };
}
// end of namespace LoKi


// ============================================================================
// standard initialization of the tool
// ============================================================================
StatusCode LoKi::HepMCJets2HepMCJets::initialize()
{
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) return sc;

  // locate LoKi service (needed for some functions)
  LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  if ( 0 == loki ) { return StatusCode::FAILURE ; }
  return sc;
}


void LoKi::HepMCJets2HepMCJets::makeRelation( const IJets2Jets::Jets& StdPrimaryJets
                                              , const IJets2Jets::Jets& StdSecondaryJets,
                                              IJets2Jets::Table &table ) const
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  typedef std::vector<LHCb::Particle*> Parts;

  // create the relation table and register it into TES
  // select
  for( IJets2Jets::Jets::const_iterator secjet =StdSecondaryJets.begin() ; 
       StdSecondaryJets.end()!= secjet ; secjet++ )
  {
    for(IJets2Jets::Jets::const_iterator primjet =StdPrimaryJets.begin() ; 
        StdPrimaryJets.end()!= primjet ; primjet++)
    {
      double weight_jetsec_jetprim = 0.;
      Parts daug_secjet ;

      LoKi::Extract::getParticles (*secjet, std::back_inserter (daug_secjet), PID!="CELLjet");
      Parts daug_primjet ;
      LoKi::Extract::getParticles (*primjet, std::back_inserter (daug_primjet), PID!="CELLjet");
      for(Parts::iterator idaug_secjet = daug_secjet.begin() ; 
          daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
      {
        for(Parts::iterator idaug_primjet = daug_primjet.begin() ; 
            daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
        {
          if(E(*idaug_secjet)==E(*idaug_primjet) && PX(*idaug_secjet)==PX(*idaug_primjet) && PY(*idaug_secjet)==PY(*idaug_primjet)
             && PZ(*idaug_secjet)==PZ(*idaug_primjet))
          {
            weight_jetsec_jetprim += E(*idaug_secjet)/E(*primjet);
            continue;
          }
        }
      }
      if(weight_jetsec_jetprim>0) table.relate((*primjet),(*secjet),weight_jetsec_jetprim);
    }
  }
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,HepMCJets2HepMCJets)
// ============================================================================
// The END
// ============================================================================
