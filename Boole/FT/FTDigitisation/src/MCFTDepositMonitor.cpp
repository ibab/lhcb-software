// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/MCHit.h"

// FTDet
#include "FTDet/DeFTDetector.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "FTDet/DeFTFibreMat.h"

// local
#include "MCFTDepositMonitor.h"

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>


using namespace LHCb;
using namespace Gaudi;
using namespace boost;

//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDepositMonitor
//
// 2012-07-04 : Eric Cogneras
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDepositMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCFTDepositMonitor::MCFTDepositMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
    , m_deFT(NULL)
{
  std::vector<std::string> tmp1 = boost::assign::list_of("/PrevPrev/")
                                                        ("/Prev/")
                                                        ("/")
                                                        ("/Next/")
                                                        ("/NextNext/");

  declareProperty( "SpillVector"                , m_spillVector                 = tmp1);
  declareProperty("HitLocation" , m_hitLocation = LHCb::MCHitLocation::FT, "Path to  MCHits");
  declareProperty("DepositLocation" , m_depositLocation =  LHCb::MCFTDepositLocation::Default, "Path to MCFTDeposits");
}
//=============================================================================
// Destructor
//=============================================================================
MCFTDepositMonitor::~MCFTDepositMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDepositMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// Retrieve and initialize DeFT
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if( NULL!=m_deFT) {
    if(msgLevel(MSG::DEBUG)) debug() << "Successfully retrieved DeFT" << endmsg;
  }  
  else { return Error("Could not retrieve " + DeFTDetectorLocation::Default); }

  // construct container spill names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+m_hitLocation;//MCHitLocation::FT;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // retrieve Hits
  const MCHits* mcHitsCont = get<MCHits>(m_hitLocation);
  if ( msgLevel( MSG::DEBUG) ) 
    debug() <<"mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;
  

  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){
    //pointer to the Hit
    MCHit* ftHit = *iterHit;

    plot2D( ftHit->entry().x(), ftHit->entry().y(), "x/y entry",  -500., 500., -500., 500., 100, 100 );


    plot(ftHit->energy(),"CheckEnergyPerHit", "Energy deposited [Hit level]; Energy [MeV];Number of hits" , 0 , 10 );  

    // ( call of calculateListOfFiredChannels method) 
    const DeFTFibreMat* pL = m_deFT->findFibreMat(ftHit->midPoint());
    FTDoublePairs energyDistribution;
    
    double fibrelengh =0 ;
    double fibrelenghfraction = 0;

    if (pL) {
      if(pL->calculateListOfFiredChannels(ftHit, energyDistribution)){
        

        plot(pL->angle()*180/M_PI,"CheckStereoAngle","Stereo Angle; Stereo Angle [#degree];" ,-10 , 10);
        plot(pL->layerInnerHoleRadius(),"CheckHoleRadius","Hole Radius ; Hole Radius  [mm];" ,50 , 150);
        plot(pL->layerMaxY(),"CheckLayerHalfSizeY","Layer Half Size Y ; Layer Half Size Y  [mm];" ,3000 , 3100);

        if(pL->hitPositionInFibre(ftHit, fibrelengh,fibrelenghfraction)){
        
          plot(fibrelengh,"CheckFibreLengh","FibreLengh; Sci. Fibre Lengh [mm]; Nber of Events" ,2900 ,3100);
          plot(fibrelenghfraction,"CheckFibreFraction","FibreFraction; Sci. Fibre Fraction ; Nber of Events" ,0 ,1);
    
          // Attenuation :
          double AttLengh = 3300;
          double AttFactor = 0.5 * ( 1 + exp( (-2.) * fibrelengh * (1 - fibrelenghfraction) / AttLengh))
            / exp(  fibrelengh * fibrelenghfraction  /AttLengh)
            ;
          plot(AttFactor,"CheckAttFactor","AttFactorDistrib; Attenuation factor ; Nber of Events" ,0 ,1);
        
        }
      
 
    
        plot(energyDistribution.size(),"CheckNbChannel", 
             "Number of fired channels per Hit; Number of fired channels; Number of hits" , 
             0 , 100);
      
        FTDoublePairs::const_iterator vecIter;
        for(vecIter = energyDistribution.begin(); vecIter != energyDistribution.end(); ++vecIter){
          // checks layer is physical layer
          if ( vecIter->first.layer() < 15 ) {
            plot(vecIter->second,"CheckEnergyPerCell","CheckEnergyPerCell; Energy in Cell ; Nber of Channels" ,0. ,10);
          }
          
        }
      }
    
    }
  }
  
  // retrieve FTDeposits
  const MCFTDeposits* mcDepositsCont = get<MCFTDeposits>(m_depositLocation);
  if ( msgLevel( MSG::DEBUG) ) 
    debug() <<"mcDepositsCont->size() : " << mcDepositsCont->size()<< endmsg;
  MCFTDeposits::const_iterator iterDeposit = mcDepositsCont->begin();
  for (; iterDeposit!=mcDepositsCont->end();++iterDeposit){
    plot((double)(*iterDeposit)->channelID(), "DepFiredChannel","Fired Channel; ChannelID" , 0. , 1000000., 10000);
    plot((double)(*iterDeposit)->channelID().sipmId(), "DepFiredSiPMID","SiPMID of Fired Channel; SiPMID", 0.,20.,20);
    plot((double)(*iterDeposit)->channelID().sipmCell(), "DepFiredSiPMChannel","SiPMCell Fired Channel; SiPMID", 0.,140.,140);
    plot((double)(*iterDeposit)->mcHitVec().size(), "DepHitPerChannel",
         "Number of Hits per Channel;Number of Hits per Channel;" , 0. , 50.);

    for (unsigned int idx = 0; idx < (*iterDeposit)->mcHitVec().size(); idx++) {
      // Direct energy to channel
      plot((*iterDeposit)->energyVec()[idx],
           "DepEnergyPerChannel",
           "Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 100);
      plot((*iterDeposit)->energyVec()[idx],
           "DepEnergyPerChannelZoom",
           "Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot((*iterDeposit)->energyVec()[idx],
           "DepEnergyPerChannelBigZoom",
           "Attenuation correction factor[Channel level]; Attenuation correction factor;Number of SiPM channels", 
           //"Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 1);
      // Reflected energy to channel
      plot((*iterDeposit)->energyRefVec()[idx],
           "DepEnergyPerChannelRef",
           "Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 100);
      plot((*iterDeposit)->energyRefVec()[idx],
           "DepEnergyPerChannelRefZoom",
           "Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot((*iterDeposit)->energyRefVec()[idx],
           "DepEnergyPerChannelRefBigZoom",
           "Attenuation correction factor[Channel level]; Attenuation correction factor;Number of SiPM channels", 
           //"Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 1);
      // Direct pulse arrival time to channel
      plot((*iterDeposit)->timeVec()[idx],
           "DirArrivalTimePerChannel",
           "Arrival time [Channel level]; Time [ns]; Number of SiPM channels", 
           -50, 150);
      plot((*iterDeposit)->timeRefVec()[idx],
           "RefArrivalTimePerChannel",
           "Arrival time [Channel level]; Time [ns]; Number of SiPM channels", 
           -50, 150);

      /*
      // Arrival time and spillover
      std::string spillname = findSpill( (*iterDeposit)->mcHitVec()[idx] );
      plot((*iterDeposit)->mcHitVec()[idx]->time(),
           (spillname+std::string("DirArrivalTimePerChannelSpill")).c_str(),
           "Arrival time [Channel level]; Time [ns]; Number of SiPM channels", 
           -50, 150);
      */
    }
    
  }


  return StatusCode::SUCCESS;
}


std::string 
MCFTDepositMonitor::findSpill( const LHCb::MCHit* mcHit )
{
  // Loop over the spills
  // retrieve MCHits and make first list of deposits
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){
    SmartDataPtr<MCHits> ftMCHits( eventSvc(), m_spillNames[iSpill] );

    if (!ftMCHits) {
      if (msgLevel(MSG::DEBUG)) 
	debug() << "Spillover missing in the loop " + m_spillNames[iSpill] <<endmsg;
    } else {
      // found spill - create some digitizations and add them to deposits
      if ( msgLevel( MSG::DEBUG) )
	debug() << "ftMCHits->size() : " << ftMCHits->size()<< endmsg;

      // add the loop on the hits
      for ( MCHits::const_iterator iterHit = ftMCHits->begin(); 
	    iterHit!=ftMCHits->end();++iterHit){
	if (*iterHit == mcHit)
	  return m_spillVector[iSpill];
      } // loop over hit
    } // spillover
  } // loop on ftMChits
  return "";
}
//=============================================================================
