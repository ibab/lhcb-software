// $Id: PatMatch.h,v 1.2 2008-05-08 12:49:29 albrecht Exp $
#ifndef PRMATCH_H
#define PRMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"
#include "TrackInterfaces/IMatchTool.h"

/** @class PrMatch PrMatch.h
 *  Match Velo and Seed tracks
 *
 *  @author Michel De Cian (migration to Upgrade)
 *  @date 2013-11-15  
 *
 *  @author Olivier Callot
 *  @date   2007-02-07 
 */

class PrMatch : public GaudiAlgorithm
{
  
public:
    
  /// Standard constructor
  PrMatch( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~PrMatch( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  
private:
  std::string m_veloLocation;
  std::string m_seedLocation;
  std::string m_matchLocation;
  
  IMatchTool* m_matchTool;
  
};

#endif // PRMATCH_H

