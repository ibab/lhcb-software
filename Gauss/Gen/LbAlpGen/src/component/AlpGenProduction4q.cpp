// Include files 
//
// local
#include "AlpGenProduction4q.h"

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

DECLARE_TOOL_FACTORY( AlpGenProduction4q );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlpGenProduction4q::AlpGenProduction4q( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlpGenProduction ( type, name , parent )
  
{
  
  declareInterface< IProductionTool >( this ) ;
  
}

//=============================================================================
// Destructor 
//=============================================================================
AlpGenProduction4q::~AlpGenProduction4q( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode AlpGenProduction4q::initialize( ) 
{
  
  return AlpGenProduction::initialize( ) ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode AlpGenProduction4q::finalize( ) {
 
  return AlpGenProduction::finalize() ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> AlpGen
//=============================================================================
StatusCode AlpGenProduction4q::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  
  return   AlpGenProduction::generateEvent(theEvent, theCollision);
}

