// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STAlgBase.h"
// Event
#include "Event/STDigit.h"
// Local
#include "STSpilloverSubtraction.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY(STSpilloverSubtraction);

STSpilloverSubtraction::STSpilloverSubtraction(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator)
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/");
  declareProperty("SpillVector", m_spillNames = tmp);
  declareSTConfigProperty("InputLocation", m_inputLocation, 
                                           STDigitLocation::TTDigits);

  // Do not subtract Spillover if charge in previous crossing below this value
  declareProperty("PrevChargeCut", m_q_prev_cut =8.5);

  // Subtract this fraction of charge measured in previous bunch crossing
  //   ( if < 0 then set current charge to zero )
  declareProperty("SpilloverFraction",  m_q_spillover_frac = 0.25 );

  // Saturation value for previous charge (kill current if reached)
  declareProperty("ChargeSaturationCut", m_q_saturation = 126.5 );

}

STSpilloverSubtraction::~STSpilloverSubtraction() {}

StatusCode STSpilloverSubtraction::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  m_spillPaths.clear();
  // Load spill paths
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }

  return StatusCode::SUCCESS;
}

StatusCode STSpilloverSubtraction::execute()
{
    // Get input containers
    if( !exist<STDigits>(m_spillPaths[0]) ){
      return Error(" No STDigits for current sampling time ",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContCentr = get<STDigits>(m_spillPaths[0]);
    if( !exist<STDigits>(m_spillPaths[1]) ){
      return Error(" No STDigits for previous sampling time ",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContPrev  = get<STDigits>(m_spillPaths[1]);
    for(LHCb::STDigits::iterator ic = digitContCentr->begin(); 
        ic != digitContCentr->end(); ic++) {
      LHCb::STDigit* dCentr = *ic;
      LHCb::STDigit* dPrev = digitContPrev->object(dCentr->key());
      if( dPrev ){
        double q_prev = dPrev->depositedCharge();
        if( q_prev > m_q_prev_cut ){
          if( m_q_spillover_frac < 0.0 ){
            dCentr->setDepositedCharge(0.0);
          } else if ( q_prev < m_q_saturation ) {
            dCentr->setDepositedCharge(dCentr->depositedCharge() - q_prev*m_q_spillover_frac); 
          } else {
            dCentr->setDepositedCharge(0.0);
          }          
        }
      }
    }


  return StatusCode::SUCCESS;
}

