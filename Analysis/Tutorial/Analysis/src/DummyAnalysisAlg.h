// $Id: DummyAnalysisAlg.h,v 1.1 2009-10-02 07:24:53 cattanem Exp $
#ifndef DUMMYANALYSISALG_H 
#define DUMMYANALYSISALG_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

/** @class DummyAnalysisAlg DummyAnalysisAlg.h
 * 
 *  Dummy algorithm to allow creation of empty library in Tutorial example
 *
 *  @author Marco Cattaneo
 *  @date   2009-10-02
 */
class DummyAnalysisAlg : public DaVinciTupleAlgorithm 
{

public: 

  /// Standard constructor
  DummyAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DummyAnalysisAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // DUMMYANALYSISALG_H
