// $Id: VeloUpdateHighThreshold.h,v 1.1 2009-08-26 11:44:21 ocallot Exp $
#ifndef VELOUPDATEHIGHTHRESHOLD_H 
#define VELOUPDATEHIGHTHRESHOLD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VeloUpdateHighThreshold VeloUpdateHighThreshold.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2009-08-20
 */
class VeloUpdateHighThreshold : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloUpdateHighThreshold( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloUpdateHighThreshold( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  double m_highThreshold;
};
#endif // VELOUPDATEHIGHTHRESHOLD_H
