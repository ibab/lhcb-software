// Event
#include "Kernel/STChannelID.h"

#include "STRndmEffSelector.h"

// for rndm numbers
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"


#include "STDet/DeSTSector.h"

DECLARE_TOOL_FACTORY( STRndmEffSelector)
 
STRndmEffSelector::STRndmEffSelector( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  declareInterface<ISTChannelIDSelector>(this);
}

StatusCode STRndmEffSelector::initialize()
{
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize, flat generator...
  auto tRandNumSvc = service<IRndmGenSvc>("RndmGenSvc", true);
  m_uniformDist = tRandNumSvc->generator( Rndm::Flat(0.,1.0) );
  if (!m_uniformDist) return Error( "Failed to init generator ", sc);
  return StatusCode::SUCCESS; 
}


bool STRndmEffSelector::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STRndmEffSelector::operator()( const LHCb::STChannelID& id ) const{
  auto sector = findSector(id);
  auto fractionToReject = 1.0 - sector->measEff();
  return m_uniformDist->shoot() < fractionToReject;
}
