// $Id: HltVeloEcalMatch.h,v 1.1 2009-05-30 19:24:13 graven Exp $
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
   	                public ITrackBiFunctionTool {

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
  double ecal_cell_size(const LHCb::Track& track);
  
protected:
  
  double m_max3DChi2;


protected:

  double m_ptkickConstant;
  double m_zKick;
  double m_eres[2]; /// de/e = sqrt( m_eres[0]^2 + m_eres[1]^2 / e )
  double m_eCorrect;  /// simple energy correction factor

  double x,y,z;
  double ex,ey;
  double e;
  
};
#endif // HLTVELOL0HADCALOMARCH_H
