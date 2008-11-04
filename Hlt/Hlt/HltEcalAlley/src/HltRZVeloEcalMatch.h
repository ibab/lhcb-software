// $Id: HltRZVeloEcalMatch.h,v 1.1 2008-11-04 08:30:28 witekma Exp $
#ifndef HLTRZVELOECALMATCH_H 
#define HLTRZVELOECALMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "HltBase/IBiFunctionTool.h"

#include "Event/Track.h"
#include "Event/L0CaloCandidate.h"

/** @class HltRZVeloEcalMatch HltRZVeloEcalMatch.h
 *  
 *
 *  @author Nicolas Zwahlen 
 *  @author Jose A. Hernando
 *  @date   2008-11-03
 */
class HltRZVeloEcalMatch : public GaudiTool, 
                            public ITrackBiFunctionTool
{
public:
  
  /// Standard constructor
  HltRZVeloEcalMatch( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~HltRZVeloEcalMatch( ); ///< Destructor

  double function(const LHCb::Track& rzvelo, const LHCb::Track& tcalo);
  
private:

  double match(const LHCb::Track& rzvelo);


  double m_ptkickConstant;
  double m_zKick;
  double m_eres[2]; /// de/e = sqrt( m_eres[0]^2 + m_eres[1]^2 / e )
  double m_eCorrect;


  double x,y,z;
  double ex,ey;
  double e;  

};
#endif // HLTRZVELOECALMATCH_H
