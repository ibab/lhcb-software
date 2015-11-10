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
        tuple->column("HitE", (*aHit)->energy());

        //Fibermat info
 	tuple->column("fibermat_id",pFibreMat->FibreMatID());
	tuple->column("fibermat_layer",pFibreMat->layer());
	tuple->column("fibermat_angle",pFibreMat->angle());
	tuple->column("fibermat_quarter",pFibreMat->quarter());
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
        //CalculateHits
        StatusCode sc = pFibreMat->calculateHits(*aHit, vectChanAndFrac);
        
        //calculateListOfFiredChannels: beware of energy below!!
        ////StatusCode sc = pFibreMat->calculateListOfFiredChannels(*aHit, vectChanAndFrac);
        
        
        if (sc.isFailure()) {
          std::string org=sc.severity().getOrigin();    //get string with severity code...
          tuple->column("chitstat", (double)atoi(org.substr(1).c_str()));
          tuple->column("NFTchannels", -1);
          tuple->column("dsxmin",(float)0.);
          tuple->column("dsxmax",(float)0.);
          tuple->column("dsymin",(float)0.);
          tuple->column("dsymax",(float)0.);
          tuple->column("layermid", 16u);     //16 for failed channels...
          tuple->column("layerin", 16u);     //16 for failed channels...
          tuple->column("mat", -1);
	  tuple->column("module",16u);
	  tuple->column("sipmmid", 0u);
	  tuple->column("sipmin", 0u);
	  tuple->column("cellmid", 0u);
	  tuple->column("cellin", 0u);
	  tuple->column("channel",0u);
	  tuple->column("emid",0.);
	  tuple->column("ein",0.);
 	  tuple->column("icfMid",0u);
	  tuple->column("meanCellx",0.);
	  tuple->column("dmeanCellx",0.);
	  tuple->column("dmeanCellxw",0.);
	  tuple->column("Nlayer15",0);
	  tuple->column("Nlayer15f",0);
	  tuple->column("Nlayer15cmax",0);
	  tuple->column("Nsipm",0);
          tuple->column("mcCellX",-10000.);
          tuple->column("dsegx", 0.);
          tuple->column("segdxdy", 0.);
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
          //loop on pairs: to be adapted to farray or something
          int Nlayer15(0),Nlayer15f(0),Nlayer15c(1),Nlayer15cmax(1),Nsipm(0);
          int lastsipm(-1);
          double meanCellx(0), meanCellxw(0), senergy(0);
          for (VectFTPairs::iterator itPair = vectChanAndFrac.begin(); itPair != vectChanAndFrac.end(); ++itPair) {
            //count sipms
            int cursipm=(int)((itPair->first).sipmId());
            if(cursipm!=lastsipm) {
              Nsipm++;
              lastsipm=cursipm;
            }
            
            //count abnormal channels
            if((itPair->first).layer()==15) {
              Nlayer15++;
              if(itPair==vectChanAndFrac.begin()) Nlayer15f++;
              if(itPair!=vectChanAndFrac.end()) {
                if(((itPair+1)->first).layer()==15) Nlayer15c++;
                else Nlayer15c=1;
                if(Nlayer15c>Nlayer15cmax) Nlayer15cmax=Nlayer15c;
              }
            }
            
            //centroid
            if((itPair->first).layer()<15) {
              //take off energy for now (keep just geom fraction), for calculateListOfFiredChannels
              ////itPair->second /= (*aHit)->energy();  
                 
              meanCellx+=pFibreMat->cellUCoordinate(itPair->first)/cos(pFibreMat->angle());
              meanCellxw+=pFibreMat->cellUCoordinate(itPair->first)/cos(pFibreMat->angle())*(1 - 2.*std::abs(itPair->second));
              senergy+=(1 - 2.*std::abs(itPair->second));
            }
          }
          //loop results
          if((unsigned int)Nlayer15<vectChanAndFrac.size()) {
            meanCellx/=((int)vectChanAndFrac.size()-Nlayer15);
            meanCellxw/=senergy;
	    tuple->column("meanCellx",meanCellx);
	    tuple->column("dmeanCellx",meanCellx-pMid.Y()*tan(pFibreMat->angle()) - pMid.X());
	    tuple->column("dmeanCellxw",meanCellxw-pMid.Y()*tan(pFibreMat->angle()) - pMid.X());
          }
          else {
 	    tuple->column("meanCellx",-10000.);
 	    tuple->column("dmeanCellx",-10000.);
	    tuple->column("dmeanCellxw",-10000.);
          }
	  tuple->column("Nlayer15",Nlayer15);
	  tuple->column("Nlayer15f",Nlayer15f);
	  tuple->column("Nlayer15cmax",Nlayer15cmax);
	  tuple->column("Nsipm",Nsipm);
          tuple->column("mcCellX",pMid.X()-pMid.Y()*tan(pFibreMat->angle()));
          
          
          //status code
          std::string org=sc.severity().getOrigin();    //get string with severity code...
          tuple->column("chitstat", (double)atoi(org.substr(1).c_str()));
          tuple->column("NFTchannels",(int)vectChanAndFrac.size());
       
          //look at middle channel only (for now)
          unsigned int icfMid=vectChanAndFrac.size()/2;
          if(vectChanAndFrac.size()>0) {
            double cellSizeX=0.25;
            VectFTPairs::iterator itPair = vectChanAndFrac.begin() + icfMid; 
            VectFTPairs::iterator itPairin = vectChanAndFrac.begin(); 

            //detector segment partial test
            DetectorSegment tmpDetSeg = pFibreMat->createDetSegment( itPair->first, itPair->second );
            tuple->column("dsxmin",tmpDetSeg.xMin());
            tuple->column("dsxmax",tmpDetSeg.xMax());
            tuple->column("dsymin",tmpDetSeg.yMin());
            tuple->column("dsymax",tmpDetSeg.yMax());

            tuple->column("layermid", (itPair->first).layer());
            tuple->column("layerin", (itPairin->first).layer());
            tuple->column("mat",(int)(itPair->first).mat());
	    tuple->column("module",(itPair->first).module());
	    tuple->column("sipmmid",(itPair->first).sipmId());
	    tuple->column("sipmin",(itPairin->first).sipmId());
	    tuple->column("cellmid",(itPair->first).sipmCell());
	    tuple->column("cellin",(itPairin->first).sipmCell());
	    tuple->column("channel",(itPair->first).channelID());
	    tuple->column("emid",itPair->second);
	    tuple->column("ein",itPairin->second);
	    tuple->column("icfMid",icfMid);
            tuple->column("dsegx", tmpDetSeg.x(pMid.Y())-pMid.X());
            tuple->column("segdxdy", (tmpDetSeg.x(1000.)-tmpDetSeg.x(-1000.))/2000.);
	    double cellx = pFibreMat->cellUCoordinate(itPair->first)/cos(pFibreMat->angle())
                          - pMid.Y()*tan(pFibreMat->angle());
	    tuple->column("cellx", cellx); 
 	    tuple->column("dcellx", cellx-pMid.X());
            double cellhitx=cellx+itPair->second*cellSizeX/cos(pFibreMat->angle());
	    tuple->column("cellhitx", cellhitx); 
	    tuple->column("dcellhitx", cellhitx-pMid.X());
	    double cellLocalx = pFibreMat->cellLocalX(itPair->first);
	    tuple->column("cellLocalx", cellLocalx); 
	    tuple->column("dcellLocalx", cellLocalx-pMidloc.X());
            double cellLocalhitx=cellLocalx+itPair->second*cellSizeX;
	    tuple->column("cellLocalhitx", cellLocalhitx); 
            //zero when only 1 cell fraction used (not default) or when nftchannels==1, partly
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

