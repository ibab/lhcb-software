// $Id: PhysSelectAlgorithm.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_PHYSSELECTALGORITHM_H
#define DAVINCITOOLS_PHYSSELECTALGORITHM_H 1

// Include files
#include "GaudiKernel/Algorithm.h"


/** @class PhysSelectAlgorithm PhysSelectAlgorithm.h DaVinciTools/PhysSelectAlgorithm.h
 *  Selection routines for a set of decay channels
 *  For the moment a wrapper of the axsel package
 *
 *  @author Gloria Corti
 *  @date   05/05/2001
 */

class PhysSelectAlgorithm : public Algorithm {

public:
  /// Standard constructor
  PhysSelectAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~PhysSelectAlgorithm(); ///< Standard destructor
  
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

private:
  std::string m_decayName;    ///< four character name identifying the decay
  int         m_selMode;      ///< selection mode for AXSEL package
  int         m_selLevel;     ///< selection level for AXSEL package 
  int         m_selTag;       ///< require or not flavour tag in AXSEL package
  int         m_selHisto;     ///< histogram control for AXSEL package
  std::string m_selCutsFname; ///< name of cdf file containg cuts
  std::string m_selHistFname; ///< name of output histogram file
  int         m_decayCode;    ///< id code of decay as in bookkeeping DB

  int         m_nEvents;      ///< event counter
};
# endif    // DAVINCITOOLS_PHYSSELECTALGORITHM_H
