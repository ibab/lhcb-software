// $Id: STDigitCreator.h,v 1.4 2010-04-07 09:27:38 mneedham Exp $
#ifndef STDigitCreator_H
#define STDigitCreator_H 1

// Gaudi
#include "Kernel/STAlgBase.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// GSL
#include "gsl/gsl_math.h"

// Event
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "LHCbMath/LHCbMath.h"


class DeSTDetector;
class DeSTSector;
class ISTPedestalSimTool;
class ISTCMSimTool;

/** @class STDigitCreator STDigitCreator.h STAlgorithms/STDigitCreator
 *
 *  Algorithm for creating STDigits from MCSTDigits. The following processes
 *  are simulated:
 *  - Apply readout inefficiency/dead channels by masking MCSTDigits
 *  - Add gaussian noise to total charge of signal digits.
 *  - Generate flat noise over all readout sectors. The ADC value follows a \
 *    gaussian tail distribution.
 *  - Merge the signal and noise digits in one container.
 *  - Add neighbours to each digit with an ADC value of a gaussian core noise.
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   05/01/2006
 */

class STDigitCreator : public ST::AlgBase {

public:
  // Constructor
  STDigitCreator( const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~STDigitCreator(); 

  StatusCode initialize();

  StatusCode execute();
  
private:

  typedef std::pair<double,LHCb::STChannelID> digitPair;

  void genRanNoiseStrips(std::vector<digitPair>& noiseCont) const;
  void createDigits(const LHCb::MCSTDigits* mcDigitCont,LHCb::STDigits* digitCont);
  void mergeContainers(const std::vector<digitPair>& noiseCont, 
                       LHCb::STDigits* digitsCont);
  void addNeighbours(LHCb::STDigits* digitsCont) const;
  double genInverseTail() const;
  LHCb::STDigit* findDigit(const LHCb::STChannelID& aChan);
  double adcValue( double value ) const;
 
  // smart interface to generators
  SmartIF<IRndmGen> m_uniformDist; 
  SmartIF<IRndmGen> m_gaussDist; 
  SmartIF<IRndmGen> m_gaussTailDist; 

  LHCb::STDigits::iterator m_cachedIter;
  LHCb::STDigits::iterator m_lastIter;
  unsigned int m_numNoiseStrips;  

  std::string m_inputLocation; 
  std::string m_outputLocation;
  double m_tailStart;
  double m_saturation;
  
  bool m_allStrips;
  bool m_useStatusConditions; ///< use dead strip info

  bool m_addPedestal;
  bool m_addCommonMode;
  std::string m_pedestalToolName;
  ISTPedestalSimTool* m_pedestalTool;
  std::string m_cmToolName;
  ISTCMSimTool* m_cmTool;


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

inline double STDigitCreator::adcValue( double value ) const
{
  return GSL_MIN(LHCb::Math::round(value), m_saturation);
  //return GSL_MIN(int(value), m_saturation);

}

#endif // STDigitCreator_H
