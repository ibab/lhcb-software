// $Id: ImpactPar.cpp,v 1.1 2002-02-08 18:25:42 gcorti Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// from Event
#include "LHCbEvent/AxPartCandidate.h"
#include "PhysEvent/VtxCandidate.h"

// local
#include "ImpactPar.h"

//--------------------------------------------------------------------
// Implementation file for class : ImpactPar
//
//  16/12/2001 : Gloria Corti
//
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static ToolFactory<ImpactPar> s_factory;
const IToolFactory& ImpactParFactory = s_factory;

//==================================================================
// Standard Constructor, initializes variables
//==================================================================
ImpactPar::ImpactPar(const std::string& type, const std::string& name,
                     const IInterface* parent) 
    : AlgTool( type, name, parent ) {

  declareInterface<IImpactPar>(this);

}

//==================================================================
// Calculate Impact parameter between a particle and a vertex
//==================================================================
double ImpactPar::calculate(AxPartCandidate* candidate, VtxCandidate* vertex) {

  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "Impact Parameter calculation" << endreq;

  // Find unit vector with same directions of "track" momentum
  double component3 = 1. /
                      ( sqrt ( 1.0 +
                        (candidate->fitSlopeX() * candidate->fitSlopeX()) +
                        (candidate->fitSlopeY() * candidate->fitSlopeY()) ) ); 

  double component1 = candidate->fitSlopeX() * component3;
  double component2 = candidate->fitSlopeY() * component3;
  Hep3Vector unitmom(component1,component2,component3);

  // Find displacement vector
  HepPoint3D firstp = candidate->firstPoint();
  HepPoint3D vtx = vertex->position();
  Hep3Vector displacement =  firstp - vtx;

  // The impact parameter is the magnitude of the cross product of 
  // these 2 vectors
  Hep3Vector impvector = displacement.cross(unitmom);
  return impvector.mag();
}

//=============================================================================
