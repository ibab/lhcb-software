// $Id: PatMatch.h,v 1.2 2008-05-08 12:49:29 albrecht Exp $
#ifndef PATMATCH_H
#define PATMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"
#include "TrackInterfaces/IMatchTool.h"

  /** @class PatMatch PatMatch.h
   *  Match Velo and Seed tracks
   *
   *  @author Olivier Callot
   *  @date   2007-02-07 
   */
  class PatMatch : public GaudiAlgorithm
  {

  public:
    
    /// Standard constructor
    PatMatch( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatMatch( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

      
  private:
    std::string m_veloLocation;
    std::string m_seedLocation;
    std::string m_matchLocation;

    IMatchTool* m_matchTool;
    
  };

#endif // PATMATCH_H

