// $Id: ResetOnOffline.h,v 1.1 2005-06-08 16:15:32 pkoppenb Exp $
#ifndef RESETONOFFLINE_H 
#define RESETONOFFLINE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IOnOffline;
/** @class ResetOnOffline ResetOnOffline.h
 *  
 *  Resets to On or offline environment
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class ResetOnOffline : public GaudiAlgorithm {
public: 
  /// Standard constructor
  ResetOnOffline( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ResetOnOffline( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IOnOffline* m_onOffline ;   ///< On or offline
  bool m_offline ; ///< reset to offline
  bool m_online ; ///< reset to online

};
#endif // RESETONOFFLINE_H
