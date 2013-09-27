#ifndef PRCHEATEDVELO_H 
#define PRCHEATEDVELO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "PrFitPolinomial.h"

/** @class PrCheatedVelo PrCheatedVelo.h
 *  Cheated Pattern for the Velo
 *
 *  @author Olivier Callot
 *  @date   2012-07-26
 */
class PrCheatedVelo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrCheatedVelo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrCheatedVelo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};

#endif // PRCHEATEDVELO_H
