
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
  else if ( "CombDlleSpike"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 13; }
  else if ( "CombDllmuSpike"         == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 14; }
  else if ( "CaloBremMatchSpike"     == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 15; }
  else if ( "BremPIDeSpike"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 16; }
  else if ( "PrsPIDeSpike"           == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 17; }
  else if ( "HcalPIDeSpike"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 18; }
  else if ( "EcalPIDeSpike"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 19; }
  else if ( "CaloElectronMatchSpike" == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 20; }
  else if ( "CaloEcalChi2Spike"      == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 21; }
  else if ( "CaloClusChi2Spike"      == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 22; }
  else if ( "CaloPrsESpike"          == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 23; }
  else if ( "CaloBremChi2Spike"      == name ) { id = (int)LHCb::ProtoParticle::LastGlobal + 24; }
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
    const double MaxP = 1000.0 * Gaudi::Units::GeV;
    var = proto->track()->p();
    if ( var > MaxP ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::TrackPt == input )
  {
    const double MaxPt = 200.0 * Gaudi::Units::GeV;
    var = proto->track()->pt();
    if ( var > MaxPt ) var = -999;
  }

  // -----------------------------------------------------------------------------------------------
  // Tracking variables
  // -----------------------------------------------------------------------------------------------
  else if ( (int)LHCb::ProtoParticle::LastGlobal+11 == input )
  {
    var = proto->track()->likelihood();
    if ( var < -150.0 ) var = -999.0;
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+12 == input )
  {
    var = proto->track()->ghostProbability();
    if ( var > 0.999 ) var = -999.0; // Move delta at 1 to -999 for NeuroBayes
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
    if      ( var < -100   ) { var = -999; }
    else if ( var > 9999.9 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloBremChi2 == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloBremChi2,-999);
    if      ( var < -100   ) { var = -999; }
    else if ( var > 9999.9 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloClusChi2 == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloClusChi2,-999);
    if      ( var < -100  ) { var = -999; }
    else if ( var > 999.9 ) { var = -999; }
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
  // CALO Variables with spikes
  // -----------------------------------------------------------------------------------------------
  else if ( (int)LHCb::ProtoParticle::CombDLLe == input )
  {
    var = proto->info(LHCb::ProtoParticle::CombDLLe,-999);
    if ( (var > -0.535  && var < -0.532)  ||
         (var > -0.740  && var < -0.739)  ||
         (var > -0.72   && var < -0.7195) ||
         (var > -0.728  && var < -0.727)  ||
         (var > -0.767  && var < -0.766)  ||
         (var > -0.789  && var < -0.788)  ||
         (var > -0.795  && var < -0.794)  ||
         (var > -0.555  && var < -0.552)  ||
         (var > -0.377  && var < -0.375)  ||
         (var > -0.608  && var < -0.607)  ||
         (var > -0.6392 && var < -0.6388) ||
         (var > -2.222  && var < -2.223)  ||
         (var > -2.314  && var < -2.314)  ||
         (var > -2.335  && var < -2.334)  )
    {
      var = -999;
    }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+13 == input )
  {
    const double CombDlle = proto->info(LHCb::ProtoParticle::CombDLLe,0);
    double & CombDlleSpike = var;
    if      (CombDlle > -0.01 && CombDlle < 0.01)
    { CombDlleSpike = 1; }
    else if (CombDlle > -0.535 && CombDlle < -0.532)
    { CombDlleSpike = 2; }
    else if (CombDlle > -0.740 && CombDlle < -0.739)
    { CombDlleSpike = 3; }
    else if  (CombDlle > -0.72  && CombDlle < -0.7195)
    { CombDlleSpike = 4; }
    else if (CombDlle > -0.728 && CombDlle < -0.727)
    { CombDlleSpike = 5; }
    else if (CombDlle > -0.767 && CombDlle < -0.766)
    { CombDlleSpike = 6; }
    else if (CombDlle > -0.789 && CombDlle < -0.788)
    { CombDlleSpike = 7; }
    else if (CombDlle > -0.795 && CombDlle < -0.794)
    { CombDlleSpike = 8; }
    else if (CombDlle > -0.555 && CombDlle < -0.552)
    { CombDlleSpike = 9; }
    else if (CombDlle > -0.377  && CombDlle < -0.375)
    { CombDlleSpike = 10; }
    else if (CombDlle > -0.608  && CombDlle < -0.607)
    { CombDlleSpike = 11; }
    else if (CombDlle > -0.6392 && CombDlle < -0.6388)
    { CombDlleSpike = 12; }
    else if (CombDlle > -2.222  && CombDlle < -2.223)
    { CombDlleSpike = 13; }
    else if (CombDlle > -2.314  && CombDlle < -2.314)
    { CombDlleSpike = 14; }
    else if (CombDlle > -2.335  && CombDlle < -2.334)
    { CombDlleSpike = 15; }
  }
  else if ( (int)LHCb::ProtoParticle::CombDLLmu == input )
  {
    var = proto->info(LHCb::ProtoParticle::CombDLLmu,-999);
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+14 == input )
  {
    double & CombDllmuSpike = var;
    const double CombDllmu = proto->info(LHCb::ProtoParticle::CombDLLmu,0);
    if      (CombDllmu > -0.1    && CombDllmu < 0.1)
    { CombDllmuSpike = 1; }
    else if (CombDllmu > -7.0001 && CombDllmu < 6.9999 )
    { CombDllmuSpike = 2; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloBremMatch == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloBremMatch,-999);
    if ( var < 0.001 ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+15 == input )
  {
    const double CaloBremMatch = proto->info(LHCb::ProtoParticle::CaloBremMatch,0);
    double & CaloBremMatchSpike = var;
    if (CaloBremMatch > -1.0 && CaloBremMatch < 0.001)
    { CaloBremMatchSpike = 1.0; }
    else if (CaloBremMatch < -1 )
    { CaloBremMatchSpike = 2.0; }
  }
  else if ( (int)LHCb::ProtoParticle::BremPIDe == input )
  {
    var = proto->info(LHCb::ProtoParticle::BremPIDe,-999);
    if ( (var > -0.534  &&  var < -0.532)  ||
         (var > -0.543  &&  var < -0.542)  ||
         (var > -0.549  &&  var < -0.548)  ||
         (var > -0.554  &&  var < -0.553)  ||
         (var > -0.608  &&  var < -0.607)  ||
         (var > -0.620  &&  var < -0.618)  ||
         (var > -0.622  &&  var < -0.620)  ||
         (var > -0.626  &&  var < -0.624)  ||
         (var > -0.640  &&  var < -0.638)  )
    {
      var = -999;
    }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+16 == input )
  {
    const double BremPIDe = proto->info(LHCb::ProtoParticle::BremPIDe,0);
    double & BremPIDeSpike = var;
    if      (BremPIDe >  -0.01 &&  BremPIDe < 0.01)
    { BremPIDeSpike = 1; }
    else if (BremPIDe >  -0.534 &&  BremPIDe < -0.532)
    { BremPIDeSpike = 2; }
    else if  (BremPIDe >  -0.543  &&  BremPIDe < -0.542)
    { BremPIDeSpike = 3; }
    else if  (BremPIDe >  -0.549  &&  BremPIDe < -0.548)
    { BremPIDeSpike = 4; }
    else if (BremPIDe >  -0.554  &&  BremPIDe < -0.553)
    { BremPIDeSpike = 5; }
    else if (BremPIDe >  -0.608  &&  BremPIDe < -0.607)
    { BremPIDeSpike = 6; }
    else if (BremPIDe >  -0.620  &&  BremPIDe < -0.618)
    { BremPIDeSpike = 7; }
    else if  (BremPIDe >  -0.622  &&  BremPIDe < -0.620)
    { BremPIDeSpike = 8; }
    else if (BremPIDe >  -0.626  &&  BremPIDe < -0.624)
    { BremPIDeSpike = 9; }
    else if (BremPIDe >  -0.640  &&  BremPIDe < -0.638)
    { BremPIDeSpike = 10; }
  }
  else if ( (int)LHCb::ProtoParticle::PrsPIDe == input )
  {
    var = proto->info(LHCb::ProtoParticle::PrsPIDe,-999);
    if ( var > -0.534 && var < -0.532 ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+17 == input )
  {
    const double PrsPIDe = proto->info(LHCb::ProtoParticle::PrsPIDe,0);
    double & PrsPIDeSpike  = var;
    if      (PrsPIDe >  -0.01 &&  PrsPIDe < 0.01)
    {  PrsPIDeSpike = 1; }
    else if (PrsPIDe >  -0.534 &&  PrsPIDe < -0.532)
    {  PrsPIDeSpike = 2; }
  }
  else if ( (int)LHCb::ProtoParticle::HcalPIDe == input )
  {
    var = proto->info(LHCb::ProtoParticle::HcalPIDe,0-999);
    if ( (var >   0.171 && var < 0.173) ||
         (var >   0.376 && var < 0.378) ||
         (var >   0.451 && var < 0.453) ||
         (var >   0.484 && var < 0.486) ||
         (var >   0.684 && var < 0.686) ||
         (var >   0.835 && var < 0.837) )
    {
      var = -999;
    }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+18 == input )
  {
    const double HcalPIDe = proto->info(LHCb::ProtoParticle::HcalPIDe,0);
    double & HcalPIDeSpike = var;
    if      (HcalPIDe >  -0.01 && HcalPIDe < 0.01)
    { HcalPIDeSpike = 1; }
    else if (HcalPIDe >   0.171 && HcalPIDe < 0.173)
    { HcalPIDeSpike = 2; }
    else if (HcalPIDe >   0.484 && HcalPIDe < 0.486)
    { HcalPIDeSpike = 3; }
    else if (HcalPIDe >   0.684 && HcalPIDe < 0.686)
    { HcalPIDeSpike = 4; }
    else if (HcalPIDe >   0.835 && HcalPIDe < 0.837)
    { HcalPIDeSpike = 5; }
    else if (HcalPIDe >   0.376 && HcalPIDe < 0.378)
    { HcalPIDeSpike = 6; }
    else if (HcalPIDe >   0.451 && HcalPIDe < 0.453)
    { HcalPIDeSpike = 7; }
  }
  else if ( (int)LHCb::ProtoParticle::EcalPIDe == input )
  {
    var = proto->info(LHCb::ProtoParticle::EcalPIDe,-999);
    if ( (var >  -3.707 && var < -3.706) ||
         (var >  -3.560 && var <  3.371) ||
         (var >  -2.974 && var < -2.972) ||
         (var >  -3.571 && var < -3.569) )
    {
      var = -999;
    }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+19 == input )
  {
    const double EcalPIDe = proto->info(LHCb::ProtoParticle::EcalPIDe,0);
    double & EcalPIDeSpike = var;
    if (EcalPIDe >  -0.01  && EcalPIDe <  0.01)
    { EcalPIDeSpike = 1; }
    else if (EcalPIDe >  -3.707 && EcalPIDe < -3.706)
    { EcalPIDeSpike = 2; }
    else if (EcalPIDe >  -3.560 && EcalPIDe <  3.371)
    { EcalPIDeSpike = 3; }
    else if (EcalPIDe >  -2.974 && EcalPIDe < -2.972 )
    { EcalPIDeSpike = 4; }
    else if (EcalPIDe >  -3.571 && EcalPIDe < -3.569)
    { EcalPIDeSpike = 5; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloElectronMatch == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloElectronMatch,-999);
    if ( var < 0.001 ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::CaloTrMatch == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloTrMatch,-999);
    if ( var < 0.001 ) var = -999;
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+20 == input )
  {
    const double CaloElectronMatch = proto->info(LHCb::ProtoParticle::CaloElectronMatch,0);
    if      (CaloElectronMatch > -1.0 && CaloElectronMatch < 0.001)
    { var = 1.0; }
    else if (CaloElectronMatch < -1 )
    { var = 2.0; }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+21 == input )
  {
    const double tmp = proto->info(LHCb::ProtoParticle::CaloEcalChi2,-999);
    var = 0;
    if      ( tmp < -100   ) { var = 1.0; }
    else if ( tmp > 9999.9 ) { var = 2.0; }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+22 == input )
  {
    const double tmp = proto->info(LHCb::ProtoParticle::CaloClusChi2,-999);
    var = 0.0;
    if      ( tmp < 0.001 ) { var = 1.0; }
    else if ( tmp > 999.9 ) { var = 2.0; }
  }
  else if ( (int)LHCb::ProtoParticle::CaloPrsE == input )
  {
    var = proto->info(LHCb::ProtoParticle::CaloPrsE,-999);
    if ( var > 260 ) { var = -999; }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+24 == input )
  {
    const double tmp = proto->info(LHCb::ProtoParticle::CaloBremChi2,-999);
    var = 0.0;
    if      ( tmp < 0.001  ) { var = 1.0; }
    else if ( tmp > 9999.9 ) { var = 2.0; }
  }
  else if ( (int)LHCb::ProtoParticle::LastGlobal+23 == input )
  {
    const double tmp = proto->info(LHCb::ProtoParticle::CaloPrsE,-999);
    var = 0.0;
    if      ( tmp < 0.001 ) { var = 1.0; }
    else if ( tmp > 260.0 ) { var = 2.0; }
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
