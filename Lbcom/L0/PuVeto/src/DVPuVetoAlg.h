#ifndef DVPUVETOALG_H 
#define DVPUVETOALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

/** @class DVPuVetoAlg DVPuVetoAlg.h
 *  Overrides the decision of the Pile-up veto based on
    the height of the second peak.
    To be used only in DaVinci.
 *
 *  @author Eduardo Rodrigues
 *  @date   22/01/2003
 */
class DVPuVetoAlg : public Algorithm {
public:
  /// Standard constructor
  DVPuVetoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVPuVetoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  std::string m_nameOfOutputPileUpVeto;
  double      m_lowThreshold;
  double      m_highThreshold;
  double      m_highPosition;
  double      m_secondPosition;

};
#endif // DVPUVETOALG_H
