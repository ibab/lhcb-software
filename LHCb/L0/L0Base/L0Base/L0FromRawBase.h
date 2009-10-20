// $Id: L0FromRawBase.h,v 1.1 2009-10-20 13:20:16 jucogan Exp $
#ifndef L0FROMRAWBASE_H 
#define L0FROMRAWBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0FromRawBase L0FromRawBase.h
 *  Base class for the L0 banks decoding algorithms
 *
 *  @author Julien Cogan
 *  @date   2009-10-20
 */
class L0FromRawBase : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0FromRawBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0FromRawBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  bool m_writeProcData;    ///< Flag to activate the writing of the raw banks
  bool m_writeOnTES;       ///< Flag to activate the writing of the Candidates/Report on the TES
  std::string m_l0context; ///< Suffix appended to the default location in TES


private:

};
#endif // L0FROMRAWBASE_H
