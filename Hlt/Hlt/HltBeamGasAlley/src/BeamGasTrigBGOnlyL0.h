// $Id: BeamGasTrigBGOnlyL0.h,v 1.1.1.2 2008-07-27 11:35:09 phopchev Exp $
#ifndef INCLUDE_BEAMGASTRIGBGONLYL0_H
#define INCLUDE_BEAMGASTRIGBGONLYL0_H 1

#include "HltBase/HltAlgorithm.h"
#include "Event/L0DUReport.h"


/** @class BeamGasTrigBGOnlyL0 BeamGasTrigBGOnlyL0.h
 * A simple HltAlgorithm that lets through events only
 * after confirming the L0 decision and checking if the
 * bunch crossing contains only 1 bunch, i.e. b1-empty
 * or empty-b2
 *
 * @author Plamen Hopchev <plamen.hopchev@cern.ch>
 * @date   2008-06-16
 */
class BeamGasTrigBGOnlyL0 : public HltAlgorithm {

public:

  /// Standard Constructor
  BeamGasTrigBGOnlyL0(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~BeamGasTrigBGOnlyL0(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
  virtual StatusCode   finalize(); ///< Algorithm finalize

private:
  std::string	m_l0Location;		///< Location of the L0 Decision
  std::string   m_odinLocation;		///< Location of ODIN
  int		m_bxType1;		///< Types of bunch-crossing that ...	
  int		m_bxType2;		///< ...are accepted by the algortihm
  
};
#endif // INCLUDE_BEAMGASTRIGBGONLYL0_H
