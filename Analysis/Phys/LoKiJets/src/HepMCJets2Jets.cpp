// $Id: HepMCJets2Jets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
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

#include "LoKi/PhysMCParticleCuts.h"

#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"

#include "LoKi/MCMatch.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatchDicts.h"

#include "LoKi/LoKiPhys.h"
#include <GaudiKernel/IIncidentListener.h>
#include <GaudiKernel/IIncidentSvc.h>
// ============================================================================
// LoKiGen 
// ============================================================================
#include "LoKi/GenParticleCuts.h" 
#include "LoKi/GenExtract.h" 
#include "LoKi/AlgoMC.h" 
// ============================================================================

namespace LoKi 
{
  class IReporter;
}
namespace LoKi
{
  class HepMCJets2Jets
    : public virtual IJets2Jets
    , public virtual IIncidentListener
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<LoKi::HepMCJets2Jets> ;
  public: 

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    StatusCode finalize() ;
    const IJets2Jets::Table makeRelation( const IJets2Jets::Jets& StdPrimaryJets  , const IJets2Jets::Jets& StdSecondaryJets  ) const ;

    virtual void handle( const Incident& );

  protected:
    // ========================================================================
    /// standard constructor
    HepMCJets2Jets 
    ( const std::string& type   , ///< tool type ??? 
      const std::string& name   , ///< tool name 
      const IInterface*  parent ) 
      : GaudiTool ( type, name , parent )
      , m_tableHepMC2StdHepMC ( "Relations/Phys/StdHepMCRef2HepMC" )
      , m_tables()
      , m_reporter(0)
      , m_matcher(0)
    {
      //
      declareInterface<IJets2Jets>(this);
      //
	    declareProperty ( "StdHepMC2HepMCTable"            , m_tableHepMC2StdHepMC   ,
                        "Relation table between StdHepMCParticles and corresponding HepMC::GenParticle"      ) ;  
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
      declareProperty("RelTableLocations",  m_tables );
    } 
    /// virtual protected destructor 
  virtual ~HepMCJets2Jets() {} 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    HepMCJets2Jets() ;
    // copy constructor is disabled 
    HepMCJets2Jets( const HepMCJets2Jets& ) ;
    // assignement operator is disabled 
    HepMCJets2Jets& operator=( const HepMCJets2Jets& ) ;

    
    inline LoKi::MCMatch matcher() const 
    {
      return LoKi::MCMatch( m_matcher );
    }
    void addTables(LoKi::MCMatchObj* matcher) const;

    // ========================================================================
  private:
    // ======================================================================== 
    std::string       m_tableHepMC2StdHepMC ;///< Location of relation table between HepMC and StdHepMC
    LoKi::ILoKiSvc* m_loki ;

    // for the mcmatching
    typedef std::vector<std::string> Addresses ;
    Addresses m_tables;
    LoKi::IReporter* m_reporter;
    LoKi::MCMatchObj* m_matcher;
    IIncidentSvc* m_incSvc;
    
    mutable bool m_loaded ;

    
    // ========================================================================
    // ========================================================================
  };
}
 // end of namespace LoKi 


// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::HepMCJets2Jets::initialize() 
{

  // locate LoKi service (needed for some functions) 
  m_loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;


  m_reporter = tool<LoKi::IReporter>( "LoKi::Reporter", this ) ;

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "RelTableLocations: " << endmsg;
    for (Addresses::const_iterator iAddr = m_tables.begin(); iAddr!= m_tables.end(); ++iAddr) {
      verbose() << "\t" << *iAddr << endmsg;
    }
  }

  m_incSvc = svc<IIncidentSvc>("IncidentSvc", true);
  if (0!=m_incSvc) m_incSvc->addListener( this, IncidentType::EndEvent);

  // create the new matcher 
  m_matcher = new LoKi::MCMatchObj( "P2MCRelator" , m_reporter ) ;
  // increment the reference counter 
  m_matcher->addRef() ;

  m_loaded = false ;
  
  return (0!=m_reporter && 0!=m_matcher && 0!= m_incSvc && 0!= m_loki) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

void LoKi::HepMCJets2Jets::handle(const Incident&) 
{
  if ( 0 != m_matcher ) { m_matcher->clear(); }
  m_loaded = false ;

}

StatusCode LoKi::HepMCJets2Jets::finalize()
{
  m_incSvc=0;
  
  if ( 0 != m_matcher ) 
  { 
    m_matcher -> clear () ; 
    long count = m_matcher->refCount();
    while ( 0 < count ) { count = m_matcher->release() ; }
    m_matcher = 0 ; 
  }  
  
  m_loaded = false ;

  return StatusCode::SUCCESS ;
}

const IJets2Jets::Table LoKi::HepMCJets2Jets::makeRelation( const IJets2Jets::Jets& StdPrimaryJets  , const IJets2Jets::Jets& StdSecondaryJets  ) const 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  typedef std::vector<const LHCb::Particle*> Parts;
 
  if ( !m_loaded ) { addTables ( m_matcher ) ; m_loaded = true ; }


  // Table to get the HepMCParticle from the StdHepMCParticle
	typedef LHCb::Relation2D<LHCb::Particle,HepMC::GenParticle*>  Table2DHepMC2Part ;

  // create the relation table and register it into TES 
  IJets2Jets::Table* table = new IJets2Jets::Table() ;
  

  LHCb::HepMC2MC2D* tableHepMC2MC = 
    get<LHCb::HepMC2MC2D> ( LHCb::HepMC2MCLocation::Default ) ;
    
  LoKi::MCMatch mc = this->matcher();


  Table2DHepMC2Part* table_togetHepMC = get<Table2DHepMC2Part>(m_tableHepMC2StdHepMC);
  // Loop over the StdJets
  for( LHCb::Particles::const_iterator secjet =StdSecondaryJets.begin() ; StdSecondaryJets.end()!= secjet ; secjet++ )
  { 
    // Loop over the StdHepMCJets
    Parts daug_secjet;    
    LoKi::Extract::getParticles (*secjet,std::back_inserter (daug_secjet), PID!="CELLjet");

    for(LHCb::Particles::const_iterator primjet =StdPrimaryJets.begin() ; StdPrimaryJets.end()!= primjet ; primjet++)
    {
      
      Parts daug_primjet;
      LoKi::Extract::getParticles (*primjet,std::back_inserter (daug_primjet), PID!="CELLjet");
      double weight_jetsec_jetprim = 0.;

      // Object to store the HepMCParticles of the jet
      LoKi::GenTypes::GenContainer TheHepMCVect;

      //Loop over the StdHepMC Particles of the HepMCjet
      for(Parts::iterator idaug_primjet = daug_primjet.begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
      {
        Table2DHepMC2Part::Range links = table_togetHepMC ->relations (*idaug_primjet ) ;
        for ( Table2DHepMC2Part::iterator ilink = links.begin() ;links.end() != ilink ; ++ilink )
        {
          const HepMC::GenParticle* gp  = ilink->to() ;
          if ( 0 == gp ) { continue ;}
          TheHepMCVect.push_back(gp);
        }
      }
      Cut StdMC_fromGen = GMCTRUTH ( TheHepMCVect , tableHepMC2MC , mc ) ;
      
      //Loop over the Std Particles of the jet
      for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
      {
        
        if (0 ==(*idaug_secjet)) { continue ;}
        if(StdMC_fromGen(*idaug_secjet)){
           weight_jetsec_jetprim += E(*idaug_secjet)/E(*primjet);
        }
      }
      
      if(weight_jetsec_jetprim>0) table->relate(*primjet,*secjet,weight_jetsec_jetprim);
    }
  }

  return (*table) ;
}
void LoKi::HepMCJets2Jets::addTables(LoKi::MCMatchObj* matcher) const 
{
  for (Addresses::const_iterator item = m_tables.begin(); item!=m_tables.end(); ++item) {
    const std::string& address = *item;
    if (exist<LoKi::Types::TablePP2MC>(address) ) {
      LoKi::Types::TablePP2MC* table = get<LoKi::Types::TablePP2MC>(address);
      matcher->addMatchInfo(table);
    } else if (exist<LoKi::Types::TableP2MC>(address) ) {
      LoKi::Types::TableP2MC* table = get<LoKi::Types::TableP2MC>(address);
      matcher->addMatchInfo(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ).ignore() ; 
    }
  }
  m_loaded = true ;
}


// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,HepMCJets2Jets);
// ============================================================================
// The END 
// ============================================================================
