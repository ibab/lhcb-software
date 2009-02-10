// $Id: STRndmClusterSelector.cpp,v 1.1 2009-02-10 09:18:38 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Event/STCluster.h"

#include "STRndmClusterSelector.h"

// for rndm numbers
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"


DECLARE_TOOL_FACTORY( STRndmClusterSelector);
 
STRndmClusterSelector::STRndmClusterSelector( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_uniformDist( (IRndmGen*)0 )
{
  declareProperty("FractionToAccept", m_fractionToAccept = 0.995); 
  declareInterface<ISTClusterSelector>(this);
}

STRndmClusterSelector::~STRndmClusterSelector()
{
  //destructer
}

StatusCode STRndmClusterSelector::initialize()
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
};


bool STRndmClusterSelector::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STRndmClusterSelector::operator()( const LHCb::STCluster*  ) const{
  return m_uniformDist->shoot() < m_fractionToAccept;
}
