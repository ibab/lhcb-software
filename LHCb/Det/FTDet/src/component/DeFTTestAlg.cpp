// $Id: $

// Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include <DetDesc/LVolume.h>

// FTDet
#include "FTDet/DeFTDetector.h" 
#include "FTDet/DeFTFibreMat.h" 

// Local
#include "DeFTTestAlg.h"

// LHCb Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

/** @file DeFTTestAlg.cpp
 *
 *  Implementation of class : DeFTTestAlg
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeFTTestAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTTestAlg::DeFTTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
//GaudiAlgorithm ( name, pSvcLocator ), 
  GaudiTupleAlg( name, pSvcLocator), 
  m_deFT(0),
  m_mcHitsLocation("")
{
  declareProperty( "MCHitsLocation", m_mcHitsLocation = "/Event/MC/FT/Hits" );
}

//=============================================================================
// Destructor
//=============================================================================
DeFTTestAlg::~DeFTTestAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTTestAlg::initialize() {

  // StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  // if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize(); 
  if ( sc.isFailure() ) { return sc; }   

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// Retrieve and initialize DeFT
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if ( m_deFT != 0 ) { debug() << "Successfully retrieved DeFT" << endmsg; }
  else { error() << "Error getting DeFT" << endmsg; }

  int FTversion(-1);
  SmartDataPtr<DetectorElement> fibretracker (detSvc(),"/dd/Structure/LHCb/AfterMagnetRegion/T/FT");
  if( fibretracker ){
    if( fibretracker -> exists( "FTversion" )) FTversion = fibretracker -> param<int>("FTversion");
    std::cout<<"----FT det version:"<<FTversion<<std::endl;
    //std::cout<<"----FT det ang:"<<fibretracker -> param<double>("BeamAngle")<<std::endl;
  }
  else std::cout<<"----no FT det"<<std::endl;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DeFTTestAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  if ( m_deFT != 0 ) {

    /// Manually defined points
    /*
    int cntrMax = 20;
    for (int cntr = 0; cntr < cntrMax; cntr++){
      double x, y, z;
      x = 1000.;
      y = 1000.;
      z = 7720. + cntr;
      const Gaudi::XYZPoint globalPoint(x, y, z);
      ...
    }
    */

    //break if not current FTversion
    if ( m_deFT->version() < 20 )   return StatusCode::SUCCESS;

    // book tuple
    Tuples::Tuple tuple = GaudiTupleAlg::nTuple("TestFTTree", "Events"); 


    /// Points of the real MCHits
    const LHCb::MCHits* hitsCont = get<LHCb::MCHits>(m_mcHitsLocation);
    /// Iterate over the first few hits and test the calculateHits method
    LHCb::MCHits::const_iterator aHit;
    std::cout<<"MC hits: "<<hitsCont->size()<<std::endl;
    for ( aHit=hitsCont->begin(); aHit != hitsCont->end(); ++aHit ) {

     Gaudi::XYZPoint pMid = (*aHit)->midPoint();
     Gaudi::XYZPoint pIn =  (*aHit)->entry();
     Gaudi::XYZPoint pOut = (*aHit)->exit();

     tuple->column("Hit_X", pMid.X()); 
     tuple->column("Hit_Y", pMid.Y()); 
     tuple->column("Hit_Z", pMid.Z()); 
     tuple->column("HitIn_X", pIn.X());
     tuple->column("HitIn_Y", pIn.Y());
     tuple->column("HitIn_Z", pIn.Z());
     tuple->column("HitOut_X", pOut.X());
     tuple->column("HitOut_Y", pOut.Y());
     tuple->column("HitOut_Z", pOut.Z());

      // Make DeFT checks
      if ( msgLevel(MSG::DEBUG) ) debug() << "\n\n\n**************************\nMC Hit " << (*aHit)->index() << "\n"
              << "**************************" << endmsg;

      std::string lVolName;

      if ( msgLevel(MSG::DEBUG) ) debug() << "Test of det. element geometry() methods, using "
              << "the midPoint() of the MC hit: " << pMid << endmsg;

      /// check isInside FT
      bool isInsideFT = m_deFT->isInside( pMid );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Global Point " << pMid << "; isInside =  " << isInsideFT << endmsg;
      if(isInsideFT) {
	tuple->column("InFT", 1); 
      }else{
	tuple->column("InFT", 0); 
      }
      /// test findStation method
      const DeFTStation* pStation = m_deFT->findStation(pMid);
      lVolName = (pStation ? pStation->geometry()->lvolumeName() : "");
      if ( msgLevel(MSG::DEBUG) ) debug() << "Found Station: " << lVolName << endmsg;
      
      /// test findLayer method
      const DeFTLayer* pLayer = m_deFT->findLayer(pMid);
      lVolName = (pLayer ? pLayer->geometry()->lvolumeName() : "");
      if ( msgLevel(MSG::DEBUG) ) debug() << "Found Layer  : " << lVolName << endmsg;

      /// test findModule method
      const DeFTModule* pModule = m_deFT->findModule(pMid);
      lVolName = (pModule ? pModule->geometry()->lvolumeName() : "");
      if ( msgLevel(MSG::DEBUG) ) debug() << "Found Module  : " << lVolName << endmsg;

      /// test findFibreModule method
      const DeFTFibreModule* pFibreModule = m_deFT->findFibreModule(pMid);
      lVolName = (pFibreModule ? pFibreModule->geometry()->lvolumeName() : "");
      if ( msgLevel(MSG::DEBUG) ) debug() << "Found FibreModule  : " << lVolName << endmsg;

      /// test findFibreMat method
      const DeFTFibreMat* pFibreMat = m_deFT->findFibreMat(pMid);
      lVolName = (pFibreMat ? pFibreMat->geometry()->lvolumeName() : "");
      if ( msgLevel(MSG::DEBUG) ) debug() << "Found FibreMat  : " << lVolName << endmsg;


      if ( pFibreMat != 0 ) {
        tuple->column("cfibrestat",1.);   //keep status
        
        //hit local coordinates
        Gaudi::XYZPoint pInloc = pFibreMat->geometry()->toLocal((*aHit)->entry());
        Gaudi::XYZPoint pMidloc = pFibreMat->geometry()->toLocal((*aHit)->midPoint());
        Gaudi::XYZPoint pOutloc = pFibreMat->geometry()->toLocal((*aHit)->exit());
        tuple->column("HitIn_XL", pInloc.X());
        tuple->column("HitIn_YL", pInloc.Y());
        tuple->column("HitIn_ZL", pInloc.Z());
        tuple->column("Hit_XL", pMidloc.X()); 
        tuple->column("Hit_YL", pMidloc.Y()); 
        tuple->column("Hit_ZL", pMidloc.Z()); 
        tuple->column("HitOut_XL", pOutloc.X());
        tuple->column("HitOut_YL", pOutloc.Y());
        tuple->column("HitOut_ZL", pOutloc.Z());
 
        //Fibermat info
 	tuple->column("fibermat_id",pFibreMat->FibreMatID());
	tuple->column("fibermat_layer",pFibreMat->layer());
	tuple->column("fibermat_angle",pFibreMat->angle());
	tuple->column("fibermat_module",pFibreMat->module());

        //Center
        tuple->column("CenterGx",pFibreMat->fibreMatGlobalCenter().X());
        tuple->column("CenterGy",pFibreMat->fibreMatGlobalCenter().Y());
        tuple->column("CenterGz",pFibreMat->fibreMatGlobalCenter().Z());
        //Corners
        tuple->column("CornerGRTx",pFibreMat->fibreMatGlobalRT().X());
        tuple->column("CornerGRTy",pFibreMat->fibreMatGlobalRT().Y());
        tuple->column("CornerGRTz",pFibreMat->fibreMatGlobalRT().Z());
        //min max
        tuple->column("fmGminx",pFibreMat->fibreMatMinX());
        tuple->column("fmGmaxx",pFibreMat->fibreMatMaxX());
        tuple->column("fmGminy",pFibreMat->fibreMatMinY());
        tuple->column("fmGmaxy",pFibreMat->fibreMatMaxY());
        tuple->column("fmGminz",pFibreMat->fibreMatMinZ());
        tuple->column("fmGmaxz",pFibreMat->fibreMatMaxZ());
        
        //test frame orientation
        Gaudi::XYZPoint testFrame=pFibreMat->geometry()->toGlobal( Gaudi::XYZPoint(100,100,0) );
        tuple->column("testFramex",testFrame.X());
        tuple->column("testFramey",testFrame.Y());
        tuple->column("testFramez",testFrame.Z());
         
        //Fibre length max
        double FibreLengthMax=pFibreMat->FibreLengh(pFibreMat->geometry()->toLocal((*aHit)->entry()),
                                                    pFibreMat->geometry()->toLocal((*aHit)->exit()));
        tuple->column("FiberLmax", FibreLengthMax);
  
   
        //get the hits in fibremat
        std::vector< std::pair<LHCb::FTChannelID, double> > vectChanAndFrac;
        StatusCode sc = pFibreMat->calculateHits(*aHit, vectChanAndFrac);
        if (sc.isFailure()) {
          std::string org=sc.severity().getOrigin();    //get string with severity code...
          tuple->column("chitstat", (double)atoi(org.substr(1).c_str()));
          tuple->column("NFTchannels", -1);
          tuple->column("dsxmin",(float)0.);
          tuple->column("dsxmax",(float)0.);
          tuple->column("dsymin",(float)0.);
          tuple->column("dsymax",(float)0.);
          tuple->column("layer", 15u);
          tuple->column("mat", -1);
	  tuple->column("module",15u);
	  tuple->column("sipm", 0u);
	  tuple->column("cell", 0u);
	  tuple->column("channel",0u);
	  tuple->column("energy",0.);
 	  tuple->column("cellx", 0.); 
	  tuple->column("dcellx", 0.);
	  tuple->column("cellhitx", 0.); 
	  tuple->column("dcellhitx", 0.);
	  tuple->column("cellLocalx", 0.); 
	  tuple->column("dcellLocalx", 0.);
	  tuple->column("cellLocalhitx", 0.); 
	  tuple->column("dcellLocalhitx", 0.);
       }
	else {
          std::string org=sc.severity().getOrigin();    //get string with severity code...
          tuple->column("chitstat", (double)atoi(org.substr(1).c_str()));
          tuple->column("NFTchannels",(int)vectChanAndFrac.size());
          //detector segment partial test
          if(vectChanAndFrac.size()>0) {
            DetectorSegment tmpDetSeg = pFibreMat->createDetSegment( vectChanAndFrac.begin()->first,
                                                                     vectChanAndFrac.begin()->second );
            tuple->column("dsxmin",tmpDetSeg.xMin());
            tuple->column("dsxmax",tmpDetSeg.xMax());
            tuple->column("dsymin",tmpDetSeg.yMin());
            tuple->column("dsymax",tmpDetSeg.yMax());
          }
       
          //loop on pairs: to be adapted to farray or something
          ////for (VectFTPairs::iterator itPair = vectChanAndFrac.begin(); itPair != vectChanAndFrac.end(); ++itPair) {
          if(vectChanAndFrac.size()>0) {
            double cellSizeX=0.25;
            VectFTPairs::iterator itPair = vectChanAndFrac.begin(); 
            tuple->column("layer", (itPair->first).layer());
            tuple->column("mat",(int)(itPair->first).mat());
	    tuple->column("module",(itPair->first).module());
	    tuple->column("sipm",(itPair->first).sipmId());
	    tuple->column("cell",(itPair->first).sipmCell());
	    tuple->column("channel",(itPair->first).channelID());
	    tuple->column("energy",itPair->second);
	    double cellx = pFibreMat->cellUCoordinate(itPair->first)/cos(pFibreMat->angle())
                          - pIn.Y()*tan(pFibreMat->angle());
	    tuple->column("cellx", cellx); 
	    ///tuple->column("dcellx", cellx-pIn.X());
 	    tuple->column("dcellx", cellx-pMid.X());
            double cellhitx=cellx+itPair->second*cellSizeX/cos(pFibreMat->angle());
	    tuple->column("cellhitx", cellhitx); 
	    ///tuple->column("dcellhitx", cellhitx-pIn.X());
	    tuple->column("dcellhitx", cellhitx-pMid.X());
	    double cellLocalx = pFibreMat->cellLocalX(itPair->first);
	    tuple->column("cellLocalx", cellLocalx); 
	    ///tuple->column("dcellLocalx", cellLocalx-pInloc.X());
	    tuple->column("dcellLocalx", cellLocalx-pMidloc.X());
            double cellLocalhitx=cellLocalx+itPair->second*cellSizeX;
	    tuple->column("cellLocalhitx", cellLocalhitx); 
            //zero when only 1 cell fraction used (not default) or when nftchannels==1, partly
	    ///tuple->column("dcellLocalhitx", cellLocalhitx-pInloc.X());   
	    tuple->column("dcellLocalhitx", cellLocalhitx-pMidloc.X()); 
	  }
          if ( msgLevel(MSG::DEBUG) ) {
            debug() << "Test of function calculateHits:\n" << endmsg;
            debug() << "Size of vector of FT pairs after call to calculateHits: " << vectChanAndFrac.size() << endmsg;
          }
        }
      }
      else {
        tuple->column("cfibrestat",-1.);
      }

      // ok, write tuple row
      StatusCode status = tuple->write(); 
      if( status.isFailure() ) return Error( "Cannot fill ntuple" ); 

    }// end loop over hits
  }// end if( m_deFT != 0 )
  
  else {
    error() << "m_deFT is not valid" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DeFTTestAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions

}

