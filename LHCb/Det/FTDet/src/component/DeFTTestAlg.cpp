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
        std::vector< std::pair<LHCb::FTChannelID, double> > vectChanAndFrac;
        pFibreMat->calculateHits(*aHit, vectChanAndFrac);
	tuple->column("NFTchannels",vectChanAndFrac.size());
        if ( msgLevel(MSG::DEBUG) ) {
          debug() << "Test of function calculateHits:\n" << endmsg;
          debug() << "Size of the vector of FT pairs after the call of"
                  << " function calculateHits and yes I am using this file: " << vectChanAndFrac.size() << endmsg;
        }

	for (VectFTPairs::iterator itPair = vectChanAndFrac.begin(); itPair != vectChanAndFrac.end(); ++itPair) { 
	  if ( msgLevel(MSG::DEBUG) ) debug() << "AND THE LAYER IS...... " <<  (itPair->first).layer() << endmsg;
	  tuple->column("layer", (itPair->first).layer());
	  tuple->column("mat",(itPair->first).mat());
	  tuple->column("module",(itPair->first).module());
	  tuple->column("fibermat_id",pFibreMat->FibreMatID());
	  tuple->column("fibermat_layer",pFibreMat->layer());
	  tuple->column("fibermat_angle",pFibreMat->angle());
	  tuple->column("fibermat_module",pFibreMat->module());
	  tuple->column("sipm",(itPair->first).sipmId());
	  tuple->column("cell",(itPair->first).sipmCell());
	  tuple->column("channel",(itPair->first).channelID());
	  tuple->column("energy",itPair->second);
	  double xChan = -90000000.;
	  xChan = pFibreMat->cellUCoordinate(itPair->first);
	  tuple->column("Chan_X", xChan); 
	  tuple->column("dChan_X", xChan-pMid.X());
          //detector segment
          DetectorSegment tmpDetSeg = pFibreMat->createDetSegment( itPair->first, itPair->second );
          tuple->column("dsxmin",tmpDetSeg.xMin());
          tuple->column("dsxmax",tmpDetSeg.xMax());
          tuple->column("dsymin",tmpDetSeg.yMin());
          tuple->column("dsymax",tmpDetSeg.yMax());
	} 

        //Corners
        tuple->column("CornerGRTx",pFibreMat->fibreMatGlobalRT().X());
        tuple->column("CornerGRTy",pFibreMat->fibreMatGlobalRT().Y());
        tuple->column("CornerGRTz",pFibreMat->fibreMatGlobalRT().Z());
        tuple->column("dxyRTB",sqrt(pow((pFibreMat->fibreMatGlobalRT().X()-pFibreMat->fibreMatGlobalRB().X()),2)+
                                    pow((pFibreMat->fibreMatGlobalRT().Y()-pFibreMat->fibreMatGlobalRB().Y()),2)));
        tuple->column("drRTB",(pFibreMat->fibreMatGlobalRT()-pFibreMat->fibreMatGlobalRB()).R());
        tuple->column("drTRL",(pFibreMat->fibreMatGlobalRT()-pFibreMat->fibreMatGlobalLT()).R());
        tuple->column("scRT",(pFibreMat->fibreMatGlobalRT()-pFibreMat->fibreMatGlobalRB()).Dot(pFibreMat->fibreMatGlobalRT()-pFibreMat->fibreMatGlobalLT()));
        tuple->column("CenterGx",pFibreMat->fibreMatGlobalCenter().X());
        tuple->column("CenterGy",pFibreMat->fibreMatGlobalCenter().Y());
        tuple->column("CenterGz",pFibreMat->fibreMatGlobalCenter().Z());
        
        //test frame orientation
        Gaudi::XYZPoint testFrame=pFibreMat->geometry()->toGlobal( Gaudi::XYZPoint(100,100,0) );
        tuple->column("testFramex",testFrame.X());
        tuple->column("testFramey",testFrame.Y());
        tuple->column("testFramez",testFrame.Z());
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

