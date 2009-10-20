// $Id: L0AlgBase.h,v 1.1 2009-10-20 13:20:16 jucogan Exp $
#ifndef L0ALGBASE_H 
#define L0ALGBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0AlgBase L0AlgBase.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2009-10-20
 */
class L0AlgBase : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0AlgBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0AlgBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  bool m_writeBanks;       ///< Flag to activate the writing of the raw banks
  bool m_writeOnTES;       ///< Flag to activate the writing of the Candidates/Report on the TES
  std::string m_l0context; ///< Suffix appended to the default location in TES

private:

};
#endif // L0ALGBASE_H
