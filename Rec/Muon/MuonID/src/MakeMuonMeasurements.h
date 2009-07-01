// $Id: MakeMuonMeasurements.h,v 1.1 2009-07-01 18:27:11 polye Exp $
#ifndef MAKEMUONMEASUREMENTS_H 
#define MAKEMUONMEASUREMENTS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IMeasurementProvider.h"

/** @class MakeMuonMeasurements MakeMuonMeasurements.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-16
 */
class MakeMuonMeasurements : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MakeMuonMeasurements( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MakeMuonMeasurements( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  IMeasurementProvider*  m_measProvider;

private:

};
#endif // MAKEMUONMEASUREMENTS_H
