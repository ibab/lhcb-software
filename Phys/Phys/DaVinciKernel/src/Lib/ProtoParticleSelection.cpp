
//-----------------------------------------------------------------------------
/** @file ProtoParticleSelection.cpp
 *
 * Implementation file for utility class ProtoParticleSelection
 *
 * CVS Log :-
 * $Id: ProtoParticleSelection.cpp,v 1.5 2006-11-20 15:52:51 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/ProtoParticleSelection.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleSelection
//
// 2006-05-03 : Christopher Rob Jones
//-----------------------------------------------------------------------------
//=============================================================================
// Destructor
//=============================================================================
ProtoParticleSelection::~ProtoParticleSelection()
{
  // clean up
  for ( DetectorRequirements::Vector::iterator iR = m_detectors.begin();
        iR != m_detectors.end(); ++iR ) { delete *iR; }
  for ( Cut::Vector::iterator iC = m_cuts.begin();
        iC != m_cuts.end(); ++iC )      { delete *iC; }
}

ProtoParticleSelection::Cut * ProtoParticleSelection::DLLCut::clone() const
{
  return new DLLCut(*this);
}

ProtoParticleSelection::Cut * ProtoParticleSelection::SingleVariableCut::clone() const
{
  return new SingleVariableCut(*this);
}

ProtoParticleSelection::DetectorRequirements *
ProtoParticleSelection::DetectorRequirements::clone() const
{
  return new DetectorRequirements(*this);
}

bool
ProtoParticleSelection::SingleVariableCut::isSatisfied( const LHCb::ProtoParticle * proto ) const
{
  ProtoParticle::ExtraInfo::const_iterator info = proto->extraInfo().find( variable() );
  return ( info == proto->extraInfo().end() ? false : testCut( info->second, cutValue() ) );
}

bool
ProtoParticleSelection::DLLCut::isSatisfied( const LHCb::ProtoParticle * proto ) const
{
  ProtoParticle::ExtraInfo::const_iterator info1 = proto->extraInfo().find( dll1() );
  if ( info1 == proto->extraInfo().end() ) return false;
  ProtoParticle::ExtraInfo::const_iterator info2 = proto->extraInfo().find( dll2() );
  if ( info2 == proto->extraInfo().end() ) return false;
  return testCut( info1->second - info2->second, cutValue() );
}

bool
ProtoParticleSelection::DetectorRequirements::isSatisfied( const LHCb::ProtoParticle * proto ) const
{
  bool detOK = true;

  if ( requirement() == MustHave )
  {
    if ( (detector() == RICH && !hasRichDLL(proto)) ||
         (detector() == MUON && !hasMuonDLL(proto)) ||
         (detector() == CALO && !hasCaloDLL(proto)) ||
         (detector() == RICH_AEROGEL  && !hasRichAerogel(proto)) ||
         (detector() == RICH_RICH1GAS && !hasRich1Gas(proto))    ||
         (detector() == RICH_RICH2GAS && !hasRich2Gas(proto))    ||
         (detector() == CALO_SPD      && !hasCaloSPD(proto))     ||
         (detector() == CALO_PRS      && !hasCaloPRS(proto))     ||
         (detector() == CALO_ECAL     && !hasCaloECAL(proto))    ||
         (detector() == CALO_HCAL     && !hasCaloHCAL(proto))    ||
         (detector() == CALO_BREM     && !hasCaloBREM(proto))
         )
    {
      detOK = false;
    }
  }
  else if ( requirement() == OnlyHave )
  {
    if ( (detector() == RICH && (!hasRichDLL(proto) || hasMuonDLL(proto) || hasCaloDLL(proto))) ||
         (detector() == MUON && (!hasMuonDLL(proto) || hasRichDLL(proto) || hasCaloDLL(proto))) ||
         (detector() == CALO && (!hasCaloDLL(proto) || hasRichDLL(proto) || hasMuonDLL(proto))) ||
         (detector() == RICH_AEROGEL  && (!hasRichAerogel(proto) || hasMuonDLL(proto) || hasCaloDLL(proto))) ||
         (detector() == RICH_RICH1GAS && (!hasRich1Gas(proto) || hasMuonDLL(proto) || hasCaloDLL(proto)))    ||
         (detector() == RICH_RICH2GAS && (!hasRich2Gas(proto) || hasMuonDLL(proto) || hasCaloDLL(proto))) ||
         (detector() == CALO_SPD      && (!hasCaloSPD(proto)  || hasRichDLL(proto) || hasMuonDLL(proto))) ||
         (detector() == CALO_PRS      && (!hasCaloPRS(proto)  || hasRichDLL(proto) || hasMuonDLL(proto))) ||
         (detector() == CALO_ECAL     && (!hasCaloECAL(proto) || hasRichDLL(proto) || hasMuonDLL(proto))) ||
         (detector() == CALO_HCAL     && (!hasCaloHCAL(proto) || hasRichDLL(proto) || hasMuonDLL(proto))) ||
         (detector() == CALO_BREM     && (!hasCaloBREM(proto) || hasRichDLL(proto) || hasMuonDLL(proto)))
         )
    {
      detOK = false;
    }
  }
  else if ( requirement() == MustNotHave )
  {
    if ( (detector() == RICH && hasRichDLL(proto)) ||
         (detector() == MUON && hasMuonDLL(proto)) ||
         (detector() == CALO && hasCaloDLL(proto)) ||
         (detector() == RICH_AEROGEL  && hasRichAerogel(proto)) ||
         (detector() == RICH_RICH1GAS && hasRich1Gas(proto))    ||
         (detector() == RICH_RICH2GAS && hasRich2Gas(proto))    ||
         (detector() == CALO_SPD      && hasCaloSPD(proto))    ||
         (detector() == CALO_PRS      && hasCaloPRS(proto))    ||
         (detector() == CALO_ECAL     && hasCaloECAL(proto))   ||
         (detector() == CALO_HCAL     && hasCaloHCAL(proto))   ||
         (detector() == CALO_BREM     && hasCaloBREM(proto))
         )
    {
      detOK = false;
    }
  }

  return detOK;
}

ProtoParticleSelection::DetectorRequirements::Vector
ProtoParticleSelection::cloneDetReqs() const
{
  DetectorRequirements::Vector newReqs;
  for ( DetectorRequirements::Vector::const_iterator iR = detReqs().begin();
        iR != detReqs().end(); ++iR )
  {
    newReqs.push_back( (*iR)->clone() );
  }
  return newReqs;
}

ProtoParticleSelection::Cut::Vector
ProtoParticleSelection::cloneCuts() const
{
  Cut::Vector newCuts;
  for ( Cut::Vector::const_iterator iC = cuts().begin();
        iC != cuts().end(); ++iC )
  {
    newCuts.push_back( (*iC)->clone() );
  }
  return newCuts;
}

ProtoParticleSelection::DetectorRequirements::Detector 
ProtoParticleSelection::DetectorRequirements::detector( const std::string & det )
{
  const DetectorRequirements::Detector Det =
    ( "RICH"          == det ? DetectorRequirements::RICH :
      "CALO"          == det ? DetectorRequirements::CALO :
      "MUON"          == det ? DetectorRequirements::MUON :
      "RICH_AEROGEL"  == det ? DetectorRequirements::RICH_AEROGEL  :
      "RICH_RICH1GAS" == det ? DetectorRequirements::RICH_RICH1GAS :
      "RICH_RICH2GAS" == det ? DetectorRequirements::RICH_RICH2GAS :
      "CALO_SPD"      == det ? DetectorRequirements::CALO_SPD :
      "CALO_PRS"      == det ? DetectorRequirements::CALO_PRS :
      "CALO_ECAL"     == det ? DetectorRequirements::CALO_ECAL :
      "CALO_HCAL"     == det ? DetectorRequirements::CALO_HCAL :
      "CALO_BREM"     == det ? DetectorRequirements::CALO_BREM :
      ProtoParticleSelection::DetectorRequirements::UndefinedDet );
  if ( Det == ProtoParticleSelection::DetectorRequirements::UndefinedDet )
  {
    throw GaudiException( "Unknown detector "+det,
                          "*ProtoParticleSelection::DetectorRequirements::detector*",
                          StatusCode::FAILURE );
  }
  return Det;
}

//=============================================================================
