// $Id: $
#ifndef FASTTTVALIDATIONTOOL_H 
#define FASTTTVALIDATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"            // Interface
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"


/** @class FastTTValidationTool FastTTValidationTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2011-01-21
 */
class FastTTValidationTool : public GaudiTool, virtual public ITrackSelector, public IIncidentListener  {
public: 
  /// Standard constructor
  FastTTValidationTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~FastTTValidationTool( ); ///< Destructor

  virtual bool accept(const LHCb::Track& aTrack) const;

  virtual StatusCode initialize();

  void handle ( const Incident& incident );
  
protected:

  void initEvent();
  
  void clearHits();
  
private:
  Tf::TTStationHitManager <PatTTHit> *       m_ttHitManager;    ///< Tool to provide hits
  double m_zTTProj;
  double m_centralHole;
  double m_horizontalHole;
  double m_maxTTSize;
  double m_maxTTProj;
  
  std::vector<PatTTHits> m_hitsPerLayer;
  std::vector<double>    m_zTT;
  std::vector<double>    m_dxDy;
  bool m_eventReady;
};
#endif // FASTTTVALIDATIONTOOL_H
