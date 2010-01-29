// $Id: L0TCKfilter.h,v 1.2 2010-01-29 12:49:27 odescham Exp $
#ifndef L0TCKFILTER_H 
#define L0TCKFILTER_H 1

// Include files
// from Gaudi
#include "L0Base/L0AlgBase.h"


/** @class L0TCKfilter L0TCKfilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-03-25
 */
class L0TCKfilter : public L0AlgBase {
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
