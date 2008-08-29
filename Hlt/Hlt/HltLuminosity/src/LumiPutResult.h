// $Id: LumiPutResult.h,v 1.1 2008-08-29 07:52:45 panmanj Exp $
#ifndef LUMIPUTRESULT_H 
#define LUMIPUTRESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "HltBase/ANNSvc.h"

/** @class LumiPutResult LumiPutResult.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-08-27
 */
class LumiPutResult : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiPutResult( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiPutResult( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  std::string m_InputContainerName;
  std::vector<IANNSvc::minor_value_type> m_items;
  unsigned int m_size;
  double *m_means;
  double *m_thresholds;
  unsigned int *m_infoKeys;

private:

};
#endif // LUMIPUTRESULT_H
