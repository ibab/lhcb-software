
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDBase.cpp
 *
 * Implementation file for algorithm ChargedProtoANNPIDBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoANNPIDBase.h"

//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDBase::ChargedProtoANNPIDBase( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : GaudiTupleAlg ( name , pSvcLocator )
{
  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPath = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_protoPath = LHCb::ProtoParticleLocation::Charged;
  }
  // Job Options
  declareProperty( "ProtoParticleLocation", m_protoPath );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDBase::~ChargedProtoANNPIDBase() { }

//=============================================================================

int ChargedProtoANNPIDBase::variableID( const std::string & name ) const
{
  int id(0);
  // special cases
  if      ( "RichUsedAero"           == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 1; }
  else if ( "RichUsedR1Gas"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 2; }
  else if ( "RichUsedR2Gas"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 3; }
  else if ( "RichAboveElThres"       == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 4; }
  else if ( "RichAboveMuThres"       == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 5; }
  else if ( "RichAbovePiThres"       == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 6; }
  else if ( "RichAboveKaThres"       == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 7; }
  else if ( "RichAbovePrThres"       == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 8; }
  else if ( "MuonIsLooseMuon"        == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 9; }
  else if ( "MuonIsMuon"             == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 10; }
  else if ( "TrackLikelihood"        == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 11; }
  else if ( "TrackGhostProbability"  == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 12; }
  else if ( "NumProtoParticles"      == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 13; }
  else if ( "NumCaloHypos"           == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 14; }
  // Default, use enum from ProtoParticle
  else                                  { id = (int)LHCb::ProtoParticle::convertExtraInfo(name); }
  return id;
}

//=============================================================================

ChargedProtoANNPIDBase::IntInputs
ChargedProtoANNPIDBase::variableIDs( const StringInputs & names ) const
{
  IntInputs inputs;
  inputs.reserve(names.size());
  for ( StringInputs::const_iterator i = names.begin();
        i != names.end(); ++i )
  {
    inputs.push_back( variableID(*i) );
  }
  return inputs;
}

//=============================================================================

double ChargedProtoANNPIDBase::getInput( const LHCb::ProtoParticle * proto,
                                         const int input ) const
{
  double var(0);

  static LHCb::MuonPID tmpMPID;
  static LHCb::RichPID tmpRPID;

  // -----------------------------------------------------------------------------------------------
  // not in the ProtoParticles
  // -----------------------------------------------------------------------------------------------
  if ( (int)LHCb::ProtoParticle::TrackP == input )
  {
    const double MaxP = 5000.0 * Gaudi::Units::GeV;
    var = proto->track()->p();
    if ( var > MaxP ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::TrackPt == input )
  {
    const double MaxPt = 1000.0 * Gaudi::Units::GeV;
    var = proto->track()->pt();
    if ( var > MaxPt ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+13 == input )
  {
    const LHCb::ProtoParticles * protos = dynamic_cast<const LHCb::ProtoParticles *>(proto->parent());
    var = ( protos ? protos->size() : -999 );
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+14 == input )
  {
    var = (double)proto->calo().size();
  }

  // -----------------------------------------------------------------------------------------------
  // Tracking variables
  // -----------------------------------------------------------------------------------------------
  else if ( (int)LHCb::ProtoParticle::LastGlobal+11 == input )
  {
    var = proto->track()->likelihood();
    if ( var < -120.0 ) var = -999.0; // Strange distribution 
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+12 == input )
  {
    var = proto->track()->ghostProbability();
    if ( var > 0.99999 ) var = -999.0; // Move delta at 1 to -999 for NeuroBayes
  }

  // -----------------------------------------------------------------------------------------------
  // special case ( variables with specific default requirements, not -999 )
  // -----------------------------------------------------------------------------------------------
  else if ( (int)LHCb::ProtoParticle::InAccEcal == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccEcal,0);
  }
  else if ( (int)LHCb::ProtoParticle::InAccHcal == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccHcal,0);
  }
  else if ( (int)LHCb::ProtoParticle::InAccPrs == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccPrs,0);
  }
  else if ( (int)LHCb::ProtoParticle::InAccSpd == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccSpd,0);
  }
  else if ( (int)LHCb::ProtoParticle::InAccBrem == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccBrem,0);
  }
  else if ( (int)LHCb::ProtoParticle::CaloEcalChi2 == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloEcalChi2,-999);
    if      ( var < -100    ) { var = -999; }
    else if ( var > 9999.99 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloBremChi2 == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloBremChi2,-999);
    if      ( var < -100    ) { var = -999; }
    else if ( var > 9999.99 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloClusChi2 == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloClusChi2,-999);
    if      ( var < -100   ) { var = -999; }
    else if ( var > 999.99 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::InAccMuon == input )
  {
    var = proto->info(LHCb::ProtoParticle::InAccMuon,0);
  }
  else if ( (int)LHCb::ProtoParticle::MuonNShared == input )
  {
    var = proto->info(LHCb::ProtoParticle::MuonNShared,-1.0) + 1.0;
  }

  // -----------------------------------------------------------------------------------------------
  // special cases ( variables without a ProtoParticle entry )
  // -----------------------------------------------------------------------------------------------
  else if ( (int)LHCb::ProtoParticle::LastGlobal+1 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.usedAerogel();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+2 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.usedRich1Gas();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+3 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.usedRich2Gas();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+4 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.electronHypoAboveThres();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+5 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.muonHypoAboveThres();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+6 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.pionHypoAboveThres();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+7 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.kaonHypoAboveThres();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+8 == input )
  {
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    var = tmpRPID.protonHypoAboveThres();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+9 == input )
  {
    tmpMPID.setStatus( static_cast<int>(proto->info(LHCb::ProtoParticle::MuonPIDStatus,0)) );
    var = tmpMPID.IsMuonLoose();
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+10 == input )
  {
    tmpMPID.setStatus( static_cast<int>(proto->info(LHCb::ProtoParticle::MuonPIDStatus,0)) );
    var = tmpMPID.IsMuon();
  }

  // -----------------------------------------------------------------------------------------------
  // Finally the non special cases
  // -----------------------------------------------------------------------------------------------
  else
  {
    var = proto->info( (LHCb::ProtoParticle::additionalInfo)input, -999 );
  }

  // return the variable
  return var;
}
