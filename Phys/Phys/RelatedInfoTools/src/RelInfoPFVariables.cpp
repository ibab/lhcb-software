// Include files
// local

#include "RelInfoPFVariables.h"
#include "Kernel/RelatedInfoNamed.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include "Event/Particle.h"
#include "math.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoPFVariables
//
// 2014-06-30 : Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoPFVariables )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoPFVariables::RelInfoPFVariables( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent):
GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "Variables",
                   m_variables,
                   "List of variables to store (store all if empty)");
  declareProperty( "PFOutputLocation",  m_PFLocation  =  "Phys/PFParticles/Particles"
                   ,"Location of the PFParticles");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoPFVariables::~RelInfoPFVariables() {}


//====================================================================
// Initialize DVAlg, etc
//====================================================================
StatusCode RelInfoPFVariables::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  //initialize the tool keys
  m_keys.clear();

  for ( const auto& var : m_variables )
  {
    short int key = RelatedInfoNamed::indexByName( var );
    if (key != RelatedInfoNamed::UNKNOWN) {
      m_keys.push_back( key );
      debug() << "Adding variable " << var << ", key = " << key << endmsg;
    } else {
      Warning("Unknown variable " + var + ", skipping");
    }
  }


  debug()<< "Initialization correct" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// invariant mass for a list of particles
//=============================================================================
double RelInfoPFVariables::invariantMass(std::vector<Gaudi::LorentzVector> mypars){
  Gaudi::LorentzVector vsum;
  if (mypars.empty()) return -1.;
  for (std::vector<Gaudi::LorentzVector>::const_iterator it1=mypars.begin();
       it1 != mypars.end(); ++it1 ) vsum+=(*it1);
  return vsum.M();
}


//=============================================================================
// Empty properties list
//=============================================================================
void RelInfoPFVariables::emptyProperties(void)
{
  m_properties["charged_mass"] = -1.;
  m_properties["all_mass"] = -1.;
  m_properties["charged_totpt"] = -1.;
  m_properties["all_totpt"] = -1.;
  m_properties["charged_misspt"] = -1.;
  m_properties["all_misspt"] = -1.;
}


//=============================================================================
// Fill Info structure
//=============================================================================
StatusCode RelInfoPFVariables::calculateRelatedInfo( const LHCb::Particle *top,
                                                     const LHCb::Particle *part )
{

  debug() << "The top particle is " << top->key() << endmsg;
  debug() << "And the particle is " << part->key() << endmsg;

  emptyProperties();
  fillMap();
  debug()<< "Filled empty dicts" << endmsg;

  // Get DVAlgo
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) return Error( "Could not get DVAlgorithm" );

  // only fill infos for top particle!
  if (part!=top) return StatusCode::SUCCESS;

  if (!exist<LHCb::Particles>(m_PFLocation)){
    debug()<<"No PFParticles available"<<endmsg;
    return StatusCode::SUCCESS;
  }
  LHCb::Particles* PFParticles = get<LHCb::Particles>(m_PFLocation);

  // get bestPV of the top particle
  int mypvkey,pvkey;
  const LHCb::VertexBase* BPV;
  BPV = dva->bestVertex(top);
  if (BPV) mypvkey  = BPV->key();
  else mypvkey  = -1;

  // initialize important variables
  std::vector<Gaudi::LorentzVector> mypars_cha,mypars_all;
  double missptx_cha=0.,misspty_cha=0.,missptx_all=0.,misspty_all=0.;
  double pttot_cha=0.,pttot_all=0.;

  // loop in PFparticles
  for (LHCb::Particles::const_iterator it1=PFParticles->begin(); it1 != PFParticles->end(); ++it1 ){
    LHCb::Particle* pfpart = (*it1);
    // force running only in particles with a track whose PV is the same as the top particle
    if (pfpart->proto() && pfpart->proto()->track()) {
      BPV = dva->bestVertex(pfpart);
      if (BPV) pvkey  = BPV->key();
      else pvkey  = -1;
      if (pvkey==mypvkey) {
        //misspt
        missptx_cha+=pfpart->momentum().x();
        misspty_cha+=pfpart->momentum().y();
        //mass
        Gaudi::LorentzVector v1;
        v1.SetPxPyPzE(pfpart->momentum().x(),pfpart->momentum().y(),pfpart->momentum().z(),pfpart->momentum().e());
        mypars_cha.push_back(v1);
        //tot pT
        pttot_cha+=pfpart->pt();
      }
      else debug()<<"Skipping particle, not same PV"<<endmsg;
    }
    //misspt
    missptx_all+=pfpart->momentum().x();
    misspty_all+=pfpart->momentum().y();
    //mass
    Gaudi::LorentzVector v2;
    v2.SetPxPyPzE(pfpart->momentum().x(),pfpart->momentum().y(),pfpart->momentum().z(),pfpart->momentum().e());
    mypars_all.push_back(v2);
    //tot pT
    pttot_all+=pfpart->pt();
  }

  m_properties["charged_mass"] = invariantMass(mypars_cha);
  m_properties["all_mass"] = invariantMass(mypars_all);
  m_properties["charged_totpt"] = pttot_cha-top->pt();
  m_properties["all_totpt"] = pttot_all-top->pt();
  m_properties["charged_misspt"] = sqrt(missptx_cha*missptx_cha+misspty_cha*misspty_cha);
  debug()<<"missptx_all="<<missptx_all<<endmsg;
  debug()<<"misspty_all="<<misspty_all<<endmsg;
  m_properties["all_misspt"] = sqrt(missptx_all*missptx_all+misspty_all*misspty_all);
  debug()<<"m_properties="<<m_properties<<endmsg;

  fillMap();

  return StatusCode::SUCCESS;
}


//=============================================================================
// Fill Output m_map
//=============================================================================
void  RelInfoPFVariables::fillMap(){

  m_map.clear();
  for ( const auto key : m_keys ){

    float value = 0;
    switch (key) {
    case RelatedInfoNamed::PFCHARGEDMASS    : value = m_properties["charged_mass"]; break;
    case RelatedInfoNamed::PFALLMASS    : value = m_properties["all_mass"]; break;
    case RelatedInfoNamed::PFCHARGEDTOTPT : value = m_properties["charged_totpt"]; break;
    case RelatedInfoNamed::PFALLTOTPT    : value = m_properties["all_totpt"]; break;
    case RelatedInfoNamed::PFCHARGEDMISSPT    : value = m_properties["charged_misspt"]; break;
    case RelatedInfoNamed::PFALLMISSPT    : value = m_properties["all_misspt"]; break;

    default: value = 0.; break;
    }

    debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endreq;
    m_map.insert( std::make_pair(key,value) );
  }

}

LHCb::RelatedInfoMap* RelInfoPFVariables::getInfo(void){
  return &m_map;
}


