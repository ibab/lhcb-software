// $Id: OTRandomDepositCreator.h,v 1.6 2006-01-20 12:57:05 janos Exp $
#ifndef OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 
#define OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"
#include "OTSimulation/IOTRandomDepositCreator.h"

#include <vector>
#include <string>

// Forward declarations
namespace LHCb 
{
  class MCOTDeposit;
}
class DeOTDetector;

/** @class OTRandomDepositCreator OTRandomDepositCreator.h \
 *         "OTSimulation/OTRandomDepositCreator.h"
 *  
 *  Outer tracker random deposit generator.
 *
 *  @author M Needham
 *  @date   28/02/2003
 */

class OTRandomDepositCreator : public GaudiTool, 
                               virtual public IOTRandomDepositCreator {

public:

  /// constructer
  OTRandomDepositCreator(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  /// destructer
  virtual ~OTRandomDepositCreator();

  /// initialize method
  virtual StatusCode initialize();

  ///create method
  virtual StatusCode createDeposits(MCOTDepositVec* depVector) const; 

  /// determine number to generate
  unsigned int nNoiseHits() const;

  /// determine number of OT channels
  unsigned int nChannels() const;

private:

  DeOTDetector* m_tracker;               ///< pointer to XML geometry
  SmartIF<IRndmGen> m_genDist;           ///< random number generator
  double m_windowSize;                   ///< readout window size
  double m_deadTime;                     ///< dead time of a OT channel
  double m_noiseRate;                    ///< noise rate per time-unit
  unsigned int m_nNoise;                 ///< number of noise deposits per event
  std::string m_readoutWindowToolName;   ///< tool read-out window
  std::vector<double> m_windowOffSet;    ///< start of readout window 
  unsigned int m_nMaxChanInModule;       ///< Max number of channels in module
  
};
#endif // OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 
