// $Id: TupleToolMCDecayType.cpp,v 1.2 2009-06-01 15:40:27 pkoppenb Exp $
// Include files
#include "gsl/gsl_sys.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
// local
#include "TupleToolMCDecayType.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

// kernel
//#include "Kernel/IP2VVMCPartAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCDecayType );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCDecayType::TupleToolMCDecayType( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_mother(0)
  , m_top(0)
  , m_assocInputs(0)
  , m_useChi2Method(0)
  , m_fillSlowFind(0)
  , m_fillPseudoFind(0)
  , m_findEventTypes(0)
  , m_hasEventType(0)
  , m_hasMCDecay("")
  , m_pLink(0)
  , m_pChi2(0)
  , m_linkerTool_Links(0)
  , m_linkerTool_Chi2(0)
  , m_mcEventType(0)
  , m_mcDecay(0)
  , m_bkg(0)
{
  declareInterface<IParticleTupleTool>(this);
  
  // categorise the mcmother of this mcparticle, not the associate itself
  declareProperty( "mother", m_mother = false );

  // categorise the ultimate mcmother of this mcparticle, not the associate itself
  declareProperty( "top", m_mother = false );

  // Associator input location. Empty should be fine for most of the cases
  declareProperty( "InputLocations", m_assocInputs = std::vector<std::string>(1,"") );

  // Use the chi2 associator instead of the link and composite associator
  declareProperty( "UseChi2Method", m_useChi2Method=false );

  // search through using decay strings, very slow method
  declareProperty( "fillSlowFind", m_fillSlowFind=false );

  // construct the event types logically, much faster but less accurate
  declareProperty( "fillPseudoFind", m_fillPseudoFind=true );

  // the full list of all event types to consider.
  declareProperty( "allEventTypes", m_findEventTypes=std::vector<long unsigned int>(0) );

  // list of event types, the number of these types in the event will appear as an entry in the ntuple
  declareProperty( "hasEventTypes", m_hasEventType=std::vector<long unsigned int>(0) );

  // look for this decay string aswell
  declareProperty( "hasMCDecay", m_hasMCDecay="" );

}

//=============================================================================

StatusCode TupleToolMCDecayType::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  StatusCode sc=StatusCode::SUCCESS;
  
  m_linkerTool_Links = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Links",this);
  m_linkerTool_Chi2 = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Chi2",this);
  // m_linkerTool_Composite = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Composite",this);
  
  m_mcEventType = tool<IMCEventTypeFinder>("MCEventTypeFinder","Event_Type",this);
  m_mcDecay = tool<IMCDecayFinder>("MCDecayFinder","Decay_Type",this);
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  
  vec2set(m_findEventTypes,m_findEventTypeSet);
  vec2set(m_hasEventType,m_hasEventTypeSet);

  if(!m_findEventTypes.empty()) sc=m_mcEventType->setEventTypes(m_findEventTypeSet);
  if(!m_findEventTypes.empty() && sc.isFailure())
  {
    warning()<<"Error setting the event types starting with " << m_findEventTypes[0] << endmsg;
    m_findEventTypes.clear();
  }
  
  if (m_mcDecay && m_hasMCDecay!="") sc=m_mcDecay->setDecay(m_hasMCDecay);
  if(sc.isFailure())
  {
    warning()<<"Error setting this decay string " << m_hasMCDecay << endmsg;
    m_mcDecay=NULL;
    m_hasMCDecay="";
      
  }
  if(msgLevel(MSG::DEBUG))
  {
    //output all the options
    debug() << "TupleToolMCDecayType initialised. Values are set as follows:" << endmsg;
    debug() << "-m_mother " << m_mother << endmsg;
    debug() << "-m_useChi2Method " << m_useChi2Method << endmsg;
    debug() << "-m_fillSlowFind " << m_fillSlowFind << endmsg;
    debug() << "-m_fillPseudoFind " << m_fillPseudoFind << endmsg;
    debug() << "-m_hasMCDecay " << m_hasMCDecay << endmsg;
    debug() << "-m_assocInputs.size() " << m_assocInputs.size() << endmsg;
    debug() << "-m_findEventTypeSet.size() " << m_findEventTypeSet.size() << endmsg;
    debug() << "-m_hasEventTypeSet.size() " << m_hasEventTypeSet.size() << endmsg;
    debug() << "-m_pLink " << (m_pLink!=NULL) << endmsg;
    debug() << "-m_bkg " << (m_bkg!=NULL) << endmsg;
    //debug() << "-m_pComp " << (m_pComp!=NULL) << endmsg;
    debug() << "-m_pChi2 " << (m_pChi2!=NULL) << endmsg;
    debug() << "-m_mcEventType " << (m_mcEventType!=NULL) << endmsg;
    debug() << "-m_mcDecay " << (m_mcDecay!=NULL) << endmsg;
    
  }
  

  return StatusCode::SUCCESS;
}

StatusCode TupleToolMCDecayType::fill( const LHCb::Particle* 
                                       , const LHCb::Particle* P
                                       , const std::string& head
                                       , Tuples::Tuple& tuple ){


  if(msgLevel(MSG::DEBUG)) debug() << "Filling TupleToolMCDecayType" << endmsg;
  

  //The fill method is is two stages and is steered by the options.
  //Firstly, if asked, (fillWholeEvent) the full event is searched for all types
  //Secondly, if asked, (fillAssociated) the MC associated particle is searched for all types

  StatusCode sc=StatusCode::SUCCESS;

  bool test=true;
  
  
  if (msgLevel(MSG::DEBUG)) debug() << "finding event types for MCAssociate" << endmsg;
  LHCb::EventTypeSet foundfull;
  LHCb::EventTypeSet foundfast;
  
  //do the association, as in the MCTruth tool
  m_pLink = m_linkerTool_Links->linker(Particle2MCMethod::Links,m_assocInputs);
  //m_pComp = m_linkerTool_Composite->linker(Particle2MCMethod::Composite,m_assocInputs);
  m_pChi2 = m_linkerTool_Chi2->linker(Particle2MCMethod::Chi2,m_assocInputs);
  
  Assert( ( !m_useChi2Method && m_pLink && m_bkg ) 
          ||
          ( m_useChi2Method || m_pChi2 )
          , "One of your associator hasn't been initialized!");
  
  const MCParticle* mcp(0);
  
  if( P )
  {
    if ( m_useChi2Method && m_pChi2)
    {
      double w=0;
      mcp = m_pChi2->firstMCP( P, w );
          
          
    } 
    else
    {
      if( P->isBasicParticle() && m_pLink) mcp = m_pLink->firstMCP( P );
      else if( m_bkg)            mcp = m_bkg->origin( P );
    }
    //find the mother if requested
    if(m_mother && mcp) mcp = mcp->mother();
    //loop to the top if requested
    while(m_top && mcp && mcp->mother()) mcp = mcp->mother();
      
  }
  
  // pointer is ready, prepare the sets:
  if( mcp ) 
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "found MC Particle" << endmsg;
    //fast or slow?
    if(m_fillSlowFind) sc=m_mcEventType->findDecayType(foundfull,mcp);
    if(sc.isFailure())
    {
      warning() << "Could not perform the fill using the slow method, reverting to the fast method" << endmsg;
      m_fillSlowFind=false;
      m_fillPseudoFind=true;
    }
        
    if(m_fillPseudoFind) sc=m_mcEventType->constructDecayType(foundfast,mcp);
    if(sc.isFailure())
    {
      warning() << "Could not perform the fill using the fast method, I therefore cannot find the overall event type" << endmsg;
      m_fillPseudoFind=false;
    }
  }
  if(msgLevel(MSG::DEBUG)) debug() << "filling info for MCAssociate" << endmsg; 
  if(m_fillSlowFind)
  {
    test &= tuple->column( head+"_MCP_numFoundTypes", foundfull.size() );
    //insert "found" as an farray
    std::vector<long unsigned int> foundvec(0);
    set2vec(foundfull,foundvec);
    test &= tuple->farray( head+"_MCP_FoundTypes", foundvec, head+"_MCP_FoundLen", 20 );
      
    if(!m_hasEventTypeSet.empty()) //book the matching types
    {
          
      LHCb::EventTypeSet intersection;
      std::set_intersection(foundfull.begin(),foundfull.end(),
                            m_hasEventTypeSet.begin(), m_hasEventTypeSet.end(),
                            std::inserter(intersection,intersection.begin()),
                            LHCb::EventTypeComp() );
          
      //bool foundGiven=(intersection.size()>0);
      test &= tuple->column( head+"_MCP_numMatchingTypes", intersection.size() );
      if(msgLevel(MSG::DEBUG)) debug() << "booked intersection of size:" << intersection.size() << endmsg;
      //insert "found" as an farray
      std::vector<long unsigned int> foundvec(0);
      set2vec(intersection,foundvec);
      test &= tuple->farray( head+"_MCP_MatchingTypes", foundvec, head+"_MCP_MatchLen", m_hasEventTypeSet.size() );
    }
  }
  if(m_fillPseudoFind)
  {
    test &= tuple->column( head+"_MCP_numPseudoTypes", foundfast.size() );
    //insert "found" as an farray
    std::vector<long unsigned int> foundvec(0);
    set2vec(foundfast,foundvec);
    test &= tuple->farray( head+"_MCP_PseudoTypes", foundvec, head+"_MCP_PseudoLen", 20 );
      
    if(!m_hasEventTypeSet.empty()) //book the matching types
    {
          
      LHCb::EventTypeSet intersection;
      std::set_intersection(foundfast.begin(),foundfast.end(),
                            m_hasEventTypeSet.begin(), m_hasEventTypeSet.end(),
                            std::inserter(intersection,intersection.begin()),
                            LHCb::EventTypeComp() );
          
      //bool foundGiven=(intersection.size()>0);
      test &= tuple->column( head+"_MCP_numMatchingPseudoTypes", intersection.size() );
      if(msgLevel(MSG::DEBUG)) debug() << "booked intersection of size:" << intersection.size() << endmsg;
      //insert "found" as an farray
      std::vector<long unsigned int> foundvec(0);
      set2vec(intersection,foundvec);
      test &= tuple->farray( head+"_MCP_MatchingPseudoTypes", foundvec, head+"_MCP_MatchPseudoLen", m_hasEventTypeSet.size() );
    }
  }
  
  if(m_hasMCDecay!="" && m_mcDecay )
  {
    bool hasMCDecay =false;
    LHCb::MCParticle::ConstVector dummyvec(0); //stupid way, but is the only way!
    if (mcp) 
    {
      dummyvec.push_back(mcp);
      hasMCDecay = m_mcDecay->hasDecay(dummyvec);
    }
      
    test &= tuple->column( head+"_MCP_hasGivenDecay", hasMCDecay );
    //use the standard decay finding method
  }
  
  

  if(msgLevel(MSG::DEBUG)) debug() << "done and returning" << endmsg;
  return StatusCode(test);
  
}

bool TupleToolMCDecayType::vec2set(std::vector<long unsigned int>& avec, LHCb::EventTypeSet& aset)
{
  aset.clear();
  for(std::vector<long unsigned int>::iterator n=avec.begin(); n!=avec.end(); n++)
  {
    aset.insert(*n);
  }
  return (aset.size() > 0);
  
}

bool TupleToolMCDecayType::set2vec(LHCb::EventTypeSet& aset,std::vector<long unsigned int>& avec)
{
  avec.clear();
  avec.reserve(aset.size());
  for(LHCb::EventTypeSet::iterator n=aset.begin(); n!=aset.end(); n++)
  {
    avec.push_back(*n);
  }
  return (avec.size() > 0);
  
}
