// $Id: RichRingReconParam.cpp,v 1.6 2009-12-16 13:42:49 seaso Exp $
// Include files 

// local
#include "RichRingReconParam.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingReconParam
//
// 2007-06-12 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingReconParam )


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
  //  declareProperty("MinRadiator" , m_MinRadiator=0);  // lowest radiator number.
  declareProperty("MinRadiator" , m_MinRadiator=1);  // lowest radiator number. Aerogel is swiched off temporarily, to
                                                     // gain cpu time.
  declareProperty("MaxRadiator" , m_MaxRadiator=2);  // highest radiator number.
  declareProperty("ActivateSingleEvHisto", m_ActivateSingleEvHisto=false); // single event histo
  declareProperty("ActivateSingleTrackDebug",m_ActivateSingleTrackDebug= false);
  declareProperty("MinTrackNumForDebug", m_MinTrackNumForDebug=0);
  declareProperty("MaxTrackNumForDebug", m_MaxTrackNumForDebug=0);
  declareProperty("RingRecHistogramPath",  m_RingRecHistoPath="RingRecDebugTest");

  declareProperty("ActivateRandomPhiLocalScaling", m_ActivateRandomPhiLocalScaling=false); // for tests

  declareProperty("ActivateMCCompareMassPrint", m_ActivateMCCompareMassPrint=false);
 
  declareProperty("ActivateMaxNumberOfTrackSegmentsInRadiator", m_ActivateMaxNumberOfTrackSegmentsInRadiator=true);
  declareProperty("MaxNumTrackSegmentsInAerogel", m_MaxNumTrackSegmentsInAerogel=15);
  declareProperty("MaxNumTrackSegmentsInR1gas", m_MaxNumTrackSegmentsInR1gas=15);
  declareProperty("MaxNumTrackSegmentsInR2gas", m_MaxNumTrackSegmentsInR2gas=15);
  declareProperty("MinTrackMomForSelectionInAerogel", m_MinTrackMomentumSelectInAerogel =15000.0);
  declareProperty("MinTrackMomForSelectionInRich1Gas", m_MinTrackMomentumSelectInRich1Gas =30000.0);
  declareProperty("MinTrackMomForSelectionInRich2Gas", m_MinTrackMomentumSelectInRich2Gas =50000.0);
  declareProperty("ActivateWithoutTrackMomentumInfo", m_ActivateWithoutTrackMomentumInfo=false);
  

  //  declareProperty("WriteOutAuxiliaryDebugHisto", m_WriteOutAuxiliaryDebugHisto=false);
  // declareProperty( "AuxiliaryDebugHistoFileName", m_RichDebugAuxHistoFileName="RichAuxDebugHistoFile");
  
}
StatusCode RichRingReconParam::initialize() {

  const StatusCode sc = Rich::Rec::HistoToolBase::initialize();
  
  return sc;
  

}

//=============================================================================
// Destructor
//=============================================================================
RichRingReconParam::~RichRingReconParam() {} 

//=============================================================================
