// $Id: PartonicJets2HepMCJets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
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

#include "LoKi/ILoKiSvc.h"

#include "Relations/Relation2D.h"

// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/ParticleCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/GenExtract.h"
#include "LoKi/PhysExtract.h"
// ============================================================================

namespace LoKi
{
  class PartonicJets2HepMCJets
    : public virtual IJets2Jets
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::PartonicJets2HepMCJets> ;
  public:

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    StatusCode finalize() { return GaudiTool::finalize ();};
    void  makeRelation( const IJets2Jets::Jets& partonjets
                        , const IJets2Jets::Jets& hepmcjets, IJets2Jets::Table &table) const ;

  protected:
    // ========================================================================
    /// standard constructor
    PartonicJets2HepMCJets
    ( const std::string& type   , ///< tool type ???
      const std::string& name   , ///< tool name
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      , m_addresses ( 1 , LHCb::HepMCEventLocation::Default )
      , m_allpartons (false)
      , m_ids   ()
      , m_hepMCfromStdHepMC ("Relations/Phys/StdHepMCRef2HepMC")
      , m_idsHadron   ()
      , m_hadronCut (LoKi::Cuts::GALL)
    {
      //
      declareInterface<IJets2Jets>(this);
      //
      declareProperty ( "AllPartons"   , m_allpartons  ,"Match all partonic jets (true)"  ) ;
      m_ids.push_back ( "b"  ) ;
      //m_ids.push_back ( "c" ) ;
      //m_ids.push_back ( "u" ) ;
      //m_ids.push_back ( "s"  ) ;
      //m_ids.push_back ( "d"  ) ;
      m_idsHadron.push_back ( "B"  ) ;
      declareProperty ( "PartonInJet"  , m_ids  ,
                        "if AllPartons == false specify which parton it should contain to be matched") ;
      declareProperty ( "StdHepMC2HepMCTable"  , m_hepMCfromStdHepMC
                        , "Relation table between StdHepMCParticles and corresponding HepMC::GenParticle"      ) ;
      declareProperty ( "HadronInJet"  , m_idsHadron 
                        ,"if not empty, will only look for jets contining a given hadron flavour [C,B]") ;
    }
    /// virtual protected destructor
    virtual ~PartonicJets2HepMCJets() {}
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled
    PartonicJets2HepMCJets() ;
    // copy constructor is disabled
    PartonicJets2HepMCJets( const PartonicJets2HepMCJets& ) ;
    // assignement operator is disabled
    PartonicJets2HepMCJets& operator=( const PartonicJets2HepMCJets& ) ;
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Addresses ;
    //

    Addresses           m_addresses         ;
    bool                m_allpartons        ;///<Match all partonic jets if true, and only those in m_ids if false
    typedef std::vector<std::string> IDs    ;
    IDs                 m_ids               ;///< if AllPartons == false specify which parton it should contain to be matched
    std::string m_hepMCfromStdHepMC;///< Relation table between StdHepMCParticles and corresponding HepMC::GenParticle
    IDs                 m_idsHadron          ;///< if not empty, will only look for jets contining a given hadron flavour ["C","B"]
    LoKi::Types::GCut   m_hadronCut          ;

    // ========================================================================
  };
}
// end of namespace LoKi


// ============================================================================
// standard initialization of the tool
// ============================================================================
StatusCode LoKi::PartonicJets2HepMCJets::initialize()
{
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) return sc;

  // locate LoKi service (needed for some functions)
  LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  if (!m_allpartons && m_ids.empty()){
    return Error ( "PartonInJet is empty while AllPartons is set to false" ) ;
  }


  if (!m_idsHadron.empty()){
    // first reset
    if(m_idsHadron[0] == "B") m_hadronCut = LoKi::Cuts::GQUARK( LHCb::ParticleID::bottom ) && LoKi::Cuts::GHADRON;
    if(m_idsHadron[0] == "C") m_hadronCut = LoKi::Cuts::GQUARK( LHCb::ParticleID::charm  ) && LoKi::Cuts::GHADRON;
    // then set the others
    for (IDs::iterator id = m_idsHadron.begin() ; m_idsHadron.end()!= id ; id++){
      if((*id) == "B") m_hadronCut = m_hadronCut || (LoKi::Cuts::GQUARK( LHCb::ParticleID::bottom ) && LoKi::Cuts::GHADRON);
      if((*id) == "C") m_hadronCut = m_hadronCut || (LoKi::Cuts::GQUARK( LHCb::ParticleID::charm  ) && LoKi::Cuts::GHADRON);
    }
    if(  m_hepMCfromStdHepMC == "" )return Error ( "No relation table between StdHepMCParticles and corresponding HepMC::GenParticle is specified but you need it" ) ;
  }

  if ( 0 == loki ) { return StatusCode::FAILURE ; }
  return sc ;
}


void LoKi::PartonicJets2HepMCJets::makeRelation( const IJets2Jets::Jets&   partonjets
                                                 , const IJets2Jets::Jets& hepmcjets, IJets2Jets::Table &table ) const
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  typedef LHCb::Relation2D< LHCb::Particle,HepMC::GenParticle* >  Table2DHepMC2Part ;
  typedef LoKi::GenTypes::GenContainer Container ;
  // create the relation table and register it into TES

  Cut IsGoodPartonic  = ALL ;
  if (!m_allpartons){
    IsGoodPartonic = 0 < NINTREE ( ABSID == m_ids[0]);
    for (unsigned int i = 1; i<m_ids.size();i++){
      IsGoodPartonic = IsGoodPartonic || 0 < NINTREE ( ABSID == m_ids[i]);
    }
  }

  IJets2Jets::Jets partonjets_filt;
  for( IJets2Jets::Jets::const_iterator partonjet = partonjets.begin() ; partonjets.end()!= partonjet ; partonjet++ ) {
    const LHCb::Particle* p = *partonjet ;
    if (IsGoodPartonic(p)){partonjets_filt.push_back(p); }
  }


  //If specified, keep only the jets with hadron as specified
  IJets2Jets::Jets hepmcjets_filt;

  if(!m_idsHadron.empty()){
    // get the link between HepMCParticles and StdHepMCParticles
    Table2DHepMC2Part* table_togetHepMC = get<Table2DHepMC2Part>(m_hepMCfromStdHepMC);

    Container g_Hadron;
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ;
          m_addresses.end() != iaddr ; ++iaddr )
    {
      const LHCb::HepMCEvent::Container* events =  get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
      LoKi::Extract::genParticles ( events , std::back_inserter( g_Hadron  ) ,m_hadronCut ) ;
    }

    GCut   g_fromHadron= GFROMTREE ( g_Hadron.begin(),g_Hadron.end() ) ;
    for( IJets2Jets::Jets::const_iterator hepjet = hepmcjets.begin() ; hepmcjets.end()!= hepjet ; hepjet++ ) {
      const LHCb::Particle* p = *hepjet ;
      IJets2Jets::Jets daug_jet ;
      // extract the daugthers
      LoKi::Extract::getParticles ( p , std::back_inserter (daug_jet), PID!= "CELLjet" );
      bool haveOneFromHadron = false;
      // loop over the daughters to check if they are from the hadron
      for(IJets2Jets::Jets::iterator idaug_jet = daug_jet.begin() ; daug_jet.end()!= idaug_jet  ; idaug_jet++ )
      {
        Table2DHepMC2Part::Range links = table_togetHepMC ->relations (*idaug_jet ) ;
        for ( Table2DHepMC2Part::iterator ilink = links.begin() ;links.end() != ilink ; ++ilink )
        {
          const HepMC::GenParticle* gp  = ilink->to() ;
          if(gp==0)continue;
          if ( g_fromHadron( gp ) ) haveOneFromHadron =true ;
        }
      }
      if(haveOneFromHadron ){
        hepmcjets_filt.push_back(p);
      }

    }
  }
  else{
    for( IJets2Jets::Jets::const_iterator hepjet = hepmcjets.begin() ; hepmcjets.end()!= hepjet ; hepjet++ ) {
      hepmcjets_filt.push_back(*hepjet);
    }
  }

  std::vector<double> ehepMC;
  for ( IJets2Jets::Jets::const_iterator partonjet = partonjets_filt.begin() ; partonjets_filt.end() != partonjet ; ++partonjet )
  {
    double distmin = 100000000.;
    const LHCb::Particle* theclothestJet = NULL;
    for( IJets2Jets::Jets::iterator hepjet = hepmcjets_filt.begin() ; hepmcjets_filt.end()!= hepjet ; hepjet++ )
    {
      Fun dphi = DPHI(*hepjet);
      double dist =
        std::sqrt(std::pow((ETA(*hepjet)-ETA(*partonjet))/ETA(*partonjet),2)+
                  std::pow((dphi(*partonjet))/PHI(*partonjet),2)+std::pow((E(*hepjet)-E(*partonjet))/E(*partonjet),2));

      if(dist<distmin)
      {
        distmin=dist;
        theclothestJet = *hepjet;
      }
    }
    if(theclothestJet!=NULL) {
      table.relate(*partonjet,theclothestJet,1.);
      ehepMC.push_back(E(theclothestJet));
    }
  }


}



// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,PartonicJets2HepMCJets)
// ============================================================================
// The END
// ============================================================================
