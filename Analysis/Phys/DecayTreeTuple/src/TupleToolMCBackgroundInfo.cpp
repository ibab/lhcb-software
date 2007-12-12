// $Id: TupleToolMCBackgroundInfo.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMCBackgroundInfo.h"

#include "Kernel/IBackgroundCategory.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MCBackgroundInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCBackgroundInfo );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCBackgroundInfo::TupleToolMCBackgroundInfo( const std::string& type,
						      const std::string& name,
						      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolMCBackgroundInfo::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolMCBackgroundInfo::fill( const Particle*
					    , const Particle* P
					    , const std::string& head
					    , Tuples::Tuple& tuple ) {
  Assert( P && m_bkg , "This should not happen :(" );

  if( !P->isBasicParticle() ){

    int category = (int)(m_bkg->category( P ));

    debug() << "BackgroundCategory decision for "
	    << head <<": " << category << endreq;

    if( tuple->column( head+"_BKGCAT", category ) ) 
      return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}
