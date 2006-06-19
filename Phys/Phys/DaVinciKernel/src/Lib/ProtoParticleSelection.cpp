
//-----------------------------------------------------------------------------
/** @file ProtoParticleSelection.cpp
*
* Implementation file for utility class ProtoParticleSelection
*
* CVS Log :-
* $Id: ProtoParticleSelection.cpp,v 1.2 2006-06-19 10:43:35 jonrob Exp $
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
  // which detectors are available for the ProtoParticle
  
  // For RICH, the PID status flag is present if RICH info was added to the proto
  const bool hasRich = proto->hasInfo( ProtoParticle::RichPIDStatus );
  // For MUON, the PID status flag is present if RICH info was added to the proto
  const bool hasMuon = proto->hasInfo( ProtoParticle::MuonPIDStatus );
  // For CALO, for the moment hardcode to false (to be added)
  const bool hasCalo = false;
  
  bool detOK = true;
  if ( requirement() == ProtoParticleSelection::DetectorRequirements::MustHave )
  {
    if ( (detector() == ProtoParticleSelection::DetectorRequirements::RICH && !hasRich) ||
         (detector() == ProtoParticleSelection::DetectorRequirements::MUON && !hasMuon) || 
         (detector() == ProtoParticleSelection::DetectorRequirements::CALO && !hasCalo) )
    {
      detOK = false;
    }
  }
  else if ( requirement() == ProtoParticleSelection::DetectorRequirements::OnlyHave )
  {
    if ( (detector() == ProtoParticleSelection::DetectorRequirements::RICH && (!hasRich||hasMuon||hasCalo)) ||
         (detector() == ProtoParticleSelection::DetectorRequirements::MUON && (!hasMuon||hasRich||hasCalo)) || 
         (detector() == ProtoParticleSelection::DetectorRequirements::CALO && (!hasCalo||hasRich||hasMuon)) )
    {
      detOK = false;
    }
  }
  
  return detOK;
}

//=============================================================================
