// $Id: PlotTrackDeviation.cpp,v 1.1 2005-12-09 07:00:20 pkoppenb Exp $

// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/TrgVertex.h"
#include "Event/TrgTrack.h"

// local
#include "PlotTrackDeviation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PlotTrackDeviation
//
// 2004-12-01 : Hugo Ruiz
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PlotTrackDeviation>          s_factory ;
const        IAlgFactory& PlotTrackDeviationFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PlotTrackDeviation::PlotTrackDeviation( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) {}

//=============================================================================
// Destructor
//=============================================================================
PlotTrackDeviation::~PlotTrackDeviation() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PlotTrackDeviation::initialize() {
  std::string path = "/stat/HLTMonitor/trackDeviation/";
  std::string nameX = "X deviation vs 1/pT";
  std::string nameY = "Y deviation vs 1/pT";

  if( 0 == histoSvc() ) { return Error("histoSvc() points to NULL ");}

//  m_nPVs = histoSvc()->book(path+"0", 1, "Number of PV's", 10, 0., 9.);
  m_xDeviation = histoSvc()->book(path+"1", 2, nameX, 20, 0.0, 8. , 10, -4., 6.);
  m_yDeviation = histoSvc()->book(path+"2", 2, nameY, 20, 0.0, 8. , 10, -4., 6.);

//  m_xDeviation = histoSvc()->book(path+"1", 2, nameX, 20, 0.0, 8. , 20, -.2,.2);
//  m_yDeviation = histoSvc()->book(path+"2", 2, nameY, 20, 0.0, 8. , 20, -.2,.2);
  // get the dataProvider
  TrgDataProvider* trgData = tool<TrgDataProvider>( "TrgDataProvider" );
  m_PVs =  trgData->vertices( TrgVertexLocation::Velo3D );
  m_Tracks =  trgData->tracks( TrgTrackLocation::Long );

// Publish the histograms
//  m_publishsvc->declareInfo("nPVs",m_nPVs,"Number of PV's",this);
#ifdef DC06
  declareInfo("xDeviation", m_xDeviation, nameX);
  declareInfo("yDeviation", m_yDeviation, nameY);
#endif
  return StatusCode::SUCCESS;

};


//=============================================================================
// Main execution
//=============================================================================
StatusCode PlotTrackDeviation::execute() {

  setFilterPassed( false );

  debug() << "\n" << endreq;
  debug() << "Execution of new event" << endreq;

  
  if ( m_PVs ==  0 ) {
    debug() << "No vertices found, event aborted" << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Number of Trg vertices:" << m_PVs->size() << endreq;

//  m_nPVs->fill(m_PVs->size(), 1.0);

  // Return if more than one reco PV
  if ( m_PVs->size() != 1 ) {
    debug()  << "More than one reco PV, event aborted" << endreq;
    return StatusCode::SUCCESS;
  }


  // Get vertex position
  HepPoint3D PVPoint;   
  for ( std::vector<TrgVertex*>::const_iterator itV = m_PVs->begin();
      m_PVs->end() != itV; itV++ ) {
    
    TrgVertex& iPV =*(*itV);
    PVPoint = iPV.position();
  }

  // Get Tracks
  if ( m_Tracks ==  0 || m_Tracks->size() == 0) {
    debug() << "No tracks found, event aborted" << endreq;
    return StatusCode::SUCCESS;
  }

  for (std::vector<TrgTrack*>::const_iterator iTrackIt=m_Tracks->begin();iTrackIt!=m_Tracks->end();++iTrackIt) {
    TrgTrack& iTrack = *(*iTrackIt);
    TrgState& iState = iTrack.firstState();
    Hep3Vector iPoint = iState.position();
    double invPt = 1/ (iState.pt()/GeV);
        
    double deltaX = (iPoint.z() - PVPoint.z())*iState.xSlope() 
      - (iPoint.x() - PVPoint.x());
    debug() << "iPointz, PVpointz, xSlope, iPointx, PVpointx" << iPoint.z() 
            << PVPoint.z() << iState.xSlope() << iPoint.x() << PVPoint.x() 
            << endreq; 

    double deltaY = (iPoint.z() - PVPoint.z())*iState.ySlope() - (iPoint.y() - PVPoint.y());
    double normX = deltaX / (0.016 + 0.014 * invPt + 0.0026 * invPt * invPt); 
    double normY = deltaY / (0.016 + 0.014 * invPt + 0.0026 * invPt * invPt);
    m_xDeviation->fill(invPt, normX, 1.0);
    m_yDeviation->fill(invPt, normY, 1.0);
//    m_xDeviation->fill(invPt, deltaX, 1.0);
//    m_yDeviation->fill(invPt, deltaY, 1.0);

  }
  
  setFilterPassed( true );
  
  return StatusCode::SUCCESS;
  
};
StatusCode PlotTrackDeviation::finalize() {

  return StatusCode::SUCCESS;
                                                                                                                             
}
