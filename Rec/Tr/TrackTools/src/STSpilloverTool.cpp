// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Event/STCluster.h"
#include "Event/STMeasurement.h"
#include "Event/FitNode.h"

// local
#include "STSpilloverTool.h"
#include "boost/assign/list_of.hpp"

// Standard libraries
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>

// namespace
using namespace std;
using namespace LHCb;

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#define init_vect(A){A}
#else
#define init_vect(A) boost::assign::list_of(A)
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : STSpilloverTool
//
// 2015-06-26 : Vincenzo Battista
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( STSpilloverTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STSpilloverTool::STSpilloverTool( const string& type,
                                  const string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ISTSpilloverTool>(this);

  declareProperty( "DetType"         , m_detType = "TT" );
  declareProperty( "ADCLowEdges"         , m_adcLowEdges = init_vect(0.0) );
  declareProperty( "CosThetaLowEdges"    , m_cosThetaLowEdges = init_vect(0.0) );
  declareProperty( "TTWeightVals"        , m_TTweightVals = init_vect(0.0) );
  declareProperty( "ITWeightVals"        , m_ITweightVals = init_vect(0.0) );
  declareProperty( "WeightBins"          , m_weightBins = init_vect(0) );
}
//=============================================================================
// Destructor
//=============================================================================
STSpilloverTool::~STSpilloverTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STSpilloverTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc; // error printed already by GaudiTool

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  //Check consistency
  if((m_ITweightVals.size() != m_weightBins.size()) || (m_TTweightVals.size() != m_weightBins.size()))
  {
    err() << "Error: number of weight values and bins for weight map are different!" << endmsg;
    return StatusCode::FAILURE;
  }  
  unsigned int Size = m_weightBins.size();
  info() << "Weight map (linear) size: " << Size << endmsg;

  //Get weight map binning
  cosThetaBins = m_cosThetaLowEdges.size() - 1;
  adcBins = m_adcLowEdges.size() - 1;

  //Build pointers to make TH2D constructor happy
  cosThetaLowEdges = (float*)calloc(m_cosThetaLowEdges.size(),sizeof(float));
  if(msgLevel(MSG::DEBUG)) debug() << "CosTheta low edges:" << endmsg;
  for(unsigned int i=0; i<m_cosThetaLowEdges.size(); ++i)
  {
    cosThetaLowEdges[i] = m_cosThetaLowEdges[i];
    if(msgLevel(MSG::DEBUG)) debug() << cosThetaLowEdges[i] << endmsg;
  }

  adcLowEdges = (float*)calloc(m_adcLowEdges.size(),sizeof(float));
  if(msgLevel(MSG::DEBUG)) debug() << "ADC low edges:" << endmsg;
  for(unsigned int i=0; i<m_adcLowEdges.size(); ++i)
  {
    adcLowEdges[i] = m_adcLowEdges[i];
    if(msgLevel(MSG::DEBUG)) debug() << adcLowEdges[i] << endmsg;
  }  

  // Build weight maps
  TTWeightMap = new TH2D("TTWeightMap", 
                         "TTWeightMap", 
                         cosThetaBins, 
                         cosThetaLowEdges, 
                         adcBins, 
                         adcLowEdges);

  ITWeightMap = new TH2D("ITWeightMap",
                         "ITWeightMap",
                         cosThetaBins,
                         cosThetaLowEdges,
                         adcBins,
                         adcLowEdges);
  

  TTWeightMap->Sumw2();
  ITWeightMap->Sumw2();
  
  // Fill weight maps
  if(msgLevel(MSG::DEBUG)) debug() << "Weight map content:" << endmsg;
  for(unsigned int i=0; i<Size; ++i)
  {
    TTWeightMap->SetBinContent( m_weightBins[i], m_TTweightVals[i] );
    if(msgLevel(MSG::DEBUG)) debug() << "TT map => Bin: " << m_weightBins[i] << ", weight: " << m_TTweightVals[i] << endmsg;
  
    ITWeightMap->SetBinContent( m_weightBins[i], m_ITweightVals[i] );
    if(msgLevel(MSG::DEBUG)) debug() << "IT map => Bin: " << m_weightBins[i] << ", weight: " << m_ITweightVals[i] << endmsg;
  }  

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Use the node from a given track
//=============================================================================
double STSpilloverTool::pdfRatio( const FitNode* fNode)
{
  //Get ST cluster charge
  const STMeasurement* aHit = dynamic_cast<const STMeasurement*>(&fNode->measurement());
  const STCluster * aCluster = aHit->cluster();
  double charge = aCluster->totalCharge();

  //Get costheta (impact angle on the silicon) from the node state
  const State & aState = fNode->state();
  double costheta = cosTheta(aState);

  //Get weight from weight map
  double ratio;
  if(aCluster->isTT()) ratio = TTWeightMap->GetBinContent(TTWeightMap->FindBin(costheta, charge));
  if(aCluster->isIT()) ratio = ITWeightMap->GetBinContent(ITWeightMap->FindBin(costheta, charge));

  if(msgLevel(MSG::DEBUG)) debug() << "Weight: costheta " << costheta << ", ADC " << charge << ", weight " << ratio << endmsg;
  
  return ratio;
}

//Get costheta from the state
double STSpilloverTool::cosTheta( const State & aState )
{
  double tx = aState.tx();
  double ty = aState.ty();
  
  return 1.0 / sqrt( 1.0 + tx*tx + ty*ty );
}
