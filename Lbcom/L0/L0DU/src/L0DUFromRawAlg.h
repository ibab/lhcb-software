// $Id: L0DUFromRawAlg.h,v 1.2 2009-04-18 00:17:12 odescham Exp $
#ifndef L0DUFROMRAWALG_H 
#define L0DUFROMRAWALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Interfaces
#include "L0Interfaces/IL0DUFromRawTool.h"


/** @class L0DUFromRawAlg L0DUFromRawAlg.h
 *  
 * Decode L0DU rawBank 
 * Produce L0DUReport on TES  
 * ReBuilt L0ProcessorData from L0DU rawBank
 * 
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-25
 */
class L0DUFromRawAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0DUFromRawAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DUFromRawAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:
  std::string m_L0DUReportLocation;
  std::string m_proDataLoc   ;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_fromRaw;

  unsigned long m_sizeMax;
  unsigned long m_sizeMin;
  unsigned long m_size;
  unsigned long m_evt;
  bool m_proc;
};
#endif // L0DUFROMRAWALG_H
