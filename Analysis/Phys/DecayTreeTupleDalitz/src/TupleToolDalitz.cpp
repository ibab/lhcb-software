// $Id: TupleToolDalitz.cpp,v 1.6 2010-01-26 15:39:26 rlambert Exp $
// Include files

#include <set>
#include <string>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Event model
#include "Event/Particle.h"

// local
#include "TupleToolDalitz.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDalitz
//
// 2009-02-17 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDalitz )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolDalitz::TupleToolDalitz( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : TupleToolBase ( type, name , parent )
    , m_ppSvc(0)
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolDalitz::~TupleToolDalitz() {}

//=============================================================================
//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolDalitz::fill( const LHCb::Particle* mother
                                  , const LHCb::Particle* part
                                  , const std::string& head
                                  , Tuples::Tuple& tuple )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Dalitz fill " << fullName(head) << " " << mother
                                        << " " << part << endmsg ;
  if ( !part ) return StatusCode::FAILURE ;
  
  const LHCb::Particle::ConstVector& dauts = part->daughtersVector() ;
  if ( 2 >= dauts.size() )
  {
    return Warning("Will not fill Dalitz of two body decay "+fullName(head),StatusCode::SUCCESS,0);
  }
  
  if ( part->particleID().abspid() == 98 )
  {
    return Warning("Will not fill Dalitz for particle type CELLjet ",StatusCode::SUCCESS,0);
  }
   
  return fill(dauts,head,tuple,(part->particleID().pid()<0)) ;
}
