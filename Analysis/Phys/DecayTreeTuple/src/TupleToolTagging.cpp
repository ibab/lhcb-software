// $Id: TupleToolTagging.cpp,v 1.7 2010-01-20 17:22:36 bkhanji Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartIF.h"

#include <Kernel/GetDVAlgorithm.h>
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
  , m_dva(0)
  , m_tagging(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("TaggingToolName", m_toolName = "BTaggingTool" );
  // declareProperty("StoreTaggersInfo", m_extendedTagging = false );

}//=============================================================================

StatusCode TupleToolTagging::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);

  m_tagging = m_dva->flavourTagging();

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

  Assert( P && mother && m_dva && m_tagging,
	  "Should not happen, you are inside TupleToolTagging.cpp" );

  // nothing to tag on something which is not a B
  if( !P->particleID().hasBottom() ) return StatusCode::SUCCESS; 

  const VertexBase* v = m_dva->bestPV ( mother );
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
  
  int taggers_code = 0;
  // intialize tagger by tagger W :
  double os_mu_prob  = -1;
  double os_el_prob  = -1;
  double os_k_prob   = -1;
  double ss_k_prob   = -1;
  double ss_pi_prob  = -1;
  double os_vtx_prob = -1;
  
  std::vector<Tagger> taggers = theTag.taggers();
  for(size_t i=0; i<taggers.size(); ++i) {
    int tdec = taggers[i].decision();
    double om = taggers[i].omega();
    
    if(tdec) switch ( taggers[i].type() ) {
    case Tagger::OS_Muon     : taggers_code += 10000 *(tdec+2); os_mu_prob = om;break;
    case Tagger::OS_Electron : taggers_code +=  1000 *(tdec+2); os_el_prob = om;break;
    case Tagger::OS_Kaon     : taggers_code +=   100 *(tdec+2); os_k_prob  = om;break;
    case Tagger::SS_Kaon     : taggers_code +=    10 *(tdec+2); ss_k_prob  = om;break;
    case Tagger::SS_Pion     : taggers_code +=    10 *(tdec+2); ss_pi_prob = om;break;
    case Tagger::VtxCharge   : taggers_code +=     1 *(tdec+2); os_vtx_prob= om;break;
      
    }
  }
  test &= tuple->column( head+"_TAGGER" , taggers_code);
  test &= tuple->column( "OS_MU_PROBA"  , os_mu_prob  );
  test &= tuple->column( "OS_EL_PROBA"  , os_el_prob  );
  test &= tuple->column( "OS_K_PROBA"   , os_k_prob   );
  test &= tuple->column( "SS_K_PROBA"   , ss_k_prob   );
  test &= tuple->column( "SS_PI_PROBA"  , ss_pi_prob  );
  test &= tuple->column( "OS_VTX_PROBA" , os_vtx_prob );
  
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
 
