// $Id: TupleToolTagging.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartIF.h"

#include <Kernel/IContextTool.h>
#include <Kernel/IPhysDesktop.h>
#include <Kernel/IBTaggingTool.h>
#include <Kernel/DVAlgorithm.h>


// local
#include "TupleToolTagging.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTagging );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTagging::TupleToolTagging( const std::string& type,
				      const std::string& name,
				      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_context(0)
  , m_tagging(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("TaggingToolName", m_toolName = "BTaggingTool" );
  // declareProperty("StoreTaggersInfo", m_extendedTagging = false );

}//=============================================================================

StatusCode TupleToolTagging::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_context = tool<IContextTool>( "ContextTool", this );

  if( !m_context ){
    Error("Unable to retrieve the IGeomDispCalculator tool");
    return StatusCode::FAILURE;
  }

  const DVAlgorithm* dv = getParent();
  if( !dv ){
    Warning("Can't find the parent DVAlgorithm, will instantiate the default tagging.");
    m_tagging = tool<IBTaggingTool>( m_toolName, this );
  } else {
    m_tagging = dv->flavourTagging();
  }

  if( !m_tagging ){
    Error("Unable to retrieve the IBTaggingTool tool");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolTagging::fill( const Particle* mother
				   , const Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){

  Assert( P && mother && m_context && m_tagging,
	  "Should not happen, you are inside TupleToolTagging.cpp" );

  // nothing to tag on something which is not a B
  if( !P->particleID().hasBottom() ) return StatusCode::SUCCESS; 

  //  m_tagging = getParent()->flavourTagging();


  const VertexBase* v = m_context->desktop()->relatedVertex ( mother );
  const RecVertex* vtx = dynamic_cast<const RecVertex*>(v);
  
  FlavourTag theTag;
  StatusCode sc=StatusCode::SUCCESS;
  if( !vtx ){
    sc = m_tagging->tag( theTag, P );
  }
  else {
    sc = m_tagging->tag( theTag, P, vtx );
  }

  // try to find unphysical defaults
  int dec = -1000; 
  int cat = -1;
  double omega = -2;

  if( sc ){
    dec = theTag.decision(); 
    cat = theTag.category();
    omega = theTag.omega(); // predicted wrong tag fraction.
  }
  else {
    Warning("The tagging algorithm failed");
  }

  bool test = true;
  test &= tuple->column( head+"_TAGDECISION" , dec );
  test &= tuple->column( head+"_TAGCAT" , cat );
  test &= tuple->column( head+"_TAGOMEGA" , omega );
  
  if( msgLevel( MSG::DEBUG ) ){
    debug() << "Tagging summary: decision: " << dec 
	    << ", category: " << cat 
	    << ", omega=" << omega 
	    << endreq;
  }

  return StatusCode(test);

}

const DVAlgorithm* TupleToolTagging::getParent() const {
  
  // look for desktop
  const IAlgTool* atool = this ;
  // get last tool in chain
  debug() << "Looking for parents of " << atool->name() << endmsg ;
  while ( NULL!=dynamic_cast<const IAlgTool*>(atool->parent())){
    atool = dynamic_cast<const IAlgTool*>(atool->parent());
    debug() << "... tool is owned by tool " << atool->name() << endmsg ;
  }
  // check it's not the ToolSvc
  const IToolSvc* tsvc = dynamic_cast<const IToolSvc*>( atool->parent() );
  if ( NULL!=tsvc ){
    warning() << "Parent of " << atool->name() << " is the ToolSvc." << endmsg ;
    return NULL;
  } 

  // check if it is an algorithm
  const DVAlgorithm* dvalgo = dynamic_cast<const DVAlgorithm*>( atool->parent() );
  if ( NULL==dvalgo ){
    warning() << "Parent of " << atool->name() << " is not a DVAlgorithm." << endmsg ;
    return NULL;
  }
  
  debug() << atool->name() << " is owned by " << dvalgo->name() << endmsg ;
  return dvalgo;
}

// std::string TupleToolTagging::getCatName( const LHCb::Tagger& tag ) const{
//   std::string tts;
//   switch ( tag.type() ) {
//   case Tagger::none        : tts="none";        break;
//   case Tagger::unknown     : tts="unknown";     break;
//   case Tagger::OS_Muon     : tts="OS_Muon";     break;
//   case Tagger::OS_Electron : tts="OS_Electron"; break;
//   case Tagger::OS_Kaon     : tts="OS_Kaon";     break;
//   case Tagger::SS_Kaon     : tts="SS_Kaon";     break;
//   case Tagger::SS_Pion     : tts="SS_Pion";     break;
//   case Tagger::jetCharge   : tts="jetCharge";   break;
//   case Tagger::OS_jetCharge: tts="OS_jetCharge";break;
//   case Tagger::SS_jetCharge: tts="SS_jetCharge";break;
//   case Tagger::VtxCharge   : tts="VtxCharge";   break;
//   case Tagger::Topology    : tts="Topology";    break;
//   default: tts="Unknown tag category";
//   }
//   return tts;
// }
 
