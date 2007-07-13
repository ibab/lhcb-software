// $Id: L0ETC.h,v 1.1 2007-07-13 18:01:38 pkoppenb Exp $
#ifndef L0ETC_H 
#define L0ETC_H 1

// Include files
// from Standard Library
#include <string>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class L0ETC L0ETC.h
 *
 *  @author P. Koppenburg
 *  @date   12/07/2007
 */

class L0ETC : public GaudiTupleAlg {
public:
  /// Standard constructor
  L0ETC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0ETC( ){}; ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

private:

  /// ETC name
  std::string m_collectionName;

  /// List of SelResult objects to look for
  std::vector<std::string> m_toLookAt;

  typedef std::map<std::string,std::string> map_t;
  map_t m_columns;

  long m_events ; ///< Number of events
  

};
#endif // L0ETC_H
