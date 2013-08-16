// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolVtxIsoln.h"

#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include <Kernel/IVertexFit.h>
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "CaloUtils/CaloAlgUtils.h"

#include "Event/Particle.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolVtxIsoln
//
// @author Mitesh Patel, Patrick Koppenburg
// @date   2008-04-15
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolVtxIsoln )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolVtxIsoln::TupleToolVtxIsoln( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : TupleToolBase ( type, name , parent )
    , m_dva(0)
    , m_dist(0)
    , m_pVertexFit(0)
    , m_isolationTool(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("MaxChi2", m_Chi2 = 9.0);
  declareProperty("VertexFit", m_typeVertexFit = "default");
  declareProperty("InputParticles", m_inputParticles );
}

//=============================================================================

StatusCode TupleToolVtxIsoln::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (!m_dva) return Error("Couldn't get parent DVAlgorithm",
                           StatusCode::FAILURE);

  m_dist = m_dva->distanceCalculator ();
  if ( !m_dist )
  {
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }

  m_pVertexFit= m_dva->vertexFitter();
  if ( !m_pVertexFit )
  {
    Error("Unable to retrieve the IVertexFit tool");
    return StatusCode::FAILURE;
  }

  m_isolationTool = tool<IExtraInfoTool>("VertexIsolation", "VertexIsolation", this) ;
  if ( !m_isolationTool )
  {
    Error("Unable to retrieve the isolation tool");
    return StatusCode::FAILURE;
  }
  // Propagate properties (I don't know how to do it)
  //m_isolationTool->setProperty("MaxChi2", m_Chi2) ;
  //m_isolationTool->setProperty("InputParticles", m_inputParticles) ;
  // Get var names
  int firstIndex = m_isolationTool->getFirstIndex() ;
  int lastIndex = m_isolationTool->getNumberOfParameters() + firstIndex ;
  double dummy ;
  std::string varName ;
  for (int index = firstIndex; index != lastIndex; index++)
  {
    m_isolationTool->getInfo(index, dummy, varName) ;
    m_varNames[index] = varName ;
  }

  return sc;
}

//=============================================================================

StatusCode TupleToolVtxIsoln::fill( const Particle* mother
                                    , const Particle* P
                                    , const std::string& head
                                    , Tuples::Tuple& tuple ){

  const std::string prefix = fullName(head);
  Assert( P && mother && m_dist, "This should not happen, you are inside TupleToolVtxIsoln.cpp :(" );
  // Check the ExtraInfo
  bool test = true ;
  int firstIndex = m_isolationTool->getFirstIndex() ;
  int lastIndex = m_isolationTool->getNumberOfParameters() + firstIndex ;
  if ( P->isBasicParticle() ) return StatusCode::SUCCESS ;
  // Fill tuple
  if ( P->hasInfo(firstIndex) )
  {
    double def = -999999. ;
    for (int index = firstIndex; index != lastIndex; ++index)
    {
      double val ;
      val = P->info(index, def) ;
      test &= tuple->column(prefix+"_"+m_varNames[index], val);
    }
  }
  else
  {
    m_isolationTool->calculateExtraInfo(P, P) ;
    for (int index = firstIndex; index != lastIndex; ++index)
    {
      double val ;
      std::string name ;
      int result = m_isolationTool->getInfo(index, val, name) ;
      if ( !result ) continue ;
      test &= tuple->column(prefix+"_"+name, val);
    }
  }
  return StatusCode(test) ;
}

