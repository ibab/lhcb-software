// $Id: $
#ifndef OLDVPDIGITSCREATOR_H 
#define OLDVPDIGITSCREATOR_H 1

// Include files
#include <string>
#include "boost/assign/list_of.hpp"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// Event
#include "Event/VPDigit.h"

using namespace LHCb;

/** @class OldVPDigitsCreator OldVPDigitsCreator.h
 *  
 *
 *  @author Thomas Britton
 *  @date   2010-07-07
 */
class OldVPDigitsCreator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  OldVPDigitsCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OldVPDigitsCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  double pulseShape(double t); // pulse shape normalized to max=1.0

  double convertToTime(int bunchCrossingNumberDifference);  

  int convertToTDC(double charge); // linear convertion to ToT
  double nonLinearModel(double charge); // non-linear correction
  
  
protected:

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;
  std::string m_outputLocation;

  // global bunch crossing counter
  int m_bunchCrossingNumber;

  // digit container to store hits from previous bunch crossings (timeStamp = bunchCrossing number)
  VPDigits* m_buffer;


  // bunch crossing spacing in ns
  double m_bunchCrossingSpacing;  

  // work in triangular pulse shape approaximation:
  //    signal increases linearly from zero to max in m_peakTime (in ns) 
  double m_peakTime; 
  //    it then decreases linearly from max to zero in m_decayTime (in ns)
  double m_decayTime;

  // discrimination threshold on number of electrons
  double m_Qthresh;
  
  // minimum ionizing charge in number of electrons (determines saturation value and references time walk)
  double m_chargeMIP;
  
  // start time (in ns) of ToT counter for MIP charge with respect to bunch crossing time
  double m_startTimeMIP;
  
  // maximal time walk allowed (in ns)  0=don't pay attention to time walk
  double m_maxTimeWalkAllowed;
  double m_minTimeWalkAllowed;

  // number of future bunch crossings to keep in the buffer
  int m_numberOfFutureBunchX;

  // number of bits to use for ToT
  int m_nBits;

  // maximal ToT value
  int m_maxToT;

  // conversion factor from electrons to ToT counts
  double m_conversion;

  // non-linearlity function   
  double m_b1;
  double m_c1;
  double m_t;
  double m_scale;  

  // gaussian random number generator for noise
  Rndm::Numbers m_gaussDist;
  // noise in electrons (0=don't simulate)
  double m_ElectronicNoise;
  
  
};
#endif // OLDVPDIGITSCREATOR_H
