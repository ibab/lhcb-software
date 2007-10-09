// $Id: PatTTCounter.h,v 1.1.1.1 2007-10-09 18:41:19 smenzeme Exp $
#ifndef PATTTCOUNTER_H 
#define PATTTCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"

static const InterfaceID IID_PatTTCounter ( "PatTTCounter", 1, 0 );

/** @class PatTTCounter PatTTCounter.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2006-06-28
 *  @update for A-Team framework 2007-08-20 SHM
 */


  class PatTTCounter : public GaudiTool {
    
  public: 
    
    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatTTCounter; }
    
    /// Standard constructor
    PatTTCounter( const std::string& type, 
		  const std::string& name,
		  const IInterface* parent);
    
    virtual ~PatTTCounter( ); ///< Destructor
    virtual StatusCode finalize();  
    
    void initEvent();
    
    void count( const LHCb::MCParticle* part, 
		std::vector<bool> flags, 
		std::vector<LHCb::LHCbID>& ids );
    
    void setContainer( std::string name )  { m_container = name; }
    
    void addSelection ( std::string name );
    
    void printStatistics();
    
  protected:
    
  private:
    typedef LinkerTool<LHCb::Track, LHCb::MCParticle> TrackAsct;
    typedef TrackAsct::InverseType InvTable;
    typedef InvTable::Range        InvRange;
    typedef InvTable::iterator     InvIterator;
    
    std::string m_container;
    TrackAsct* m_link;
    const InvTable* m_invTable;
    bool m_validData;
    
    double m_nbGhost;
    double m_nbGhostHit;
    int m_totTrack;
    
    std::vector<std::string> m_name;    ///< Name of the sub-counters
    std::vector<double> m_nbTrack;
    std::vector<double> m_mcHits;          ///< Nb of MC hits on tracks
    std::vector<double> m_foundOK;         ///< Nb of correct hits
    std::vector<double> m_wrong;           ///< Nb of wrong ones
    std::vector<double> m_nbTrack3;
    std::vector<double> m_mcHits3;         ///< Nb of MC hits on tracks with >= 3 TT hits
    std::vector<double> m_foundOK3;        ///< Nb of correct hits 3 TT hits
    std::vector<double> m_wrong3;          ///< Nb of wrong ones 3 TT hits
  };

#endif // PATTTCOUNTER_H
