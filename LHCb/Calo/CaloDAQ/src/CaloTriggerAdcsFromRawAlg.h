// $Id: CaloTriggerAdcsFromRawAlg.h,v 1.1 2006-04-19 16:37:46 odescham Exp $
#ifndef CALOTRIGGERADCSFROMRAWALG_H 
#define CALOTRIGGERADCSFROMRAWALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// CaloDAQ
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"

/** @class CaloTriggerAdcsFromRawAlg CaloTriggerAdcsFromRawAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-04-07
 */
class CaloTriggerAdcsFromRawAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloTriggerAdcsFromRawAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloTriggerAdcsFromRawAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_outputData;         ///< Output container
  std::string m_toolName;
  ICaloTriggerAdcsFromRaw*    m_l0AdcTool;
  
};
#endif // CALOTRIGGERADCSFROMRAWALG_H
