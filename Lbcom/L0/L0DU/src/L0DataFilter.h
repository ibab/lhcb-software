#ifndef L0DATAFILTER_H 
#define L0DATAFILTER_H 1

// Include files
#include "L0Base/L0AlgBase.h"


/** @class L0DataFilter L0DataFilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2012-04-10
 */
class L0DataFilter : public L0AlgBase {
public: 
  /// Standard constructor
  L0DataFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DataFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_l0Location;
  std::string m_logical;
  std::map<std::string,std::vector<std::string> > m_selection ; 
  bool m_revert;
};
#endif // L0DATAFILTER_H
