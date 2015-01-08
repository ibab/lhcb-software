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
   *  Match Velo and Seed tracks. This is just a wrapper, 
   *  the acutal work is done in the tool, per default in PatMatchTool.
   *
   *  - VeloInput: Input location of Velo tracks
   *  - SeedInput: Input location of Seed tracks
   *  - MatchOutput: Output location of match tracks
   *  - ToolName: Name of the tool that does the matching
   *
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
    std::string m_matchToolName;
    
    
    IMatchTool* m_matchTool;
    
};

#endif // PATMATCH_H

