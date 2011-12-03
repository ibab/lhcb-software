// $Id: TupleToolMCBackgroundInfo.cpp,v 1.4 2010-01-26 15:39:26 rlambert Exp $
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
  : TupleToolBase ( type, name , parent )
  , m_backCatType("BackgroundCategory")
  , m_bkg(NULL)
{

  declareInterface<IParticleTupleTool>(this);

  declareProperty( "IBackgroundCategoryType", m_backCatType);

}

//=============================================================================

StatusCode TupleToolMCBackgroundInfo::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_bkg = tool<IBackgroundCategory>( m_backCatType, "BackgroundCategory", this );

  return sc;
}

//=============================================================================

StatusCode TupleToolMCBackgroundInfo::fill( const Particle*
                                            , const Particle* P
                                            , const std::string& head
                                            , Tuples::Tuple& tuple )
{

  const std::string prefix=fullName(head);

  Assert( P && m_bkg , "This should not happen :(" );

  if( !P->isBasicParticle() )
  {

    const int category = (int)(m_bkg->category( P ));

    if (msgLevel(MSG::DEBUG)) debug() << "BackgroundCategory decision for "
                                      << prefix <<": " << category << endreq;

    if ( tuple->column( prefix+"_BKGCAT", category ) )
      return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;

}
