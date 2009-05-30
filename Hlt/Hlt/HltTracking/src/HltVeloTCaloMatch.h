// $Id: HltVeloTCaloMatch.h,v 1.1 2009-05-30 12:40:21 graven Exp $
#ifndef HLTVELOTCALOMATCH_H 
#define HLTVELOTCALOMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"


#include "TrackInterfaces/ITrackMatch.h"
#include "HltBase/ITrackL0CaloMatchTool.h"
#include "HltBase/IBiFunctionTool.h"
#include "Event/Track.h"
#include "Event/L0CaloCandidate.h"


/** @class HltVeloTCaloMatch HltVeloTCaloMatch.h
 *  
 *
 *  @author original from N. Zwahlen
 *  @date   2006-12-08
 */
class HltVeloTCaloMatch : public GaudiTool ,
                          public ITrackMatch ,
                          public ITrackBiFunctionTool,
                          public ITrackL0CaloMatchTool {
public:
  
  /// Standard constructor
  HltVeloTCaloMatch( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~HltVeloTCaloMatch( ); ///< Destructor
  
  virtual StatusCode initialize();

  double function(const LHCb::Track& velo, const LHCb::Track& tcalo);
    
  
  StatusCode match(const LHCb::Track& velo, const LHCb::Track& tcalo, 
                   LHCb::Track& velotcalo,
                   double& quality, double& quality2);
  
  double match(const LHCb::Track& rzvelo, 
               const LHCb::L0CaloCandidate& tcalo);

protected:

  double match(const LHCb::Track& rzvelo);
  
protected:
  
  bool m_doRematch2D;
  double m_max2DChi2;
  double m_max3DChi2;

  ITrackBiFunctionTool* m_rematchTool;


protected:

  double m_ptkickConstant;
  double m_zKick;
  double m_eres[2]; /// de/e = sqrt( m_eres[0]^2 + m_eres[1]^2 / e )

  double x,y,z;
  double ex,ey;
  double e;
  
};
#endif // HLTVELOL0HADCALOMARCH_H
