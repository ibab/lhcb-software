// $Id: L0DUFromRaw.h,v 1.1 2006-05-25 22:37:17 odescham Exp $
#ifndef L0DUFROMRAW_H 
#define L0DUFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0DUFromRaw L0DUFromRaw.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-05-25
 */
class L0DUFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0DUFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DUFromRaw( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:
  std::string m_L0DUReportLocation;
  std::vector<std::string> m_channelNames;
  std::vector<std::string> m_conditionNames;
  

};
#endif // L0DUFROMRAW_H
