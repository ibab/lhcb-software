// $Id: GeomDispCalculator.cpp,v 1.26 2010-01-13 16:17:25 pkoppenb Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IParticleTransporter.h"

// From LHC
#include "LHCbMath/GeomFun.h"
// from Event
#include "Event/Particle.h"

// local
#include "GeomDispCalculator.h"
//--------------------------------------------------------------------
//
//  ClassName  : GeomDispCalculator
// 
//  Description: These methods return the impact parameter, the
//               distance between two vertices and the distance of
//               closest approach
//
//  Authors    : Miriam Gandelman, Juan Palacios 
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
DECLARE_TOOL_FACTORY( GeomDispCalculator );

//==================================================================
// Standard Constructor
//==================================================================
GeomDispCalculator::GeomDispCalculator(const std::string& type, 
                                       const std::string& name, 
                                       const IInterface* parent) 
  : 
  GaudiTool( type, name, parent)
{

  declareInterface<IGeomDispCalculator>(this);

}
//==================================================================
// Initialize
//==================================================================
StatusCode GeomDispCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  return  printFailure() ;
  
}

