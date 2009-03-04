// $Id: RichRingRecToolBase.cpp,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRingRec/RichRingRecToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecToolBase
//
// 2007-05-18 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingRecToolBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecToolBase::RichRingRecToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichRecHistoToolBase ( type, name , parent )
{
  declareInterface<IRichRingRecToolBase>(this);

}
//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingRecToolBase::initialize() {

  const StatusCode sc = RichRecHistoToolBase::initialize();


  m_RichRingRecConstants = tool<IRichRingRecConstants>("RichRingRecConstants");
  
  // acquiretool does not seem to work here. so using the basic version which seems to work fine.
  m_TargetDataTool = tool<IRichTargetDataTool>("RichTargetDataTool");
  //if(!m_TargetDataTool)  acquireTool("RichTargetDataTool", m_TargetDataTool);
  
  m_TemplateDataTool = tool<IRichTemplateDataTool>("RichTemplateDataTool");
  m_RichRingRecTransformTool= tool<IRichRingRecTransformTool>("RichRingRecTransformTool");
  m_RichRingReconParam= tool<IRichRingReconParam>("RichRingReconParam");
  m_RichLocalTargetConfig = tool<IRichLocalTargetConfig>("RichLocalTargetConfig");
  m_RichLocalTemplateConfig = tool<IRichLocalTemplateConfig>("RichLocalTemplateConfig");
  m_RichFFPlan = tool<IRichFFPlan>("RichFFPlan");
  m_RichRingRecResult = tool<IRichRingRecResult>("RichRingRecResult");
  m_RichSingleEvMonHistoTool=tool<IRichSingleEvMonHistoTool>("RichSingleEvMonHistoTool");
  

  m_RichMCTruthData = tool<IRichMCTruthData>("RichMCTruthData");  
  m_RichRingRecMass=tool<IRichRingRecMass>("RichRingRecMass");
  //  m_IRichRingRecStorePidTool=tool<IRichRingRecStorePidTool>("RichRingRecStorePidTool");
  

  // tools from the other rich reconstrution packages

  //  if(! m_RichPhotonPredictor)  acquireTool("RichPhotonPredictor", m_RichPhotonPredictor);
  
  return sc;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingRecToolBase::finalize() {

  const StatusCode sc = RichRecHistoToolBase::finalize();


  return StatusCode::SUCCESS;
  
}
//=============================================================================
// tools access 
//=============================================================================

IRichTargetDataTool* RichRingRecToolBase::tgD()
 {    return m_TargetDataTool;}
IRichRingRecConstants* RichRingRecToolBase::RConst()
{ return m_RichRingRecConstants; }
IRichTemplateDataTool*  RichRingRecToolBase::tmD()
{return m_TemplateDataTool;}
IRichRingRecTransformTool*  RichRingRecToolBase::Tfm()
{return m_RichRingRecTransformTool;}
IRichRingReconParam* RichRingRecToolBase::RParam()
{return m_RichRingReconParam;}
IRichLocalTargetConfig* RichRingRecToolBase::RLocTgC()
{  return m_RichLocalTargetConfig;}
IRichLocalTemplateConfig* RichRingRecToolBase::RLocTDC()
{  return m_RichLocalTemplateConfig;}
IRichFFPlan*  RichRingRecToolBase::RFFP(){  return m_RichFFPlan;}
IRichRingRecResult*  RichRingRecToolBase::RRslt() {return m_RichRingRecResult;}
IRichSingleEvMonHistoTool* RichRingRecToolBase::HSingleEv()
{  return m_RichSingleEvMonHistoTool;}
IRichMCTruthData* RichRingRecToolBase::MCT() 
{  return m_RichMCTruthData;}
IRichRingRecMass*  RichRingRecToolBase::RMass(){  return m_RichRingRecMass;}
//IRichRingRecStorePidTool* RichRingRecToolBase::RPid(){return m_IRichRingRecStorePidTool;}


//IRichPhotonPredictor* RPhotonPredict()
//{  return m_RichPhotonPredictor;}


//=============================================================================
// Destructor
//=============================================================================
RichRingRecToolBase::~RichRingRecToolBase() {} 

//=============================================================================
