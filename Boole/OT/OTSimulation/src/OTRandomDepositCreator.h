// $Id: OTRandomDepositCreator.h,v 1.11 2008-05-28 20:10:34 janos Exp $
#ifndef OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 
#define OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "IOTRandomDepositCreator.h"

#include <vector>
#include <string>

// Forward declarations
class DeOTDetector;
class DeOTModule;

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
  void createDeposits(OTDeposits& deposits) const;

private:
  IRndmGenSvc* randSvc() const;
  

  mutable IRndmGenSvc*     m_rgs;
  mutable Rndm::Numbers    m_flat;
  DeOTDetector*            m_tracker;               ///< Pointer to tracker
  std::vector<DeOTModule*> m_modules;               ///< Vector of modules
  unsigned int             m_nModules;              ///< Number of modules
  unsigned int             m_nMaxChanInModule;      ///< Maximum possible number of channels in a module 
  unsigned int             m_nChannels;             ///< Total number of channels
  double                   m_windowSize;            ///< Readout window size
  double                   m_deadTime;              ///< Dead time of a OT channel
  double                   m_noiseRate;             ///< noise rate per time-unit
  unsigned int             m_nNoise;                ///< number of noise deposits per event
  std::string              m_readoutWindowToolName; ///< tool read-out window
  std::vector<double>      m_windowOffSet;          ///< start of readout window
};

#endif // OTSIMULATION_OTRANDOMDEPOSITCREATOR_H 
