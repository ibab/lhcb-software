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

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  std::vector<std::string> m_list;
  std::string m_reportLocation;
  

};
#endif // L0TCKFILTER_H
