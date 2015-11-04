#include "FilterJet.h"

//=======================================
//
// addapted from FilterDesktop
//
//=======================================

DECLARE_ALGORITHM_FACTORY( FilterJet )

//=============================================================================
// Constructor
FilterJet::FilterJet(const std::string& name,
                     ISvcLocator* pSvcLocator)
: DaVinciAlgorithm(name,pSvcLocator)
  , m_tagToolName ("LoKi__TopoTag")
  , m_tagTool     ( 0   )
  , m_save_all (false)
  , m_save_addinfo (true)
  , m_start_addinfo(-1)
  , m_SubPID("")
  , m_cut(-10000.0)

{
  declareProperty ( "tagToolName"        ,  m_tagToolName   ) ;
  declareProperty ( "saveAllJetWithInfo"        ,  m_save_all   ) ;
  declareProperty ( "saveTaggerInfo"        ,  m_save_addinfo   ) ;
  declareProperty ( "startNumberForAddInfo"        ,  m_start_addinfo  ) ;
  declareProperty ( "setNewPID"       ,  m_SubPID  ) ;
  declareProperty ( "Cut"       ,  m_cut  ) ;
}

//=============================================================================
// Initalise
StatusCode  FilterJet::initialize ()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tagTool = tool<IJetTagTool>(m_tagToolName, this);
 
  return sc;
}

StatusCode FilterJet::execute()
{

  i_markedParticles.clear();

  const LHCb::Particle::ConstVector& jets = i_particles () ;

  for ( LHCb::Particle::ConstVector::const_iterator ijet = jets.begin(); 
        jets.end() != ijet ;++ijet ) 
  {
    const LHCb::Particle* jet = *ijet;

    std::map<std::string,double> property;
    const bool tagBool = m_tagTool->calculateJetProperty(jet, property);
    if ((tagBool && property["Tag"] > m_cut )|| m_save_all)
    {

      LHCb::Particle* tagJet = jet->clone();

      if ( !m_SubPID.empty() )
      {
        LHCb::IParticlePropertySvc* ppSvc =
          svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
        const LHCb::ParticleProperty* pp = ppSvc->find ( m_SubPID ) ;
        tagJet->setParticleID(pp->particleID());
      }

      const LHCb::VertexBase* bestPV = getStoredBestPV(jet);
      if ( 0!= bestPV ) { this->relate ( tagJet , bestPV ) ; }
      if(m_save_addinfo){
        int infokey = property["extraInfo"];
        if(m_start_addinfo>0)
          infokey = m_start_addinfo;
        for ( std::map<std::string, double>::iterator it = property.begin();  
              it != property.end(); ++it )
        {
          if(!tagJet->hasInfo(infokey)){
            if(it->first != "extraInfo")
              tagJet->addInfo(infokey++, (it->second));
          } 
          else
          {
            warning() << "Info Key" << infokey 
                      << " already used... please check the starting point." 
                      << endmsg;
          }

        }

      }

      i_markedParticles.push_back( tagJet ) ;

    }

  }

  setFilterPassed(true);

  return StatusCode::SUCCESS ;
}


// ============================================================================
// save (clone if needed) selected particles in TES
// ============================================================================
template <class Particles>
StatusCode FilterJet::_save ( ) const
{
  //
  Particles* p_tes = new Particles () ;
  //
  put ( p_tes ,    particleOutputLocation () ) ;


  for ( std::vector<LHCb::Particle*>::const_iterator ip = i_markedParticles.begin(); 
        ip != i_markedParticles.end(); ++ip )
  {
    //
    LHCb::Particle* p = *ip ;
    if ( NULL == p ) { continue ; } // CONTINUE

    p_tes -> insert ( p ) ;
    //

  }

  return ( i_markedParticles.size() != p_tes->size() ?
           StatusCode::FAILURE : StatusCode::SUCCESS   );
}

// ============================================================================

// ============================================================================

StatusCode FilterJet::_saveInTES ()
{
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "FilterJet::_saveInTES " << i_markedParticles.size() << " Particles" << endmsg;
  }
  return this -> _save<LHCb::Particle::Container> ();
}
// ============================================================================


// ============================================================================
void FilterJet::writeEmptyTESContainers()
{
  return true ?
    writeEmptyKeyedContainers()  :
    writeEmptySharedContainers() ;

}
// ============================================================================
void FilterJet::writeEmptyKeyedContainers() const
{
  LHCb::Particle::Container* container = new LHCb::Particle::Container();
  put(container, particleOutputLocation());
}
// ============================================================================
void FilterJet::writeEmptySharedContainers() const
{
  LHCb::Particle::Selection* container = new LHCb::Particle::Selection();
  put(container, particleOutputLocation());
}
