// $Id: HltVeloEcalMatch.h,v 1.1 2008-11-04 08:30:28 witekma Exp $
#ifndef HLTVELOECALMATCH_H 
#define HLTVELOECALMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"


#include "HltBase/IBiFunctionTool.h"
#include "Event/Track.h"

/** @class HltVeloEcalMatch HltVeloEcalMatch.h
 *  
 *
 *  @author original from N. Zwahlen
 *  @date   2006-12-08
 */
class HltVeloEcalMatch : public GaudiTool ,
                         public ITrackBiFunctionTool
{
public:
  
  /// Standard constructor
  HltVeloEcalMatch( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~HltVeloEcalMatch( ); ///< Destructor
  
  virtual StatusCode initialize();

  double function(const LHCb::Track& velo, const LHCb::Track& tcalo);
    
protected:

  double match(const LHCb::Track& rzvelo);
  
protected:
  
  double m_max3DChi2;

protected:

  double m_ptkickConstant;
  double m_zKick;

  double x,y,z;
  double ex,ey;
  double e;
  
};
#endif // HLTVELOL0HADCALOMARCH_H
