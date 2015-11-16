/** @file MCFTDepositCreator.cpp
 *
 *  Implementation of class : MCFTDepositCreator
 *
 *  @author COGNERAS Eric
 *  @date   2012-06-05
 */

// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// FTDet
#include "FTDet/DeFTDetector.h"

// from FTEvent

#include "FTDet/DeFTLayer.h"
#include "FTDet/DeFTFibreMat.h"

// local
#include "MCFTDepositCreator.h"

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>


using namespace LHCb;
using namespace Gaudi;
using namespace boost;


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDepositCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDepositCreator::MCFTDepositCreator( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: GaudiHistoAlg ( name , pSvcLocator )
  , m_spillNames()
  , m_deFT(nullptr)
  , m_yMax(0.0)
  , m_numLayers(12)
{


  std::vector<std::string> tmp1 = { "/PrevPrev/",
                                    "/Prev/",
                                    "/",
                                    "/Next/",
                                    "/NextNext/"};
                                    
  std::vector<double> tmp2 = { -50.0*Gaudi::Units::ns,
                               -25.0*Gaudi::Units::ns,
                               0.0*Gaudi::Units::ns,
                               25.0*Gaudi::Units::ns,
                               50.0*Gaudi::Units::ns };
  
  declareProperty( "SpillVector"                , m_spillVector                 = tmp1);
  declareProperty( "SpillTimes"                 , m_spillTimes                  = tmp2);  
  declareProperty( "InputLocation"              , m_inputLocation               = LHCb::MCHitLocation::FT, "Path to input MCHits");
  declareProperty( "OutputLocation"             , m_outputLocation              = LHCb::MCFTDepositLocation::Default, 
                   "Path to output MCDeposits");
  declareProperty( "FiberPropagationTime"       , m_fiberPropagationTime        = 6.0 * Gaudi::Units::ns / Gaudi::Units::m, 
                   "light propagation time in fiber");
  declareProperty( "ScintillationDecayTime"     , m_scintillationDecayTime      = 2.8 * Gaudi::Units::ns, 
                   "Scintillation decay time of photons");
  declareProperty( "AttenuationToolName"        , m_attenuationToolName         = "MCFTAttenuationTool"   );
  declareProperty( "UseAttenuation"             , m_useAttenuation              = true );
  
  declareProperty( "PutMCParticlePcut"          , m_putMCParticlePcut           = 0. , "apply lower P cut on MCParticles");

  declareProperty( "SimulateNoise"              , m_simulateNoise               = true, "Simulate noise");
  declareProperty( "KillSignal"                 , m_killSignal                  = false, "Remove signal deposits");
  declareProperty( "ThermalNoiseRate"           , m_thermalNoiseRate            = 7., "Thermal noise rate @ -40 deg.C");
  declareProperty( "ThermalNoiseWindows"        , m_rdwindows                   = 2,  "Thermal noise readout windows of 25ns to simulate");
  declareProperty( "CrossTalkProb"              , m_crossTalkProb               = 0.10, "Cross-talk probability per PE");
  declareProperty( "AfterpulseProb"             , m_afterpulseProb              = 0.02, "Afterpulse probability per PE");
  declareProperty( "PhotoElectronsPerMeV"       , m_photoElectronsPerMeV        = 120. );  // 0.21 MeV per MIP, 25 photoelectrons per MIP

  


}
//=============================================================================
// Destructor
//=============================================================================
MCFTDepositCreator::~MCFTDepositCreator() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDepositCreator::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  }
  
  m_attenuationTool = tool<IMCFTAttenuationTool>( m_attenuationToolName, this );
  if( m_attenuationTool == nullptr ){
    error() << "Could not find: " << m_attenuationToolName << endmsg;
  }
  

  // -- Setup the transmissionMap
  if( m_useAttenuation ){
    sc = m_attenuationTool->initializeTool();
    if( sc.isFailure() ) return Error( "Could not initialize attenuation tool", sc );
  }
  

  // Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  m_numLayers = m_deFT->layers().size();
  
  if(m_deFT->version() >= 20 ){
    // THIS HAS TO BE CHECKED, WHAT HAPPENS IF FIBRE MAT IS EMPTY???
    m_yMax = m_deFT->fibremats()[0]->layerMaxY();
    if ( msgLevel( MSG::DEBUG) ) debug() << "[initialize] RUN NEW GEOMETRY" << endmsg;
  }else{
    error() << "Unknown FTDet version" << endmsg; 
    return  StatusCode::FAILURE;
	}

  // Initialize random generator
  sc = m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0) );
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Flat generator", sc );
  sc = m_rndmLandau.initialize( randSvc(), Rndm::Landau(39.0, 7.8) ); // for noise, from a fit to the
                                                                      // MCHit cluster charge (6bit ADC)
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Landau generator", sc );

  // construct container names once
  for( const std::string spillName : m_spillVector){
    m_spillNames.push_back(  "/Event"+(spillName)+m_inputLocation );
  }
  
  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositCreator::execute() {
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[MCFTDepositCreator] ==> Execute NEW EVENT" << endmsg;
  }

  // define deposits container
  MCFTDeposits *depositCont = new MCFTDeposits();

  // register MCFTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  // Loop over the spills
  // retrieve MCHits and make first list of deposits
  

  int iSpill = 0;
  for( const std::string spillName : m_spillNames ){
    
    MCHits* ftMCHits = getIfExists<MCHits>( spillName );
    
    if (!ftMCHits) {
      if (msgLevel(MSG::DEBUG)) debug() << "Spillover missing in the loop " + spillName << endmsg;
      iSpill++;
      continue;
    }
    
    // found spill - create some digitizations and add them to deposits
    if ( msgLevel( MSG::DEBUG) ) debug() << "ftMCHits->size() : " << ftMCHits->size()<< endmsg;
    
    for( MCHit* ftHit : *ftMCHits ){

      // check if we need to apply a cut on MCParticle properties first
      double MCP = ftHit -> mcParticle() -> p();
      if(m_putMCParticlePcut != 0.) {
        plot( MCP, "MCParticle_P", "MCParticle Momentum (per MCHit); P [GeV/c]; N MCHits", 0., 200., 200 );
        if( MCP < m_putMCParticlePcut ) { 
          plot( MCP, "MCParticle_P_cutout", "MCParticle Momentum (per MCHit) cutout; P [GeV/c]; N MCHits", 0., 200., 200 );
          continue;
        }
      }
      
      counter("NbOfMCHits")++;
      
      // -- Plot some useful quantities
      plot2D( ftHit->entry().x(), ftHit->entry().y(), "HitEntryPosition", "Entry position of hits ; x [mm]; y [mm]",  
              -500., 500., -500.,500., 100, 100 );
      plot(ftHit->energy(),"EnergyOfHit", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 10 );
      plot(ftHit->energy(),"EnergyOfHitZOOM", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 1 );
      // -----------------------------
      
      // -- DEBUG printing
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << "[ HIT ] XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
                << "][" << ftHit->exit ()<< "]\tE="<< ftHit->energy()
                << "\ttime="<< ftHit->time()<< "\tP="<<ftHit->p()
                << "\n...DeltaX="<<std::abs(ftHit->entry().x()-ftHit->exit().x()) 
                << endmsg;
        if(ftHit->mcParticle()){
          debug()  << "... Come from PDGId="<<(ftHit->mcParticle()->particleID())
                   << "\t p="<<(ftHit->mcParticle()->p())
                   << "\t pt="<<(ftHit->mcParticle()->pt())
                   << "\t midPoint.X="<< floor(fabs(ftHit->midPoint().x())/128.)
                   <<" remains="<<((int)fabs(ftHit->midPoint().x()))%128
                   << endmsg; 
        }
      }
      // ----------------
      
      // Get the list of fired FTChannel from the (x,y,z) position of the hit, with, 
      // for each FTChannel, the relative distance to the middle of the cell of the barycentre 
      // of the (entry point, endpoint) system :
      if(m_deFT->version() >= 20 ){
 
        StatusCode sc;
        if( !m_killSignal ) 
          sc = hitToChannelConversion(ftHit,depositCont,iSpill);
        
        if (  sc.isFailure() ){
          error() << " HitToChannelConversion() FAILED" << endmsg; 
          return sc;
        }
      }else{
        error() << "Unknown FTDet version" << endmsg; 
        return  StatusCode::FAILURE;
      }
      
    } // loop on hits
    
    ++iSpill;
    
  } // loop on spills

  
  


  //=========================================================
  // add noise deposits
  if(m_simulateNoise) {
    debug() << " -- STARTING NOISE DEPOSIT CALCULATIONS" << endmsg;
    
    // Get channels containing a deposit from an MCHit
    //FTchanDepMap mapChannels;
    //for (MCFTDeposits::const_iterator iterDeposit = depositCont->begin(); iterDeposit!=depositCont->end();++iterDeposit){
    //  MCFTDeposit* mcDeposit = *iterDeposit;
    //  mapChannels.insert( FTchanDep( mcDeposit -> channelID(), mcDeposit ) );
    //}
  
    std::vector<double> intOffsets = boost::assign::list_of(26*Gaudi::Units::ns)
                                                           (28*Gaudi::Units::ns)
                                                           (30*Gaudi::Units::ns);//  tof (21+2+2)
                                                                        // + full fiber propagation time (15) 
                                                                        // - integration rise (15)
    


    //====== THERMAL NOISE ======
    //
    // Set thermal noise propoerties
    float readoutFreq   =  40. / m_rdwindows; // MHz
    float tempCoef      =  10.; // deg.C for factor 2 reduction
    float startTemp     = -40.; // assumption
    float m_irradiation =   6.; // <-- should be made configurable
    float m_temperature = -40.; // <-- should be made configurable
    float startIrrad    =   6.; // x10^11 neq/cm2
    float noiseProb = (m_thermalNoiseRate/readoutFreq) * (m_irradiation / startIrrad)
      * pow(2, (m_temperature - startTemp)/tempCoef);
    int NthermalNoise = Ntotchannels * noiseProb; // N_PE of noise over total detector

    if ( msgLevel( MSG::DEBUG) )
      debug() << "NthermalNoise = " << NthermalNoise << endmsg;

    // Loop over all noise PEs
    for(int i=0; i<NthermalNoise; i++) {

      // Get random channel and time to add 1 PE to
      int layer    = int( m_flatDist() * Nlayers );
      int module   = int( m_flatDist() * NModule);
      int mat      = int( m_flatDist() * NMat );
      int sipmID   = int( m_flatDist() * Nsipms );
      int sipmCell = int( m_flatDist() * Nchannels);
      FTChannelID noiseChannel(layer, module, mat, sipmID, sipmCell);

      int Npe = 1;
      double Energy = float(Npe) / m_photoElectronsPerMeV; 
      double TOA = m_flatDist() * 25. * m_rdwindows * Gaudi::Units::ns; // we simulate more than 25ns for the time response
      TOA += intOffsets[ noiseChannel.layer() / 4 ] - ((m_rdwindows-1.)*0.25/2.)*Gaudi::Units::ns; // subtract integration time offset per station 

      addNoiseDeposit(noiseChannel, Energy, TOA, 0, 1, depositCont);
      plot2D(noiseChannel.channelID(), TOA, "ThermalNoiseTOA","ThermalNoiseTOA; channelID; TOA [ns]", 0., (float)Ntotchannels*(12./9.), 0., 100, Ntotchannels/Nchannels * (12./9.), 100);
      plot2D(noiseChannel.channelID(), Energy, "ThermalNoiseEnergy","ThermalNoiseEnergy; channelID; Energy [MeV]", 0., (float)Ntotchannels*(12./9.), 0., 0.1, Ntotchannels/Nchannels * (12./9.), 100);

    } // end of loop over noise PEs


    //====== AFTERPULSING ======

    
    // Initialize some numbers for the occupancy distribution
    double N   = 102.;    // these numbers are the result of a fit
    double c   = 393.;    //  to an occupancy profile
    double e1  = 0.00168;
    double e2  = -0.00032;
    double off = 26.;

    double norm = N * exp( e1 * c ) / exp( e2 * c );
    double intBeg = 0.;
    double intEnd = Nchannels*Nsipms;
    double integralBeforCut = off*(c-intBeg) + (N   /e1) * ( exp(e1*c     ) - exp(e1*intBeg) );
    double integralAfterCut = off*(intEnd-c) + (norm/e2) * ( exp(e2*intEnd) - exp(e2*c     ) );
    
    // These are the integrated 25ns bin values of Figure 8 of the support note (afterpulse time distribution).
    // Values used here are trec = 20ns, tshort = 25ns, tlong = 100ns, fshort = 0.5.
    float NsumBinFractions[20] = { 0.204691106081,  0.176326997846,  0.132542585516,  0.0997058439121,  0.0760373252544,
                                   0.0585607741894,  0.0453520643227,  0.0352234776303,  0.0273958443495,  0.0213224364803,  
                                   0.0166009440283,  0.0129269868938,0.010066868574,  0.00783983501193,  0.00610557755609,  
                                   0.00475499468632,  0.0037031811119,  0.00288403576027,  0.00224608761991,  0.001749254176 };
    float NsumBinFracCum[20] = {0.};
    float sum=0.;
    for (int j=0; j<20; j++){
      sum += NsumBinFractions[j];
      NsumBinFracCum[j] = sum;
    }

    int NavgClusInEvent = int(396788./100.); // Based on 100 events, nu=7.6, Bs2phiphi, TDR time, with noise. 
    NavgClusInEvent *= m_rdwindows; // increase amount of noise with readout window

    // loop over all 'clusters in the event'
    int Napcharges[20] = {0};
    for(int i=0; i<NavgClusInEvent; i++) {

      // get Nap for this cluster
      int Npe=100;
      while(Npe > 30) { Npe = (m_rndmLandau()/2.) * m_afterpulseProb; }
      Rndm::Numbers rndmPoisson(randSvc(), Rndm::Poisson( Npe ) );
      int Nap = rndmPoisson();

      // drop each Nap in a time bin
      int Naptimes[21] = {0}; // 1 overflow time bin
      for(int ap=0; ap<Nap; ap++) {
        float r = m_flatDist();
        for (int j=0; j<20; j++) {
          if (r < NsumBinFracCum[j]) {
            Naptimes[j]++;
            plot(j,"hitAfterpulseTimeDist","hitAfterpulseTimeDist; hitAfterpulseTimeDist", 0., 21., 21);
            break;
          }
        }
      }

      // count bins with same number of entries.
      // These will be groups of simultaneously fired pixels (within 25ns) in the same channel. 
      for(int t=0; t<20; t++) {
        Napcharges[ Naptimes[t] ]++;
        plot(Naptimes[t], "hitAfterpulsePE","hitAfterpulsePE; hitAfterpulsePE", 0., 20., 20);
      }
    } // end of looping over all 'clusters in the event'

    // loop over all hit afterpulses
    for(int i=1; i<20; i++) { // skip the 0 count groups
      for( int j=0; j<Napcharges[i]; j++) {

        int Npe = i;

        // create random channel according to occupancy
        // FUNCTION of IDCell occupancy distribution per quarter (fit):
        //if (x <= c) {     y = off + N * exp( e1 * x );   }
        //if (x > c) {    double norm = N * exp( e1 * c ) / exp( e2 * c );     y = off + norm * exp( e2 * x );   }

        int sipmID = -1;
        int sipmCell = -1;
        int IDCell;
        while(sipmID < 0 || sipmID > Nsipms || sipmCell < 0 || sipmCell > Nchannels) { // safety check for rawBankEncoder
          if (m_flatDist() * (integralBeforCut+integralAfterCut) < integralBeforCut) {
            // first expo
            double randRange = (off+N*exp(e1*intBeg)) + m_flatDist()*( (off+N*exp(e1*c)) - (off+N*exp(e1*intBeg)));
            double randExp = log( (randRange-off)/N ) / e1;
            IDCell = int( randExp + 0.5 );
          } else {
            // second expo
            double randRange = (off+norm*exp(e2*intEnd)) + m_flatDist()*( (off+norm*exp(e2*c)) - (off+norm*exp(e2*intEnd)));
            double randExp = log( (randRange-off)/norm ) / e2;
            IDCell = int( randExp + 0.5 );
          }
          int bitShiftsipmID = 7;
          sipmID = (IDCell >> bitShiftsipmID);
          sipmCell = IDCell - sipmID * pow(2,bitShiftsipmID);
        }

        // Now that we have the IDCell (= sipmID and sipmCell, represents 1 quarter), get random quarter:
        double r = m_flatDist()*(Nlayers);
        int layer = int(r);
        r = m_flatDist()*(NModule);
        int module = int(r);
        r = m_flatDist()*(NMat);
        int mat = int(r);
        FTChannelID noiseChannel(layer, module, mat, sipmID, sipmCell);

        double Energy = 1. / m_photoElectronsPerMeV; 
        for(int k=0; k<Npe; k++) {
          // loop over all PE's that will end up in one channel in one window. They still need a random time per PE.
          double TOA = m_flatDist() * 25. * m_rdwindows * Gaudi::Units::ns; // we simulate more than 25ns for the time response
          TOA += intOffsets[ noiseChannel.layer() / 4 ] - ((m_rdwindows-1.)*0.25/2.)*Gaudi::Units::ns; // subtract integration time offset per station 

          addNoiseDeposit(noiseChannel, Energy, TOA, 1, 1, depositCont);
          plot2D(noiseChannel.channelID(), TOA, "AfterPulseTOA","AfterPulseTOA; channelID; TOA [ns]", 0., (float)Ntotchannels * (12./9.), 0., 100., Ntotchannels/Nchannels * (12./9.), 100);
          plot2D(noiseChannel.channelID(), Energy, "AfterPulseEnergy","AfterPulseEnergy; channelID; Energy [MeV]", 0., (float)Ntotchannels * (12./9.), 0., .1, Ntotchannels/Nchannels * (12./9.), 100);
        }
      }
    }


    
    
    //===== END OF NOISE =====
    
  }
  //=========================================================

  return StatusCode::SUCCESS;
}



//=========================================================================
// Convert the hit into channels
//=========================================================================
StatusCode MCFTDepositCreator::hitToChannelConversion(LHCb::MCHit* ftHit, LHCb::MCFTDeposits* depositCont, 
                                                      const unsigned int iSpill) {
  
  const DeFTFibreMat* pL = m_deFT->findFibreMat(ftHit->midPoint());
  FTDoublePairs channels;
  
  if ( msgLevel( MSG::DEBUG) )
    debug() << "[HitToChannelConversion] RUN NEW GEOMETRY" << endmsg;


  if ( pL == nullptr) {
    if ( msgLevel( MSG::DEBUG) ) debug() << "Could not find FibreMat corresponding to MCHit with mid point " << ftHit->midPoint().X() << " " << 
                                   ftHit->midPoint().Y() << " " << ftHit->midPoint().Z() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  // -- If this fails, it's mostly due to geometrical reasons (in FTDet v2.0)
  // -- so not dangerous
  if( pL->calculateListOfFiredChannels( ftHit, channels) == StatusCode::FAILURE){
    if ( msgLevel( MSG::DEBUG) ) debug() << "Could not calculate list of fired channels" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  counter("NbOfHitInActiveArea")++;     
  counter("NbOfFiredChannels") += channels.size();

  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "--- Hit index: " << ftHit->index() 
            << ", size of vector of channels: " << channels.size() << endmsg;
  }
      
  plotChannelProperties(pL, channels, ftHit);
  
  double att    = 1;
  double attRef = 1;
  
  // -- Calculate the attenuation using the attenuation tool
  if( m_useAttenuation) m_attenuationTool->attenuation(ftHit, att, attRef);
  
  
  // -- Calculate scintillation light release time
  // -- Here could use a fast log, precision should not matter that much
  const double releaseTime = -log( m_flatDist() ) * m_scintillationDecayTime; 
  
  // -- Calculate the arrival time
  const double timeToSiPM    = calcTimeToSiPM(   ftHit, releaseTime, iSpill);
  const double timeRefToSiPM = calcTimeRefToSiPM(ftHit, releaseTime, iSpill);
  
  
  if ( msgLevel( MSG::DEBUG) ){
    debug()  << "[Pulse Arrival Time] Hit(y)=" << fabs(ftHit->midPoint().y())
             << " DirectPulseArrTime="<< timeToSiPM 
             << " ReflectedPulseArrTime="<< timeRefToSiPM << endmsg;
  }
  
  
  // -- Fill MCFTDeposit 
  FTDoublePair negEnergyPair;
  // Temporary Patch [Eric 14/05/2014 : remove hits whose fired channels have negative energy!!]
  bool positiveEnergy = true;
  for( FTDoublePair ftPair : channels){
    if (ftPair.second < 0 ) {
      positiveEnergy = false;
      negEnergyPair = ftPair;
      counter("NegativeEnergyLayer")++;
    }
  }

  // -- End Temporary Patch
  if(positiveEnergy){

    for( FTDoublePair ftPair : channels ){
      double directEnergyInSiPM    = ftPair.second * att;
      double reflectedEnergyInSiPM = ftPair.second * attRef;
      
      
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "[FTCHANNEL] FTChannel=" << ftPair.first 
                 << " DirectEnergyHitFraction="<< directEnergyInSiPM 
                 << " ReflectedEnergyHitFraction="<< reflectedEnergyInSiPM << endmsg;
      }
      
      // -- add XTalk
      int totalXtalk = 0;
      if(m_simulateNoise) {
        auto xtalkDirect = addXtalk( directEnergyInSiPM );
        auto xtalkReflected = addXtalk( reflectedEnergyInSiPM );
        directEnergyInSiPM = xtalkDirect.second;
        reflectedEnergyInSiPM = xtalkReflected.second;
        totalXtalk = xtalkDirect.first + xtalkReflected.first;
      }
      
      
      // -- plotting stuff
      plot(ftPair.second, "EnergyDepositedInCell",
           "EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 2.);    
      plot(ftPair.second, "EnergyDepositedInCellZOOM",
           "EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 1.);        
      plot(directEnergyInSiPM,"DirectEnergyRecordedInCell",
           "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
      plot(directEnergyInSiPM,"DirectEnergyRecordedInCellZOOM",
           "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
      plot(reflectedEnergyInSiPM,"ReflectedEnergyRecordedInCell",
           "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
      plot(reflectedEnergyInSiPM,"ReflectedEnergyRecordedInCellZOOM",
           "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
      // ---------------------------------------------------------------------------------------

      
      
      // -- if reference to the channelID already exists, just add DepositedEnergy and arrival time
      LHCb::MCFTDeposit* deposit = depositCont->object(ftPair.first);
      if( deposit != nullptr ){
        deposit->addMCHit(ftHit, directEnergyInSiPM, reflectedEnergyInSiPM, timeToSiPM, timeRefToSiPM);
        deposit->addNoise(totalXtalk, 2);
        counter("NbOfAppendDeposit")++;
      } else {
        if ( ftPair.first.layer() < m_numLayers ) { // Set to 12 instead 15 because of a bug in new geometry
          // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
          MCFTDeposit* energyDeposit = new MCFTDeposit(ftPair.first,ftHit,directEnergyInSiPM, 
                                                       reflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
          energyDeposit->addNoise(totalXtalk, 2);
          depositCont->insert(energyDeposit, ftPair.first);
          counter("NbOfCreatedDeposit")++; 
        }else if ( ftPair.first.layer() >= m_numLayers ){
          counter("NbOfGhostDeposit")++;
          if ( msgLevel( MSG::DEBUG) ) debug() << "hit is not within range of physical layers" << endmsg;
        }
      }
      plot2D(ftPair.first.channelID(), timeToSiPM, "directTOA","directTOA; channelID; TOA [ns]", 0., (float)Ntotchannels*(12./9.), 0., 100., Ntotchannels/Nchannels*(12./9.), 100);
      plot2D(ftPair.first.channelID(), timeRefToSiPM, "reflectedTOA","reflectedTOA; channelID; TOA [ns]", 0., (float)Ntotchannels*(12./9.), 0., 100., Ntotchannels/Nchannels*(12./9.), 100);
    }
  }else{
    
    plot(negEnergyPair.first.layer(),"CheckNegativeEnergyLayer",
         "CheckNegativeEnergyLayer; Layer of negative energy hit; Nber of Channels" ,0.,20.,20); 
    
  }
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// plot stuff
//=========================================================================
void MCFTDepositCreator::plotChannelProperties(const DeFTFibreMat* pL, FTDoublePairs channels, const MCHit* ftHit){
  
  // -- PRINTING -------------------------------------------------------------------------------------------------
  plot(pL->angle()*180/M_PI,"LayerStereoAngle","Layer Stereo Angle;Layer Stereo Angle [#degree];" ,-10 , 10);
  plot(pL->layerInnerHoleRadius(),"LayerHoleRadius","Layer Hole Radius ; Hole Radius  [mm];" ,50 , 150);
  plot(pL->layerMaxY(),"LayerHalfSizeY","Layer Half Size Y ; Layer Half Size Y  [mm];" ,0 , 3500);
  
  plot(channels.size(),"CheckNbChannel", 
       "Number of fired channels per Hit;Number of fired channels;Number of hits", 
       0. , 50., 50);
  
  
  const int NbOfFiredFibres = std::floor(std::abs((ftHit->entry().x()-ftHit->exit().x())*cos(pL->angle()) 
                                                  + (ftHit->entry().y()-ftHit->exit().y())*sin(pL->angle()))/.25+1
                                         );
  
  plot2D(channels.size(),NbOfFiredFibres,"NbChannelvsNbFiredFibres", 
         "Number of fired channels per Hit vs deposit length; Number of fired channels;Deposit Length (in channel units)", 
         0 , 50, 0, 50, 50, 50);
  
  plot2D(channels.size(),NbOfFiredFibres,"NbChannelvsNbFiredFibresZOOM", 
         "Number of fired channels per Hit vs deposit length; Number of fired channels;Deposit Length (in channel units)", 
         0 , 10, 0, 10, 10, 10);
  
  
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "--- Hit index: " << ftHit->index() 
            << ", size of vector of channels: " << channels.size() << endmsg;
    debug() << "[ HIT ] XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
            << "][" << ftHit->exit ()<< "]\tE="<< ftHit->energy()
            << "\ttime="<< ftHit->time()<< "\tP="<<ftHit->p()
            << "\n...DeltaX="<<std::abs(ftHit->entry().x()-ftHit->exit().x()) 
            << " pL->angle()="<<pL->angle() << " DX=" << (ftHit->entry().x()-ftHit->exit().x()) 
            << " DY=" << (ftHit->entry().y()-ftHit->exit().y())
            << "==> #Channels="<<NbOfFiredFibres 
            << "+2 (light sharing) + 1 (positioning)"<< endmsg;
  }
  // -----------------------------------------------------------------------------------------------------
  
  double fibrelength = 0;
  double fibrelengthfraction = 0;
  if(pL->hitPositionInFibre(ftHit, fibrelength,fibrelengthfraction)){
        
    
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << format( "fibrelength=%10.3f fibrelengthfraction=%10.3f \n",fibrelength,fibrelengthfraction);
    }
    
    plot(fibrelength,"FibreLength","FibreLength; Sci. Fibre Lengh [mm]; Nber of Events" ,2000 ,3000);
    plot(fibrelengthfraction,"FibreFraction","FibreFraction; Sci. Fibre Fraction ; Nber of Events" ,0 ,1);
  }
  // ----------------------------------------

}

//double MCFTDepositCreator::Pafterpulse(double t) {
//  double t_rec   =  20. ;
//  double t_short =  25. ;
//  double t_long  = 100. ;
//  double f_short =   0.5;
//  double Norm = 1./48.2742;
//  double Pap = Norm * (1 - exp(t/t_rec)) * ( f_short * exp(t/t_short) + (1-f_short) * exp(t/t_long) );
//  return Pap;
//}

void MCFTDepositCreator::addNoiseDeposit(LHCb::FTChannelID noiseChannel, double Energy, double TOA, int type, int npe, LHCb::MCFTDeposits* depositCont) {
      
      LHCb::MCHit* dummyMCHit = 0; 

      // -- add XTalk
      std::pair<int,double> xtalkPair = addXtalk( Energy );
      Energy = xtalkPair.second;
    
      plot2D(Energy,TOA, "NoiseDepositContributions","NoiseDepositContributions;Energy [MeV];Time [ns]",0.,0.1, 0., 100., 100,100);
      
      // -- if reference to the channelID already exists, just add DepositedEnergy and arrival time
      LHCb::MCFTDeposit* deposit = depositCont->object( noiseChannel );
      if( deposit != nullptr ){
        deposit->addMCHit( dummyMCHit, Energy, 0., TOA, 0.);
        deposit -> addNoise(type,npe);
        deposit -> addNoise(2, xtalkPair.first);
      } else {
        MCFTDeposit* noiseDeposit = new MCFTDeposit( noiseChannel, dummyMCHit, Energy, 0., TOA, 0. ); 
        noiseDeposit -> addNoise(type,npe);
        noiseDeposit -> addNoise(2, xtalkPair.first);
        depositCont->insert(noiseDeposit, noiseChannel);
      }
}

std::pair<int,double> MCFTDepositCreator::addXtalk(double Energy) {
  float Npe = Energy * m_photoElectronsPerMeV;
  float mean = Npe * m_crossTalkProb;
  Rndm::Numbers rndmPoisson(randSvc(), Rndm::Poisson( mean ) );
  int Nxtalk = rndmPoisson();
  float EnergyAdd = Nxtalk / m_photoElectronsPerMeV;
  plot(Nxtalk, "NXtalk","NXtalk; PE;",0.,30.,30);
  plot2D(Energy, Energy+EnergyAdd, "xTalkEnergy","xTalkEnergy; Energy; Energy+EnergyAdd [MeV]", 0., .5, 0., .5, 200, 200);
  std::pair<int,double> retval = std::make_pair(Nxtalk,Energy + EnergyAdd);
  return retval;
}


