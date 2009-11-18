// ============================================================================
// Include files
#include <stdio.h>
#include <string.h>
//From Kernel/LHCbDefintions
/////#include "Kernel/Point3DTypes.h"
#include "Kernel/ParticleID.h"
#include "Kernel/RichParticleIDType.h"
// #include "RichKernel/RichParticleIDType.h"
// Event
#include "Event/RecHeader.h"
#include "Event/Particle.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
// #include "Event/MuonID.h"
// GaudiKernel
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
// Inteerfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// *******************
// *** local *********
// *******************
#include "ElecPresel.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ElecPresel
//-----------------------------------------------------------------------------
// ****************************************************
// ****** Declaration of the Algorithm Factory ********
// ****************************************************
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,ElecPresel );
/////static const  AlgFactory<ElecPresel>          s_factory ;
/////const        IAlgFactory& ElecPreselFactory = s_factory ;



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Kali::ElecPresel::ElecPresel( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator),
    m_Extra()
{
  declareProperty( "EcalIsolation",  m_ECALisolation = 300.);
//  declareProperty( "ProtoSource",  m_Input = ParticleLocation::Charged);
// Locations in DET
  declareProperty( "SpdLocation",  
    m_Calos[ElecPreselDets::Spd]   = DeCalorimeterLocation::Spd);
  declareProperty( "PrsLocation",  
    m_Calos[ElecPreselDets::Prs]   = DeCalorimeterLocation::Prs);
  declareProperty( "EcalLocation",  
    m_Calos[ElecPreselDets::Ecal]  = DeCalorimeterLocation::Ecal);
  declareProperty( "HcalLocation",  
    m_Calos[ElecPreselDets::Hcal]  = DeCalorimeterLocation::Hcal);
// Locations in RAW
  declareProperty( "SpdLocation",  
    m_Digits[ElecPreselDets::Spd]   = LHCb::CaloDigitLocation::Spd);
  declareProperty( "PrsLocation",  
    m_Digits[ElecPreselDets::Prs]   = LHCb::CaloDigitLocation::Prs);
  declareProperty( "EcalLocation",  
    m_Digits[ElecPreselDets::Ecal]  = LHCb::CaloDigitLocation::Ecal);
  declareProperty( "HcalLocation",  
    m_Digits[ElecPreselDets::Hcal]  = LHCb::CaloDigitLocation::Hcal);
}


//=============================================================================
// Destructor
//=============================================================================
Kali::ElecPresel::~ElecPresel() {};


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Kali::ElecPresel::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
// Retrieve geometry info
  for (int i=0; i<4; i++) {
    m_Calo[i]  = getDet<DeCalorimeter> ( m_Calos[i] );
    const IGeometryInfo *geometry = m_Calo[i]->geometry();
    Gaudi::XYZPoint local(0., 0., m_Calo[i]->zShowerMax());
    Gaudi::XYZPoint global = geometry->toGlobal(local);
    m_zShowerMax[i] = global.Z();
// Get other parameters
    double size = m_Calo[i]->zSize();
    double offs = m_Calo[i]->zOffset();
    local.SetZ(offs-size/2.);
    global = geometry->toGlobal(local);
    zShowerLength[i] = m_zShowerMax[i]-global.Z();
  }
// Set Track Extrapolator
  m_Extra = tool<ITrackExtrapolator>("TrackFastParabolicExtrapolator",this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Kali::ElecPresel::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  MsgStream  log( msgSvc(), name() );

  setFilterPassed(false);

  // ********************************************************************
  // ***** Retrieve informations about event ****************************
  // ********************************************************************

  // RecHeader *header = get<RecHeader>( RecHeaderLocation::Default );
  // if (!header) {
  //  log << MSG::ERROR << "    not able to retrieve event" << endreq;
  //  return StatusCode::FAILURE;
  //}
  
// Retrieve Particles 
  const  LHCb::Particle::ConstVector&  parts  = desktop()->particles();
//  ProtoParticles *protos = get<ProtoParticles>( m_Input );

  if (parts.size() == 0) return sc;
// Start selection
  m_NAllTracks = 0;  
  for (LHCb::Particle::ConstVector::const_iterator iAll = parts.begin();
    iAll != parts.end(); iAll++) {
    if ((*iAll)->charge() == 0) continue;
    const LHCb::ProtoParticle* proto= (*iAll)->proto();
    if (!proto) continue;
    const Track *track = proto->track();
    if (track) {  // Track really exists
      State state = track->closestState (m_zShowerMax[2]);
      if (state.z() < 7500.) continue;
      PropagateToZ (proto, m_zShowerMax[2]-zShowerLength[2], 
                     &m_XYEcal[0][m_NAllTracks],&m_XYEcal[1][m_NAllTracks]);
      if (fabs(m_XYEcal[0][m_NAllTracks]) > 3878. ||
	fabs(m_XYEcal[1][m_NAllTracks]) > 3151.) continue;
      m_AllParts[m_NAllTracks] = *iAll;
      m_AllProto[m_NAllTracks] = proto;
      m_Mask[m_NAllTracks++] = 1;
    }
  }
  
  
  if (!m_NAllTracks) return sc;
// Get pointers on Digits
  for (int i=0; i<4; i++) {
    m_RawDigits[i] = get<LHCb::CaloDigits>(m_Digits[i]);
  }
// Check isolation and electron ID
  m_NETracks = 0;
  for (int i=0; i<m_NAllTracks-1; i++) {
    if (m_Mask[i]) {
      int j=i+1;
      int isol = 1;
      double closest = 1000000000.;
//      while (isol && j<m_NAllTracks) {
      while (j<m_NAllTracks) {
        double dist =
        (m_XYEcal[0][i]-m_XYEcal[0][j])*(m_XYEcal[0][i]-m_XYEcal[0][j])+
        (m_XYEcal[1][i]-m_XYEcal[1][j])*(m_XYEcal[1][i]-m_XYEcal[1][j]);
        if (dist < closest) closest = dist;
        if (dist < m_ECALisolation*m_ECALisolation) {
	  isol = 0;
	  m_Mask[i] = 0;
	  m_Mask[j] = 0;
        }
        j++;
      }
      if (isol) {   // Save this track if it matches electron ID
// Check track momentum
        const Track *track = m_AllProto[i]->track();
        State state = track->closestState (m_zShowerMax[2]);
        if (fabs(state.p()) > 4000.) {
// Check VELO info
//          float vcharge = m_AllProto[i]->info(ProtoParticle::VeloCharge,-100.);
// Get RICH ID
          const LHCb::RichPID *rpid = m_AllProto[i]->richPID();
          int ele1=(rpid && (rpid->bestParticleID() == Rich::Electron)) ? 1 : 0;
	  
//          if ( vcharge > 1.7 || ele1 == 1) {
          if ( ele1 == 1) {
            if (!CheckHCal (m_AllProto[i])) continue;
            m_AllProto[m_NETracks]   = m_AllProto[i];
            m_AllParts[m_NETracks++] = m_AllParts[i];
// Save in desktop
            desktop()->keep(m_AllParts[i]);
          }
        }
      }
    }
  }
  if (m_NETracks) setFilterPassed(true);
  return sc;
}

//=============================================================================
// Propagate ProtoParticle to specified Z
//=============================================================================
void Kali::ElecPresel::PropagateToZ(const ProtoParticle *proto, double z, 
                                                   double *x, double *y) {
  
  const Track *track = proto->track();
  State state = track->closestState (z);
//  fprintf (stdout,"Where: %f, What: %f %f\n",z,state
  m_Extra->propagate (state,z,LHCb::ParticleID(211*(proto->charge())));
  *x = state.x();
  *y = state.y();
  if (state.z() < z - 10.) {
    *x = -100000.;
    *y = -100000.;
  }
}

//=============================================================================
// HCAL
//=============================================================================
bool Kali::ElecPresel::CheckHCal (const ProtoParticle *proto) {
  
  float ehcal = 0.;
  int iCharge = proto->charge();
// Propagate to HCAL
   const Track *track = proto->track();
   State Pstate = track->closestState (m_zShowerMax[3]);
   float zpos = m_zShowerMax[3];
   zpos += zShowerLength[3]*(Pstate.momentum().Z()/Pstate.p() - 1.);
   m_Extra->propagate (Pstate,zpos,LHCb::ParticleID(211*iCharge));
   Gaudi::XYZPoint mpos(Pstate.x(), Pstate.y(), zpos);
   CaloNeighbors near = m_Calo[3]->neighborCells (m_Calo[3]->Cell(mpos));
   near.push_back (m_Calo[3]->Cell(mpos));
   for (CaloNeighbors::const_iterator iN = near.begin(); near.end() != iN; iN++) {
     const LHCb::CaloDigit *dig = m_RawDigits[3]->object( *iN );
     if (dig != 0) {
       if (dig->e() > 0.) ehcal += dig->e();
       if (ehcal > 1000.) return false;
     }
   }
   return true;
}
  
//=============================================================================
//  Finalize
//=============================================================================
StatusCode Kali::ElecPresel::finalize() {

////////  if (m_Linker) delete m_Linker;
  return DVAlgorithm::finalize();
}
