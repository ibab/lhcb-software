// $Id: MCDecayCounter.h,v 1.1 2004-04-29 17:19:13 gcorti Exp $
#ifndef MCDECAYCOUNTER_H 
#define MCDECAYCOUNTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from LHCb
#include "MCTools/IMCDecayFinder.h"


/** @class MCDecayCounter MCDecayCounter.h
 *  Algorithm that counts how many decay of the desired type
 *  are present in the events 
 *
 *  @author Gloria CORTI
 *  @date   2004-04-26
 */
class MCDecayCounter : public Algorithm {
public: 
  /// Standard constructor
  MCDecayCounter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCDecayCounter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  bool            m_debug;           ///< detailed debug printout
  unsigned int    m_nEvents;         ///< Total number of events processed
  unsigned int    m_nMCFound;        ///< Total number of decays found
  IMCDecayFinder* m_mcFinder;        ///< Pointer to tool

};
#endif // MCDECAYCOUNTER_H
