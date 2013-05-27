// $Id: L0ETC.h,v 1.2 2007-07-15 16:57:48 pkoppenb Exp $
#ifndef L0ETC_H 
#define L0ETC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
/** @class L0ETC L0ETC.h
 *
 *  Writes out ETC for L0 stripping  
 *
 *  @author Patrick Koppenburg
 *  @date   2007-07-13
 */
class L0ETC : public GaudiTupleAlg {
public: 
  /// Standard constructor
  L0ETC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0ETC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// ETC name
  std::string m_collectionName;
  /// L0 channels
  std::vector<std::string> m_l0channels ;

  long m_events ; ///< Number of events

};
#endif // L0ETC_H
