// $Id: $
#ifndef OTSIMULATION_MCOTDEPOSITCREATOR_H
#define OTSIMULATION_MCOTDEPOSITCREATOR_H 1

// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IOTEffCalculator;
class IOTRandomDepositCreator;
class IOTDoublePulseTool;
class DeOTDetector;

namespace LHCb
{
  class MCOTDeposit;
}

/** @class MCOTDepositCreator MCOTDepositCreator.h 
 *        
 *  Top level algorithm that makes MCOTDeposits from MCHits.
 *   @li find hits and calculate distance to closest wire
 *   @li apply single cell efficiency (function of distance).
 *   @li smear the time with a Gaussian
 *   @li apply r-t relation
 *   @li add propagation time along wire
 *   @li add cross talk
 *   @li add random noise
 *
 *  @author M.Needham
 *  @author adapted to the Event Model by: J. van Tilburg 
 *          jtilburg@nikhef.nl (03-04-2002)
 *  @date   21/10/2000
 */

class MCOTDepositCreator : public GaudiAlgorithm {

public:
 
  /// constructor
  MCOTDepositCreator(const std::string& name, 
		     ISvcLocator *svcloc );
  
  /// destructor
  virtual ~MCOTDepositCreator();

  /// intialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  /// Make initial vector of deposits from MCHits
  void makeDeposits() const;

  /// apply cross talk
  void addCrossTalk() const;

  /// apply pulse reflections
  void addDoublePulse() const;

  /// make unique name for each tool instance
  std::string toolName(const std::string& aName, const int id) const;

  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

  /// pointers to Tools
  std::vector<IOTEffCalculator*> m_singleCellEffVector;
  IOTRandomDepositCreator*       m_noiseTool;
  IOTDoublePulseTool*            m_pulseTool;

  typedef std::vector<LHCb::MCOTDeposit*> OTDeposits;
  mutable OTDeposits       m_deposits;               ///< Vector of tmp deposits
  unsigned int             m_firstStation;           ///< First OT station
  std::vector<std::string> m_spillVector;            ///< Spills to digitize  
  std::vector<double>      m_spillTimes;             ///< Spill times
  std::vector<std::string> m_spillNames;             ///< Spill names
  bool                     m_addCrossTalk;           ///< Flag to add Xtalk
  double                   m_crossTalkLevel;         ///< Level of crosstalk
  bool                     m_addNoise;               ///< Flag to add random noise
  bool                     m_addDoublePulse;         ///< Flag to add double pulses
  mutable Rndm::Numbers    m_flat;                   ///< Flat random dist for crosstalk and double pulse
  mutable Rndm::Numbers    m_gauss;                  ///< Gauss random dist
  std::string              m_doublePulseToolName;    ///< Choose between Simple and LUT tool

};

#endif // OTSIMULATION_MCOTDEPOSITCREATOR_H
