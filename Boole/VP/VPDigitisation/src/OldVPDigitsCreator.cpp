// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
 
// local
#include "OldVPDigitsCreator.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : OldVPDigitsCreator
//
// 2010-07-07 : Thomas Britton
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OldVPDigitsCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OldVPDigitsCreator::OldVPDigitsCreator( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_buffer(NULL)
{
  std::vector<std::string> tmp =
    boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/");
  declareProperty("SpillVector", m_spillNames = tmp);
  declareProperty("InputLocation", m_inputLocation =  "VP/PreDigits");
  declareProperty("OutputLocation", m_outputLocation ="/Event/"+LHCb::VPDigitLocation::VPDigitLocation );

  // discrimination threshold in electrons
  declareProperty("ChargeThreshold", m_Qthresh = 1000.0);  // Si thickness = 150um

  // approaximate value of minimum ionizing charge in the sensor (in electrons)
  declareProperty("MIPCharge", m_chargeMIP = 11000.0); 

  // number of bits for ToT
  declareProperty("NumberOfBits",m_nBits = 4); // 4 bits
  
  // bunch spacing in ns 
  declareProperty("BunchCrossingSpacing", m_bunchCrossingSpacing = 25.0 );

  // signal shape parameters
  //     peak time relative to bunch crossing time (related to time walk effects) 
  declareProperty("PeakTime",m_peakTime=200.0 ); //ns
  //     decay time since the peak value (related to deadtime and spillover effects) 0=not simulated!
  declareProperty("DecayTime",m_decayTime=1250.0 );//ns

  // maximal time walk allowed (in ns)  
  //      equal to zero -> don't pay attention to time walk
  declareProperty("MaximalTimeWalk",m_maxTimeWalkAllowed=0.0 );

  // electronic noise in electrons
  declareProperty("ElectronicNoise",m_ElectronicNoise = 100.0 );  


}
//=============================================================================
// Destructor
//=============================================================================
OldVPDigitsCreator::~OldVPDigitsCreator() {}
//=============================================================================
// Initialization
//=============================================================================
StatusCode OldVPDigitsCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_spillPaths.clear();
    // Load spill paths
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }

  m_bunchCrossingNumber =0; // initial value of the BX counter

  // maximal ToT value (saturation); 
  m_maxToT = ( 2 << (m_nBits-1) ) - 1;
  // conversion factor (assuming saturation charge set at 2.5 MIP charge)
  m_conversion = m_maxToT/(2.5*m_chargeMIP);
  

  // start time for MIP charge (in ns) relative to the bunch crossing time
  m_startTimeMIP = m_peakTime * m_Qthresh / m_chargeMIP;
  // earliest signal accepted for this bunch crossing (corresponds to walk of charge= +infinity)
  m_minTimeWalkAllowed = - m_startTimeMIP;

  m_numberOfFutureBunchX = int( m_maxTimeWalkAllowed/m_bunchCrossingSpacing );

  m_buffer = new VPDigits();

  // parameters of charge non-linearlity
  m_b1=-10869.0;
  m_c1=1019450038.5;
  m_t=120552.0; 
  double b=m_b1-m_t;
  double zero = -0.5*(b+sqrt(b*b+4.0*(m_b1*m_t+m_c1)));  
  m_scale = zero/m_Qthresh;

  if( m_ElectronicNoise > 0.1 ){    
    StatusCode scG = m_gaussDist.initialize(randSvc(),Rndm::Gauss(0.0,1.0));
    if( !scG ) {
      error() << "Random number initialisation failure" << endmsg;
      return (StatusCode::FAILURE);
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OldVPDigitsCreator::execute() {
  
  debug() << "==> Execute" << endmsg;
  // printf("OldVPDigitsCreator::execute() =>\n");

  // ================================
  // add Previous (or Next) bunch crossing hits (min.bias) to the buffer with prev. bunch crossing info
  // ================================

  //   don't simulate if decay time is essentially zero 
  if( m_decayTime > 0.5 ){
    

    for( unsigned int i=1; i<m_spillPaths.size(); ++i){
      VPDigits* container = get<VPDigits>(m_spillPaths[i]);

      // increment global bunch crossing number for previous bunch crossing hits
      m_bunchCrossingNumber++;
     
      VPDigits::const_iterator j=container->begin();      
      while(j != container->end()) {

        VPDigit & thisHit = **j;

        // do we have hits for this pixels already in our buffer ?
        VPDigit* check= m_buffer->object(thisHit.key());
        if( check ) {
          // add to the previous hit and rest its clock
          double timeEff = m_peakTime + convertToTime( m_bunchCrossingNumber - check->timeStamp() );
          double oldChargeNow = pulseShape( timeEff ) * check->ToTValue();        
          check->setToTValue(int( oldChargeNow + thisHit.ToTValue() + 0.5 ));
          check->setTimeStamp(m_bunchCrossingNumber);
        } else {
          VPDigit* toBeAdded = thisHit.clone();
          toBeAdded->setTimeStamp(m_bunchCrossingNumber);
          m_buffer->insert(toBeAdded, thisHit.channelID());
        }
        j++;
      }
    } 
    
  }
  

  // ================================
  // clean the buffer with previous bunch crossing info to drop hits which expired 
  // ================================

  // bunch crossing number of the current bunch crossing
  int bunchCrossingNumberNow =  m_bunchCrossingNumber + 1 - m_numberOfFutureBunchX;
  

  //   don't simulate if decay time is essentially zero 
  if( m_decayTime > 0.5 ){

    VPDigits* tempBuffer= new VPDigits();
    VPDigits::const_iterator j=m_buffer->begin();
    while(j != m_buffer->end()) {
      VPDigit & thisHit = **j;
      double timeEff = m_peakTime + convertToTime( bunchCrossingNumberNow - thisHit.timeStamp() );
      //  keep pulses which did not decayed completely
      if(  pulseShape( timeEff  ) > 0.0  ){
        VPDigit* toBeAdded = thisHit.clone();
        tempBuffer->insert(toBeAdded,thisHit.channelID());
      }
      j++;
    }  
    m_buffer->clear();
    delete m_buffer;
    m_buffer=tempBuffer;
    
  }
  

  // ================================
  // Process Current bunch crossing container (signal)
  // ================================

  // output container
  LHCb::VPDigits* digitizedCont= new LHCb::VPDigits();
 
  // current bunch crossing container
  VPDigits* container = get<VPDigits>(m_spillPaths[0]);

  for( VPDigits::const_iterator j=container->begin();j!= container->end(); ++j) {

    VPDigit & thisHit = **j;    

    double thisCharge = thisHit.ToTValue();
    if( m_ElectronicNoise > 0.1 ){
      thisCharge += m_ElectronicNoise * m_gaussDist();
      thisCharge = (thisCharge < 1.0 )?1.0:thisCharge;
    }
    double thisStartTime; 
    if( thisCharge >= m_Qthresh ){
      thisStartTime = m_peakTime * m_Qthresh / thisCharge;
    } else {
      thisStartTime = 1.0E6; // +infty
    }
    
    // see if there was a hit in the same pixel in the previous bunch crossings
    if( m_decayTime > 0.5 ){    
      VPDigit* check= m_buffer->object(thisHit.key());
      if( check ){
        VPDigit & oldHit = *check;
        // time elapsed since the old bunch crossing
        double timeElapsed = convertToTime( bunchCrossingNumberNow - oldHit.timeStamp() );
        if( timeElapsed <=0.0 )timeElapsed -= m_numberOfFutureBunchX ? m_bunchCrossingSpacing : 0;        
        // see if charge was over the threshold
        double oldCharge = oldHit.ToTValue();
        if( oldCharge  >= m_Qthresh ){
          // start time of the old hit 
          double oldStartTime = m_peakTime * m_Qthresh / oldCharge - timeElapsed;
          if( oldStartTime < thisStartTime ){
            // chip was counting before this hit arrived; 
            if( pulseShape( timeElapsed ) * oldCharge > m_Qthresh ){
              // and is still busy calculating ToT
              if( m_maxTimeWalkAllowed > 0.1 ){              
                thisStartTime = oldStartTime;
                double timeWalk = thisStartTime - m_startTimeMIP;    
                // kill the pixel if old start time was out of accepted range
                if( timeWalk < m_minTimeWalkAllowed )continue;
                if( timeWalk > m_maxTimeWalkAllowed )continue;
                // accept this pixel but with the old start time
              } else {
                // no simulation of accepted time walk range; kill the pixel since it was counting before
                continue;
              }
            }
          }
        }
        // need to calculate decrease of the old charge due to time elapsed
        double timeEff = m_peakTime + timeElapsed;
        double oldChargeNow = pulseShape( timeEff ) * oldCharge;        
        // add it to the current charge
        thisCharge += oldChargeNow;
      }
    }
    
    
    // skip hits if below the threshold
    if( thisCharge < m_Qthresh ) continue;

    // calculate start time if not done already 
    if( thisStartTime == 1.0E6 ){
      thisStartTime = m_peakTime * m_Qthresh / thisCharge;
    }
    
    double timeWalk = thisStartTime - m_startTimeMIP;    
    if( m_maxTimeWalkAllowed > 0.1 ){              
      // kill the pixel if start time was out of accepted range
      if( timeWalk < m_minTimeWalkAllowed )continue;
      if( timeWalk > m_maxTimeWalkAllowed )continue;
    }

    // electrons -> ToT  convertion
    int tot = convertToTDC(nonLinearModel(thisCharge));

    const VPChannelID aChan = thisHit.channelID();
    // printf(" %9.1f e => %2d [%02d:%c, %02d:%03dx%03d]\n", thisCharge, tot,
    //        aChan.station(), aChan.sidepos()?'R':'L',
    //        aChan.chip(), aChan.pixel_lp(), aChan.pixel_hp() );

    LHCb::VPDigit* newDigit =
      new LHCb::VPDigit(tot,int(timeWalk)); 
    digitizedCont->insert(newDigit,thisHit.channelID());

  }

  // ==========================
  // add old/future hits that walked into present bunch crossing
  // ==========================
  if( ( m_maxTimeWalkAllowed > 0.1 ) && ( m_decayTime > 0.5 ) ){
    for( VPDigits::const_iterator j=m_buffer->begin();j!= m_buffer->end(); ++j) {

      VPDigit & thisHit = **j;    

      // skip if this pixel was present in current bunch crossing (already processed)
      VPDigit* check= container->object(thisHit.key());
      if( check ) continue;

      // above the threshold ?
      double thisCharge = thisHit.ToTValue();
      if( m_ElectronicNoise > 0.1 ){    
        thisCharge += m_ElectronicNoise * m_gaussDist();
        thisCharge = (thisCharge < 1.0 )?1.0:thisCharge;
      }
      if( thisCharge < m_Qthresh )continue;

      // time walk
      double timeElapsed = convertToTime( bunchCrossingNumberNow - thisHit.timeStamp() );
      if( timeElapsed <=0.0 )timeElapsed -= m_numberOfFutureBunchX ? m_bunchCrossingSpacing : 0;   
      double thisStartTime = m_peakTime * m_Qthresh / thisCharge - timeElapsed;      
      double timeWalk = thisStartTime - m_startTimeMIP;

      // kill the pixel if start time was out of accepted range
      if( timeWalk < m_minTimeWalkAllowed )continue;
      if( timeWalk > m_maxTimeWalkAllowed )continue;

      // electrons -> ToT  convertion
      int tot = convertToTDC(nonLinearModel(thisCharge));

      const VPChannelID aChan = thisHit.channelID();
      // printf(" %9.1f e => %2d [%02d:%c%c, %02d:%03dx%03d]\n", thisCharge, tot,
      //       aChan.station(), aChan.zpos()?'B':'F', aChan.sidepos()?'R':'L',
      //       aChan.chip(), aChan.pixel_lp(), aChan.pixel_hp() ); 

      LHCb::VPDigit* newDigit =
        new LHCb::VPDigit(tot,int(timeWalk)); 
      digitizedCont->insert(newDigit,thisHit.channelID());
    }
  }

  put( digitizedCont, m_outputLocation); 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OldVPDigitsCreator::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_buffer->clear();
  delete m_buffer;
  return GaudiAlgorithm::finalize();  // must be called after all other actions

}

// pulse shape with max normalized to 1.0
double OldVPDigitsCreator::pulseShape(double time) {
  if( time < 0.0 )return 0.0;
  if( time <= m_peakTime )return (time/m_peakTime);
  if( time < ( m_peakTime+m_decayTime ) )return (1.0 - (time-m_peakTime)/m_decayTime);
  return 0.0;
}

// from bunch crossing number difference to time
double OldVPDigitsCreator::convertToTime(int bunchCrossingNumberDifference) {
  return (m_bunchCrossingSpacing*bunchCrossingNumberDifference);  
}

// linear conversion from charge in electrons to ToT counts
int OldVPDigitsCreator::convertToTDC(double charge) {
  int tot = int(ceil(charge * m_conversion));
  if( tot > m_maxToT ) tot = m_maxToT;
  if( tot <  1 )tot = 1;  
  return tot;
}

// non-linearity in charge digitazation
double OldVPDigitsCreator::nonLinearModel(double charge){
  double q = charge*m_scale;  
  return (q + m_b1 - m_c1/(q-m_t))/m_scale;  
}

//=============================================================================
