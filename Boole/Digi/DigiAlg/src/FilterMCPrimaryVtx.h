// $Id: FilterMCPrimaryVtx.h,v 1.1 2009-04-06 09:53:59 cattanem Exp $
#ifndef FILTERMCPRIMARYVTX_H 
#define FILTERMCPRIMARYVTX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FilterMCPrimaryVtx FilterMCPrimaryVtx.h
 *  Filters MCParticles container to keep only primary vertices
 *  Removes daughters, updates SmartRefs in MCHeader
 *
 *  @author Marco Cattaneo
 *  @date   2009-04-06
 */
class FilterMCPrimaryVtx : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterMCPrimaryVtx( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterMCPrimaryVtx( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // FILTERMCPRIMARYVTX_H
