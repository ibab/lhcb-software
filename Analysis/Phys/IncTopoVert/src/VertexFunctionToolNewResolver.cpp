// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "VertexFunctionToolNewResolver.h"


//-----------------------------------------------------------------------------
// Implementation file for class : VertexFunctionToolNewResolver
//
// 2012-10-15 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexFunctionToolNewResolver )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexFunctionToolNewResolver::VertexFunctionToolNewResolver( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
: VertexFunctionTool ( type, name, parent )
{

}

//=============================================================================

bool   VertexFunctionToolNewResolver::areResolved(LHCb::RecVertex & V1,LHCb::RecVertex & V2)
{
  //info()<<"VertexFunctionToolNewResolver::areResolved "<<&V1<<" "<<&V2<<endmsg;

  //====================================================================
  // This function check if two vertices are resolved. Return true if so.
  //====================================================================
  std::vector <double> MeshPoints;

  Gaudi::XYZPoint P1(V1.position());
  Gaudi::XYZPoint P2(V2.position());
  double distance = (P2-P1).R();
  Gaudi::XYZVector Dir(P2-P1);
  Dir *= 1./distance;

  MeshPoints.push_back(0);
  MeshPoints.push_back(distance);

  //compute Vmin
  double Vmin = valueAt(V1);
  if(Vmin>valueAt(V2)) Vmin = valueAt(V2);
  if (Vmin <= 1e-8) return false;


  //int NumberOfIterations =0;
  //return areResolveDicotomy(P1,Dir,Vmin,MeshPoints,NumberOfIterations);

  return areResolveDicotomy(P1,Dir,Vmin,1,distance/2.);

}


//=============================================================================
// Destructor
//=============================================================================
VertexFunctionToolNewResolver::~VertexFunctionToolNewResolver() {}

//=============================================================================
