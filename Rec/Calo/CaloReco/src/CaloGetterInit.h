// $Id: CaloGetterInit.h,v 1.1 2009-04-17 11:44:53 odescham Exp $
#ifndef CALOGETTERINIT_H 
#define CALOGETTERINIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//from LHCb
#include "CaloInterfaces/ICaloGetterTool.h"

/** @class CaloGetterInit CaloGetterInit.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-04-17
 */
class CaloGetterInit : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloGetterInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloGetterInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ICaloGetterTool* m_getter;
  std::string m_name;
};
#endif // CALOGETTERINIT_H
