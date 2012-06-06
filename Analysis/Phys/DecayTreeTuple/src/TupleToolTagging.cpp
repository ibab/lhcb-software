// $Id: TupleToolTagging.cpp,v 1.9 2010-01-27 11:43:50 rlambert Exp $
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
#include <utility>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTagging )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTagging::TupleToolTagging( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  , m_dva(0)
  , m_tagging(0)
  , m_tagger_map()
  , m_tagger_rmap()
  , m_activeTaggers(0)
{
  declareInterface<IParticleTupleTool>(this);

  //there is a long list of taggers defined in the enum
  m_tagger_map[(int)Tagger::none]="none";
  m_tagger_map[(int)Tagger::unknown]="unknown";
  m_tagger_map[(int)Tagger::OS_Muon]="OS_Muon";
  m_tagger_map[(int)Tagger::OS_Electron ]="OS_Electron";
  m_tagger_map[(int)Tagger::OS_Kaon]="OS_Kaon";
  m_tagger_map[(int)Tagger::SS_Kaon]="SS_Kaon";
  m_tagger_map[(int)Tagger::SS_Pion]="SS_Pion";
  m_tagger_map[(int)Tagger::jetCharge]="jetCharge";
  m_tagger_map[(int)Tagger::OS_jetCharge]="OS_jetCharge";
  m_tagger_map[(int)Tagger::SS_jetCharge]="SS_jetCharge";
  m_tagger_map[(int)Tagger::VtxCharge]="VtxCharge";
  m_tagger_map[(int)Tagger::Topology]="Topology";

  for(std::map<int, std::string>::const_iterator t=m_tagger_map.begin();
      t!=m_tagger_map.end(); t++)
  {
    m_tagger_rmap[t->second]=t->first;
  }

  //but only these ones really need to be filled, and only these are really used
  m_activeTaggers.push_back("OS_Muon");
  m_activeTaggers.push_back("OS_Electron");
  m_activeTaggers.push_back("OS_Kaon");
  m_activeTaggers.push_back("SS_Kaon");
  m_activeTaggers.push_back("SS_Pion");
  m_activeTaggers.push_back("VtxCharge");


  declareProperty("TaggingToolName", m_toolName = "",
                  "The Tagging Tool, if empty string, the tool will be retrieved from the parent DVAlg");
  // declareProperty("StoreTaggersInfo", m_extendedTagging = false );

  declareProperty("ActiveTaggers", m_activeTaggers );
  // declareProperty("StoreTaggersInfo", m_extendedTagging = false );


}//=============================================================================

StatusCode TupleToolTagging::initialize() {
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  //if (m_dva==NULL) return Error("Couldn't get parent DVAlgorithm",
  //                           StatusCode::FAILURE);

  //if null string, get parent DVA, else use own private tool
  if(m_toolName == "" && m_dva!=NULL) m_tagging = m_dva->flavourTagging();
  else if (m_toolName != "") m_tagging = tool<IBTaggingTool>( m_toolName, this );

  if( !m_tagging && m_dva==NULL  )
  {
    return Error("Unable to retrieve the IBTaggingTool tool, unable to retrieve parent DVAlg",StatusCode::FAILURE);
  }
  if( !m_tagging )
  {
    return Error("Unable to retrieve the IBTaggingTool tool",StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolTagging::fill( const Particle* mother
                                   , const Particle* P
                                   , const std::string& head
                                   , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);

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
  int dec = 0;//-1000
  int cat = -1;
  double omega = 0.5;//-2
  int decOS = 0;
  int catOS = -1;
  double omegaOS = 0.5;

  if( sc ){
    dec = theTag.decision();
    cat = theTag.category();
    omega = theTag.omega(); // predicted wrong tag fraction.
    decOS = theTag.decisionOS();
    catOS = theTag.categoryOS();
    omegaOS = theTag.omegaOS(); // predicted wrong tag fraction.
  }
  else {
    Warning("The tagging algorithm failed");
  }

  bool test = true;
  test &= tuple->column( prefix+"_TAGDECISION" , dec );
  test &= tuple->column( prefix+"_TAGCAT" , cat );
  test &= tuple->column( prefix+"_TAGOMEGA" , omega );
  test &= tuple->column( prefix+"_TAGDECISION_OS" , decOS );
  test &= tuple->column( prefix+"_TAGCAT_OS" , catOS );
  test &= tuple->column( prefix+"_TAGOMEGA_OS" , omegaOS );

  int taggers_code = 0;
  // intialize tagger by tagger W :

  std::vector<Tagger> taggers = theTag.taggers();
  for(size_t i=0; i<taggers.size(); ++i) {
    int tdec = taggers[i].decision();

    if(tdec) switch ( taggers[i].type() ) {
    case Tagger::OS_Muon     : taggers_code +=  10000 *(tdec+2); break;
    case Tagger::OS_Electron : taggers_code +=   1000 *(tdec+2); break;
    case Tagger::OS_Kaon     : taggers_code +=    100 *(tdec+2); break;
    case Tagger::SS_Kaon     : taggers_code +=     10 *(tdec+2); break;
    case Tagger::SS_Pion     : taggers_code +=     10 *(tdec+2); break;
    case Tagger::VtxCharge   : taggers_code +=      1 *(tdec+2); break;

    }
  }

  test &= tuple->column( prefix+"_TAGGER" , taggers_code);

  if(isVerbose())
  {
    //intitialize a map to some unphysical defaults
    std::map< std::string, std::pair<int, double > > tag_results;
    for(std::vector<std::string>::const_iterator t=m_activeTaggers.begin();
        t!=m_activeTaggers.end(); t++)
    {
      tag_results[*t]=std::pair<int, double >(0,0.5);//-1000,-2
    }
    //fill the map for the taggers which were used
    for(size_t i=0; i<taggers.size(); ++i)
    {
      tag_results[m_tagger_map[(int)taggers[i].type()]]=
        std::pair<int, double >(taggers[i].decision(),taggers[i].omega());
    }
    //fill the tuple for
    for(std::vector<std::string>::const_iterator t=m_activeTaggers.begin();
        t!=m_activeTaggers.end(); t++)
    {
      test &= tuple->column( prefix+"_"+(*t)+"_DEC"   , tag_results[*t].first   );
      test &= tuple->column( prefix+"_"+(*t)+"_PROB"  , tag_results[*t].second   );
    }
  }


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

