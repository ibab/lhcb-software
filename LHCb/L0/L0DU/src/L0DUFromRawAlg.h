#ifndef L0DUFROMRAWALG_H 
#define L0DUFROMRAWALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//Base
#include "L0Base/L0FromRawBase.h" 

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
class L0DUFromRawAlg : public L0FromRawBase {
public: 
  /// Standard constructor
  L0DUFromRawAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DUFromRawAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  void compareReports();
  
private:
  std::string m_L0DUReportLocation;
  std::string m_procDataLocation   ;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_fromRaw;
  IL0DUFromRawTool* m_fromRaw2;
  bool m_hlt1;
  bool m_compare;
  bool m_ensureKnownTCK;
};
#endif // L0DUFROMRAWALG_H
