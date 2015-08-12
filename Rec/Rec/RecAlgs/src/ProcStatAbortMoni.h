// $Id: $
#ifndef RECALGS_PROCSTATABORTMONI_H 
#define RECALGS_PROCSTATABORTMONI_H 1

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// GaudiUtils
#include "GaudiUtils/HistoLabels.h"

 // boost
#include "boost/assign/list_of.hpp"

// Event model
#include "Event/ProcStatus.h"

// AIDA
#include "AIDA/IProfile1D.h"

/** @class ProcStatAbortMoni ProcStatAbortMoni.h
 *  
 *  Monitor for abort rates in ProcStat
 *  
 *  @author Chris Jones
 *  @date   2010-07-16
 */

class ProcStatAbortMoni : public GaudiHistoAlg
{

public: 

  /// Standard constructor
  ProcStatAbortMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProcStatAbortMoni( );   ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute ();   ///< Algorithm execution

private:

  /// List of subsystems
  std::vector<std::string> m_subSystems;

  /// cache the histogram pointer
  AIDA::IProfile1D * m_h;

  /// Location of processing status object in TES
  std::string m_procStatLocation;

};

#endif // RECALGS_PROCSTATABORTMONI_H
