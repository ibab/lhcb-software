// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
// from LHCb
#include "CaloUtils/CaloMomentum.h"
#include "GaudiKernel/Point3DTypes.h"
//#include "Event/CaloCellID.h"
// local
#include "BremAdder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BremAdder
//
// 2006-10-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BremAdder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  BremAdder::BremAdder( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
    : GaudiTool ( type, name , parent )
    ,m_dllBrem()
    ,m_calo(NULL)
{
  declareInterface<IBremAdder>(this);
  declareProperty("BremCor"     , m_bremCor  = 1.0     ); // Should be 1 with photon correction
  declareProperty("BremDllCut"  , m_dllBrem  = -999999.); // No cut
  declareProperty("BremChi2Cut" , m_chi2Brem = 300.);
}
//=============================================================================
// Destructor
//=============================================================================
BremAdder::~BremAdder() {}

//=============================================================================
StatusCode BremAdder::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  return StatusCode::SUCCESS;
}
//=============================================================================
bool BremAdder::brem4particle( LHCb::Particle* particle, std::string what ) const
{
  int sign = 0;
  if( "add"    == what ) sign = +1;
  if( "remove" == what ) sign = -1;

  debug() << " --- Brem adding/removal (" << sign << ") to/from particle momentum " << particle->momentum() << endreq ;

  const LHCb::ProtoParticle* proto = particle ->proto();
  if( NULL == proto) {
    debug() << " The particle is not basic - no brem to be added" << endreq;
    return false;
  }
  if( NULL == proto->track() ){
    debug() << " No charged track associated to the particle - no brem to be added" << endreq;
    return false;
  }


  // check if bremmStrahlung has already been added
  const LHCb::State  state = proto->track()->firstState();
  bool bremIsAdded = false;
  double eps = 1E-4;
  if( fabs( particle->momentum().P() - state.p()) > eps ) bremIsAdded = true ;
  //
  if( bremIsAdded  && "add" == what ){
    debug() << " The bremstrahlung has already been added - nothing to add"<< endreq;
    return false;
  }
  if( !bremIsAdded && "remove" == what ){
    debug() << " No bremstrahlung had been added - nothing to remove "<< endreq;
    return false;
  }
  //

  const SmartRefVector<LHCb::CaloHypo> hypos = proto->calo();
  if( 0 == hypos.size() ){
    debug() << " No caloHypo associated to the particle - nothing to add "<< endreq;
    return false;
  }


  LHCb::CaloHypo* electronHypo = NULL;
  LHCb::CaloHypo* photonHypo   = NULL;
  for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihyp = hypos.begin();ihyp!=hypos.end();++ihyp){
    const LHCb::CaloHypo* hypo = *ihyp;
    if (NULL != hypo){
      if( hypo->hypothesis() == LHCb::CaloHypo::Photon    ) photonHypo   = (LHCb::CaloHypo*) hypo;
      if( hypo->hypothesis() == LHCb::CaloHypo::EmCharged ) electronHypo = (LHCb::CaloHypo*) hypo;
      debug() << " Hypo " << hypo->hypothesis() << " " << hypo << endreq;
    }
  }

  if( NULL == photonHypo){
    debug() << " No photon associated to the particle - nothing to add "<< hypos.size() <<endreq;
    return false;
  }

  // Reject if the brem candidate is also the electron cluster candidate

  if( NULL != electronHypo){
    LHCb::CaloPosition* photonPos   = photonHypo->position();
    LHCb::CaloPosition* electronPos = electronHypo->position();
    if(NULL != photonPos && NULL != electronPos){
      const Gaudi::XYZPoint photonPoint( photonPos->x(),photonPos->y(),photonPos->z() );
      const Gaudi::XYZPoint electronPoint(electronPos->x(),electronPos->y(),electronPos->z());
      const LHCb::CaloCellID photonCell   = m_calo->Cell( photonPoint );
      const LHCb::CaloCellID electronCell = m_calo->Cell( electronPoint );
      debug() << "Points : " << photonPoint << " / " << electronPoint <<endreq;
      debug() << "Cells : " << photonCell << " / " << electronCell <<endreq;
      if(photonCell == electronCell) {
        debug() << " Electron and Brem candidates are the same calo - nothing to add "<< endreq;
        return false;
      }
    }
  }

  // Add here the criteria for the identified  brem candidate to be actually added
  if( "add" == what ){
    if( proto->info(LHCb::ProtoParticle::InAccBrem,0.) == 0.)return false; // no Brem in Acceptance
    if( m_dllBrem  > proto->info(LHCb::ProtoParticle::BremPIDe,-9999. ) )return false ;
    if( m_chi2Brem < proto->info(LHCb::ProtoParticle::CaloBremMatch,9999. ) )return false ;
    // ...
  }


  // Add/remove brem
  LHCb::CaloMomentum bremPhoton( photonHypo ,particle->referencePoint(), particle->posCovMatrix() );
  // neglect effect from e-mass : Etot = Ebrem + Ee
  (Gaudi::LorentzVector&)particle->momentum()    += sign*m_bremCor*bremPhoton.momentum();
  (Gaudi::SymMatrix4x4&)particle->momCovMatrix() += sign*m_bremCor*bremPhoton.momCovMatrix();
  (Gaudi::Matrix4x3&)particle->posMomCovMatrix() += sign*m_bremCor*bremPhoton.momPointCovMatrix();

  debug() << "Brem momentum : "<< bremPhoton.momentum() << endreq;

  debug() << "Particle Momentum after Brem correction " << particle->momentum()<< endreq;

  return true;
}
