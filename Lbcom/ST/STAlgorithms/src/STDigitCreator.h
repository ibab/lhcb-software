// $Id: STDigitCreator.h,v 1.3 2006-02-10 16:32:30 mneedham Exp $
#ifndef _STDigitCreator_H_
#define _STDigitCreator_H_


/** @class STDigitCreator STDigitCreator.h STAlgorithms/STDigitCreator
 *
 *  Class for filling ST summary info
 *
 *  @author M.Needham
 *  @date   10/03/2002
 */

#include <string>
#include <vector>
#include <utility>
#include <functional>

#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

class ISTSignalToNoiseTool;
class IITEffCalculator;
class DeSTDetector;
class DeSTSector;

class STDigitCreator : public GaudiAlgorithm {

public:
  // Constructor: A constructor of this form must be provided.
  STDigitCreator(const std::string& name, 
                 ISvcLocator* pSvcLocator); 
  virtual ~STDigitCreator(); 

  StatusCode initialize();
  StatusCode execute();
  
private:

  typedef std::pair<double,LHCb::STChannelID> digitPair;


  std::string m_detType;
  DeSTDetector* m_tracker;

  void initCache(LHCb::MCSTDigits* digits);
  void genRanNoiseStrips(std::vector<digitPair>& noiseCont) const;
  void createDigits(LHCb::MCSTDigits* mcDigitCont,LHCb::STDigits* digitCont);
  void mergeContainers(const std::vector<digitPair>& noiseCont, 
		       LHCb::STDigits* digitsCont);
  LHCb::MCSTDigit* findDigit(const LHCb::STChannelID& aChan);
 
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  double m_fracOfNoiseStrips;
  double m_tailStart;

  std::string m_inputLocation; 
  std::string m_outputLocation;
  
  std::string m_effToolName;
  ISTEffCalculator* m_effTool;

  LHCb::MCSTDigits::iterator m_cachedIter;
  LHCb::MCSTDigits::iterator m_lastIter;

  double m_saturation;

  unsigned int m_numNoiseStrips;  

  // smart interface to generators
  SmartIF<IRndmGen> m_uniformDist; 
  SmartIF<IRndmGen> m_gaussDist; 
  SmartIF<IRndmGen> m_gaussTailDist; 

  class Less_by_Channel
    : public std::binary_function<digitPair,digitPair,bool> 
  {
  public:

    /** compare the channel of one object with the 
     *  channel of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() (digitPair obj1 ,digitPair obj2 ) const 
    { 
      return obj1.second < obj2.second ; 
    }
    ///
  };

};

#endif // STDigitCreator
