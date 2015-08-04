// Event
#include "Kernel/STChannelID.h"

#include "STRndmChannelIDSelector.h"

// for rndm numbers
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"


DECLARE_TOOL_FACTORY( STRndmChannelIDSelector)
 
STRndmChannelIDSelector::STRndmChannelIDSelector( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_uniformDist( (IRndmGen*)0 )
{
  declareProperty("FractionToReject", m_fractionToReject = 0.0); 
  declareInterface<ISTChannelIDSelector>(this);
}

STRndmChannelIDSelector::~STRndmChannelIDSelector()
{
  //destructer
}

StatusCode STRndmChannelIDSelector::initialize()
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


bool STRndmChannelIDSelector::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STRndmChannelIDSelector::operator()( const LHCb::STChannelID&  ) const{
  return m_uniformDist->shoot() < m_fractionToReject;
}
