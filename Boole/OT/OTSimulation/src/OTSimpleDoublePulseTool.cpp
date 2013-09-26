// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "boost/lexical_cast.hpp"

// local
#include "OTSimpleDoublePulseTool.h"

/** @file OTSimpleDoublePulseTool.cpp
 *
 *  Implementation of OT tool with simple tool.
 *
 *  @author A. Bien
 *  @date   18/10/2011
 */

using namespace LHCb;

// Declaration of the tool Factory
DECLARE_TOOL_FACTORY( OTSimpleDoublePulseTool )

OTSimpleDoublePulseTool::OTSimpleDoublePulseTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  GaudiTool( type, name, parent ),
  m_rgs(0)
{
  declareInterface<IOTDoublePulseTool>(this);
  declareProperty( "TimeOffset"             , m_timeOffset             = 30.0*Gaudi::Units::ns );
  declareProperty( "DoublePulseProbability" , m_doublePulseProbability = 0.3 );
}

OTSimpleDoublePulseTool::~OTSimpleDoublePulseTool()
{
  //destructor
}

StatusCode OTSimpleDoublePulseTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize OTSimpleDoublePulseTool", sc );

  // Random number from a flat distribution between 0 and 1
  sc = m_flat.initialize( randSvc() , Rndm::Flat( 0.0, 1.0) );
  if ( sc.isFailure() ) return Error( "Failed to initialize flat random number generator", sc );

  return StatusCode::SUCCESS;
}

IRndmGenSvc* OTSimpleDoublePulseTool::randSvc() const {
  if( 0 == m_rgs ) m_rgs = svc<IRndmGenSvc>( "RndmGenSvc", true );
  return m_rgs;
}

double OTSimpleDoublePulseTool::timeOffset(const MCOTDeposit*) const 
{
  const double probability = m_flat();
  if (probability < m_doublePulseProbability) return m_timeOffset;
  else return -1.0*Gaudi::Units::ns;

}
