// $Id: HltRZVeloTCaloMatch.h,v 1.1 2009-05-30 12:40:21 graven Exp $
#ifndef HLTRZVELORZTCALOMATCH_H 
#define HLTRZVELORZTCALOMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "HltBase/IBiFunctionTool.h"
#include "HltBase/ITrackL0CaloMatchTool.h"

#include "Event/Track.h"
#include "Event/L0CaloCandidate.h"

/** @class HltRZVeloTCaloMatch HltRZVeloTCaloMatch.h
 *  
 *
 *  @author Nicolas Zwahlen 
 *  @author Jose A. Hernando
 *  @date   2006-12-08
 */
class HltRZVeloTCaloMatch : public GaudiTool, 
                            public ITrackBiFunctionTool, 
                            public ITrackL0CaloMatchTool
{
public:
  
  /// Standard constructor
  HltRZVeloTCaloMatch( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~HltRZVeloTCaloMatch( ); ///< Destructor

  double function(const LHCb::Track& rzvelo, const LHCb::Track& tcalo);
  
  double match(const LHCb::Track& rzvelo, const LHCb::L0CaloCandidate& tcalo);

private:

  double match(const LHCb::Track& rzvelo);


  double m_ptkickConstant;
  double m_zKick;
  double m_eres[2]; /// de/e = sqrt( m_eres[0]^2 + m_eres[1]^2 / e )


  double x,y,z;
  double ex,ey;
  double e;  

};
#endif // HLTVELORZL0HADCALOMARCH_H
