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
  ST::ToolBase(type, name, parent),
  m_uniformDist( (IRndmGen*)0 )
{
  declareInterface<ISTChannelIDSelector>(this);
}

STRndmEffSelector::~STRndmEffSelector()
{
  //destructer
}

StatusCode STRndmEffSelector::initialize()
{
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize, flat generator...
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator( Rndm::Flat(0.,1.0), m_uniformDist.pRef() );
  if (sc.isFailure()) return Error( "Failed to init generator ", sc);
  sc = release(tRandNumSvc);
  if (sc.isFailure()) return Error( "Failed to release RndmGenSvc ", sc);

  return StatusCode::SUCCESS; 
}


bool STRndmEffSelector::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STRndmEffSelector::operator()( const LHCb::STChannelID& id ) const{
  const DeSTSector* sector = findSector(id);
  const double fractionToReject = 1.0 - sector->measEff();
  return m_uniformDist->shoot() < fractionToReject;
}
