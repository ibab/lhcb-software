// Include files 
//
// local
#include "AlpGenProductiontop.h"

// STL 
#include <fstream>

// Boost
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/SystemOfUnits.h"
//for getenv():
#include "GaudiKernel/System.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/Hepeup.h"
#include "LbAlpGen/AlpGenFunctions.h"
#include "LbAlpGen/Ahio.h"
#include "LbAlpGen/Aiounits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlpGenProduction
//
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( AlpGenProductiontop );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlpGenProductiontop::AlpGenProductiontop( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlpGenProduction ( type, name , parent )
  
{
  
  declareInterface< IProductionTool >( this ) ;
  
}

//=============================================================================
// Destructor 
//=============================================================================
AlpGenProductiontop::~AlpGenProductiontop( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode AlpGenProductiontop::initialize( ) 
{
  return AlpGenProduction::initialize( ) ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode AlpGenProductiontop::finalize( ) {
 
  return AlpGenProduction::finalize() ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> AlpGen
//=============================================================================
StatusCode AlpGenProductiontop::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  
  return   AlpGenProduction::generateEvent(theEvent, theCollision);
}

