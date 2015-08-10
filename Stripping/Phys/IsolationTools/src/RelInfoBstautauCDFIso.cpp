#include "GaudiKernel/ToolFactory.h"
#include "RelInfoBstautauCDFIso.h"
#include "Kernel/RelatedInfoNamed.h"
#include "Event/Particle.h"
// kernel
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
//#include "Kernel/IOnOffline.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>


#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
//#include "TMVA/Reader.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Math/Boost.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoBstautauCDFIso
// Converted from ConeVariables by A. Shires
//
// 2014-07-25 : Giampiero Mancinelli
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoBstautauCDFIso )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoBstautauCDFIso::RelInfoBstautauCDFIso( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent) : GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);

  setProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty(    "PVInputLocation"
                      , m_PVInputLocation = LHCb::RecVertexLocation::Primary
                      , " PV input location"
                      );

  //sort these keys out by adding all
  m_keys.clear();
  /*    std::vector<std::string>::const_iterator ivar;
        for (ivar = m_variables.begin(); ivar != m_variables.end(); ivar++) {
        short int key = RelatedInfoNamed::indexByName( *ivar );
        if (key != RelatedInfoNamed::Unknown) {
        m_keys.push_back( key );
        } else {
        }
        }
  */
  m_keys.push_back( RelatedInfoNamed::BSTAUTAUCDFISO );

}

//=============================================================================
// Destructor
//=============================================================================
RelInfoBstautauCDFIso::~RelInfoBstautauCDFIso() {}


//=============================================================================
// Initialize
//=============================================================================
StatusCode RelInfoBstautauCDFIso::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  return sc;
}

//=============================================================================
// Fill Extra Info structure
//=============================================================================
StatusCode RelInfoBstautauCDFIso::calculateRelatedInfo( const LHCb::Particle *top,
                                                        const LHCb::Particle *part )
{

  m_decayParticles.clear();
  m_bdt1 = 0.;

  if (  part->isBasicParticle() ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Running tau isolation on non-final state particle, skipping" << endmsg;
    return StatusCode::SUCCESS ;
  }
  LHCb::Particles*  parts = get<LHCb::Particles>("/Event/Phys/StdAllNoPIDsPions/Particles");
  if (!parts) {
    Error( " Failed to get particles container " );
    return StatusCode::SUCCESS;
  }

  saveDecayParticles(top);

  double pt_bs = top->momentum().rho(); //pt();
  double iso_giampi = 0.0;

  for(LHCb::Particles::const_iterator ipp=parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {

        Gaudi::XYZVector ptrack =  ((*ipp)->proto()->track()->momentum());
        //double ptrack = atrack->p(); //
        double pttrack = ptrack.rho(); //   (*ipp)->pt();  //
        if (isTrackInDecay(atrack)) continue;

        double deta      =  top->momentum().Eta() - (*ipp)->momentum().Eta() ;
        double delta_phi =  top->momentum().Phi() - (*ipp)->momentum().Phi();
        delta_phi = TMath::Abs(delta_phi);

        if (delta_phi > TMath::Pi() )  delta_phi = 2*TMath::Pi() - delta_phi;

        double rad_cone = TMath::Sqrt(  (delta_phi*delta_phi + deta*deta) );
        if (  rad_cone <=1.0)
        {
          iso_giampi += pttrack;
        }
      } // atrack
    } //proto
  } // ipp

  m_bdt1 = pt_bs/(iso_giampi+pt_bs);
   if (msgLevel(MSG::DEBUG)) debug()<<"Tool7*** - m_bdt1 : "<<m_bdt1<<"  , pt_bs/(iso_giampi+pt_bs) : "<<pt_bs<<"/"<<iso_giampi+pt_bs<<endmsg;
  
  m_map.clear();

  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {

    float value = 0.0;
    switch (*ikey) {
    case RelatedInfoNamed::BSTAUTAUCDFISO : value = m_bdt1; break;
    }
    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;

    m_map.insert( std::make_pair( *ikey, value) );
  }
  bool test = true;
  return StatusCode(test);
}


//rel infor methods

LHCb::RelatedInfoMap* RelInfoBstautauCDFIso::getInfo(void) {
  return &m_map;
}

std::string RelInfoBstautauCDFIso::infoPath(void) {
  std::stringstream ss;
  ss << std::string("Particle2CDFIsoRelations");
  return ss.str();
}

//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoBstautauCDFIso::saveDecayParticles( const LHCb::Particle *top)
{

  // -- Get the daughters of the top particle
  const SmartRefVector< LHCb::Particle > & daughters = top->daughters();

  // -- Fill all the daugthers in m_decayParticles
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){

    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() ){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      m_decayParticles.push_back( (*idau) );
    }else{
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( (*idau) );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      saveDecayParticles( (*idau) );
    }

  }

}

//=============================================================================
// Check if the track is already in the decay
//=============================================================================
bool RelInfoBstautauCDFIso::isTrackInDecay(const LHCb::Track* track){

  bool isInDecay = false;

  for(  std::vector<const LHCb::Particle*>::iterator it = m_decayParticles.begin() ; it != m_decayParticles.end() ; ++it ){

    const LHCb::ProtoParticle* proto = (*it)->proto();
    if(proto){
      const LHCb::Track* myTrack = proto->track();

      if(myTrack){

        if(myTrack == track){
          if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }
      }
    }
  }
  return isInDecay;
}

