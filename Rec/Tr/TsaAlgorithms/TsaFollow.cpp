// $Id: TsaFollow.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "Tsa/IITDataSvc.h"
#include "TsaFollow.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"

// Geometry
#include "STDet/DeSTDetector.h"

static const ToolFactory<TsaFollow>  s_factory;
const IToolFactory& TsaFollowFactory = s_factory;


TsaFollow::TsaFollow(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer
  declareProperty("itDataSvc", m_dataSvcName = "ITDataSvc");
  declareProperty("vTol", m_vTol = 0.5*mm);
  declareProperty("deltaU", m_deltaU = 0.90*mm);
 
  declareInterface<ITsaFollow>(this);
};

TsaFollow::~TsaFollow(){
  // destructer
}

StatusCode TsaFollow::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcName);

  // get geometry
  m_tracker = getDet<DeSTDetector>(DeSTDetectorLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode TsaFollow::execute(const unsigned int stationNum, const Tsa::Parabola& parab, 
                              const Tsa::Line& line, 
                              std::vector<Tsa::Cluster*>& clusVector){

  // Pick up hits in station where no spacepoint was found
  unsigned int  nLayer = m_tracker->station(stationNum)->numLayers();
  for (unsigned int iLayer = 1; iLayer<= nLayer; ++iLayer){
    STDetectionLayer* aLayer =   m_tracker->station(stationNum)->layer(iLayer);
    for (unsigned int iPart = 1; iPart <= 2; ++iPart){
      TsaCluster* aCluster = searchPartition(stationNum, aLayer,
                                             parab,line,iPart);
      if (0 != aCluster) clusVector.push_back(aCluster);
    }  // iPart   
  }  // iLayer
  
  return StatusCode::SUCCESS;
}




