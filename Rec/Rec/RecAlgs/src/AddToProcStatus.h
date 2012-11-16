#ifndef ADDTOPROCSTATUS_H 
#define ADDTOPROCSTATUS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class AddToProcStatus AddToProcStatus.h
 * 
 *  Add an entry to ProcStatus if some sequeence failed
 *
 *  @author Patrick Koppenburg
 *  @date   2011-06-15
 */
class AddToProcStatus : public GaudiAlgorithm {
public: 
  /// Standard constructor
  AddToProcStatus( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AddToProcStatus( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_subsystem ; ///< subsystem
  std::string m_reason ; ///< Reason for error
  int m_status ; ///< return code
  bool m_abort ; ///< should processing be aborted?
};
#endif // ADDTOPROCSTATUS_H
