// $Id: RichRingRecToolBase.cpp,v 1.2 2009-06-05 17:21:35 jonrob Exp $
// Include files 

// local
#include "RichRingRec/RichRingRecToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecToolBase
//
// 2007-05-18 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

DECLARE_TOOL_FACTORY(RichRingRecToolBase)
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecToolBase::RichRingRecToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : Rich::Rec::HistoToolBase ( type, name , parent )
{
  declareInterface<IRichRingRecToolBase>(this);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingRecToolBase::initialize() {

  const StatusCode sc = Rich::Rec::HistoToolBase::initialize();


  m_RichRingRecConstants = tool<IRichRingRecConstants>("Rich::Rec::TemplateRings::RichRingRecConstants");
  
  // acquiretool does not seem to work here. so using the basic version which seems to work fine.
  m_TargetDataTool = tool<IRichTargetDataTool>("Rich::Rec::TemplateRings::RichTargetDataTool");
  //if(!m_TargetDataTool)  acquireTool("RichTargetDataTool", m_TargetDataTool);
  
  m_TemplateDataTool = tool<IRichTemplateDataTool>("Rich::Rec::TemplateRings::RichTemplateDataTool");
  m_RichRingRecTransformTool= tool<IRichRingRecTransformTool>("Rich::Rec::TemplateRings::RichRingRecTransformTool");
  m_RichRingReconParam= tool<IRichRingReconParam>("Rich::Rec::TemplateRings::RichRingReconParam");
  m_RichLocalTargetConfig = tool<IRichLocalTargetConfig>("Rich::Rec::TemplateRings::RichLocalTargetConfig");
  m_RichLocalTemplateConfig = tool<IRichLocalTemplateConfig>("Rich::Rec::TemplateRings::RichLocalTemplateConfig");
  m_RichFFPlan = tool<IRichFFPlan>("Rich::Rec::TemplateRings::RichFFPlan");
  m_RichRingRecResult = tool<IRichRingRecResult>("Rich::Rec::TemplateRings::RichRingRecResult");
  m_RichSingleEvMonHistoTool=tool<IRichSingleEvMonHistoTool>("Rich::Rec::TemplateRings::RichSingleEvMonHistoTool");
  

  m_RichMCTruthData = tool<IRichMCTruthData>("Rich::Rec::TemplateRings::RichMCTruthData");  
  m_RichRingRecMass=tool<IRichRingRecMass>("Rich::Rec::TemplateRings::RichRingRecMass");
  //  m_IRichRingRecStorePidTool=tool<IRichRingRecStorePidTool>("RichRingRecStorePidTool");
  

  // tools from the other rich reconstrution packages

  //  if(! m_RichPhotonPredictor)  acquireTool("RichPhotonPredictor", m_RichPhotonPredictor);
  
  return sc;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingRecToolBase::finalize() 
{
  return Rich::Rec::HistoToolBase::finalize();  
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
