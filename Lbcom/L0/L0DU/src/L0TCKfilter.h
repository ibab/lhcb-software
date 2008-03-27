// $Id: L0TCKfilter.h,v 1.1 2008-03-27 16:32:14 odescham Exp $
#ifndef L0TCKFILTER_H 
#define L0TCKFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0TCKfilter L0TCKfilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-03-25
 */
class L0TCKfilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0TCKfilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0TCKfilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::vector<std::string> m_list;
  std::string m_reportLocation;
  

};
#endif // L0TCKFILTER_H
