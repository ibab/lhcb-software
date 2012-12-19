#ifndef TRIGGERTYPECOUNTER_H 
#define TRIGGERTYPECOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IEventTimeDecoder.h"

/** @class TriggerTypeCounter TriggerTypeCounter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-11-18
 */
class TriggerTypeCounter : public GaudiHistoAlg {
public: 
  /// Standard constructor
  TriggerTypeCounter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TriggerTypeCounter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  void hbook(const std::string& name, const std::vector<std::string>& names);
  IEventTimeDecoder* m_odin;
  
};
#endif // TRIGGERTYPECOUNTER_H
