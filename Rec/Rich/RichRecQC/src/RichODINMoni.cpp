
//-----------------------------------------------------------------------------
/** @file RichODINMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::DAQ::ODINMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichODINMoni.h"

// namespace
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( ODINMoni )

// Standard constructor, initializes variables
ODINMoni::ODINMoni( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : HistoAlgBase ( name, pSvcLocator )
{ }

// Destructor
ODINMoni::~ODINMoni() { }

StatusCode ODINMoni::prebookHistograms()
{
  richHisto1D( HID("TriggerType"), "LHCb Trigger Type", -0.5, 7.5, 8 ); 
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode ODINMoni::execute()
{
  // Get the ODIN
  const auto * odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );
  
  // plot of trigger types
  richHisto1D( HID("TriggerType") ) ->fill( odin->triggerType() ); 
  
  // return
  return StatusCode::SUCCESS;
}
