// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel includes
#include "Kernel/VeloChannelID.h"
// Event
#include "Event/VeloDigit.h"
// Local
#include "VeloSpillSubtraction.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloSpillSubtraction )

VeloSpillSubtraction::VeloSpillSubtraction(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  GaudiAlgorithm ( name , pSvcLocator )
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/");
  declareProperty("SpillVector", m_spillNames = tmp);
  declareProperty("InputLocation", m_inputLocation = 
		  LHCb::VeloDigitLocation::Default );


  // Do not subtract Spillover if charge in previous crossing below this value
  declareProperty("PrevChargeCut", m_q_prev_cut =5.5);

  // Subtract this fraction of charge measured in previous bunch crossing
  //   ( if =0 then no subtraction is done )
  //   ( if < 0 then just kill the current hit )
  declareProperty("SpilloverFraction",  m_q_spillover_frac = 0.225 );


  // Kill VeloDigit if charge after subtraction below this value
  declareProperty("ChargeCut", m_q_cut =5.5);

  // Saturation value for previous charge (kill current if reached)
  declareProperty("ChargeSaturationCut", m_q_saturation = 126.5 );

}

VeloSpillSubtraction::~VeloSpillSubtraction() {}

StatusCode VeloSpillSubtraction::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
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

StatusCode VeloSpillSubtraction::execute()
{
    // Get input containers
    if( !exist<VeloDigits>(m_spillPaths[0]) ){
      return Error("No VeloDigits for current sampling time ",StatusCode::SUCCESS,100);
    }    
    VeloDigits* digitContCentr = get<VeloDigits>(m_spillPaths[0]);
    if( !exist<VeloDigits>(m_spillPaths[1]) ){
      return Error(" No VeloDigits for previous sampling time ",StatusCode::SUCCESS,100);
    }    
    VeloDigits* digitContPrev  = get<VeloDigits>(m_spillPaths[1]);
    std::vector<LHCb::VeloChannelID> spillChannelIDs;
    for(LHCb::VeloDigits::const_iterator ic = digitContCentr->begin(); 
        ic != digitContCentr->end(); ic++) {
      LHCb::VeloDigit* dCentr = *ic;
      LHCb::VeloDigit* dPrev = digitContPrev->object(dCentr->key());
      if( dPrev ){
        short int q_prev = dPrev->adcValue();
        if( q_prev > m_q_prev_cut ){
          if( m_q_spillover_frac < 0.0 ){
            // dCentr->setADCValue(0);
            spillChannelIDs.push_back(dCentr->key());
          } else if ( q_prev < m_q_saturation ) {
            double q_subtracted = dCentr->adcValue() - q_prev*m_q_spillover_frac;
            if( q_subtracted < m_q_cut ){
              spillChannelIDs.push_back(dCentr->key());
            } else {
              dCentr->setADCValue((short int)(q_subtracted)); 
            }
          } else {
            // dCentr->setADCValue(0);
            spillChannelIDs.push_back(dCentr->key());
          }
        }
      }
    }
    // Remove spillover killed digits from input container (central)
    std::vector<LHCb::VeloChannelID>::reverse_iterator iterVec = 
      spillChannelIDs.rbegin();
    for(; iterVec != spillChannelIDs.rend(); ++iterVec) {
      digitContCentr->erase(*iterVec);
    }

  return StatusCode::SUCCESS;
}

