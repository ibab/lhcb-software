// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/ODIN.h"
#include "Event/HCDigit.h"

// local
#include "TupleToolHerschel.h"

DECLARE_TOOL_FACTORY( TupleToolHerschel )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolHerschel::TupleToolHerschel( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : TupleToolBase ( type, name, parent )
{
  declareInterface<IEventTupleTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolHerschel::~TupleToolHerschel() {}
//=============================================================================

//=============================================================================
StatusCode TupleToolHerschel::fill( Tuples::Tuple& tup )
{
  const std::string prefix = fullName();
 
  // Get ODIN
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default);
  if ( !odin ) { odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default,false); }
  if ( !odin ) {
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }

  // Get Herschel digits.
  LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(LHCb::HCDigitLocation::Default);
  if ( !digits ) {
    // should always be available ...
    return Error( "Cannot load the HCDigits data object", StatusCode::SUCCESS );
  }

  // Fill ADC vectors
  std::vector<double> adcB,adcF;
  for (const LHCb::HCDigit* dig : *digits) {
    const int cell = int(dig->cellID().cellID());
    const int crate = cell >> 6;
    if (crate == 0) {
      adcB.push_back((double)dig->adc());
    } else if (crate == 1) {
      adcF.push_back((double) dig->adc());
    } else {
      info() << "Crate = " << crate << ". We have a problem." << endmsg;
    }
  }
  
  // Fill tuple 
  bool test = true;
  test &= tup->farray( prefix+"adc_B", adcB.begin(), adcB.end(),"nchB",1000);
  test &= tup->farray( prefix+"adc_F", adcF.begin(), adcF.end(),"nchF",1000);

  return StatusCode(test);
}

