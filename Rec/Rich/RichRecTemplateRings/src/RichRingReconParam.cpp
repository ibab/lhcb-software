// $Id: RichRingReconParam.cpp,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "RichRingReconParam.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingReconParam
//
// 2007-06-12 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingReconParam );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingReconParam::RichRingReconParam( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  :  RichRingRecToolBase ( type, name , parent )
{
  declareInterface<IRichRingReconParam>(this);
  declareProperty("AerogelYHitShift",m_YAgelShift=225.0); // shift in mm for aerogel hits between Top and Bottom.
  declareProperty("MinRadiator" , m_MinRadiator=0);  // lowest radiator number.
  declareProperty("MaxRadiator" , m_MaxRadiator=2);  // highest radiator number.
  declareProperty("ActivateSingleEvHisto", m_ActivateSingleEvHisto=false); // single event histo
  declareProperty("ActivateSingleTrackDebug",m_ActivateSingleTrackDebug= false);
  declareProperty("MinTrackNumForDebug", m_MinTrackNumForDebug=0);
  declareProperty("MaxTrackNumForDebug", m_MaxTrackNumForDebug=0);
  declareProperty("RingRecHistogramPath",  m_RingRecHistoPath="RingRecDebugTest");

  declareProperty("ActivateRandomPhiLocalScaling", m_ActivateRandomPhiLocalScaling=false); // for tests

  declareProperty("ActivateMCCompareMassPrint", m_ActivateMCCompareMassPrint=false);
  

  //  declareProperty("WriteOutAuxiliaryDebugHisto", m_WriteOutAuxiliaryDebugHisto=false);
  // declareProperty( "AuxiliaryDebugHistoFileName", m_RichDebugAuxHistoFileName="RichAuxDebugHistoFile");
  
}
StatusCode RichRingReconParam::initialize() {

  const StatusCode sc = RichRecHistoToolBase::initialize();
  
  return sc;
  

}

//=============================================================================
// Destructor
//=============================================================================
RichRingReconParam::~RichRingReconParam() {} 

//=============================================================================
