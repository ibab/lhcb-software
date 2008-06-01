// $Id: PatCounter.h,v 1.2 2008-06-01 17:28:53 mjohn Exp $
#ifndef PATCOUNTER_H 
#define PATCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"

static const InterfaceID IID_PatCounter ( "PatCounter", 1, 0 );

/** @class PatCounter PatCounter.h
 *  This is a counter for track efficiency measurement.
 *
 *  @author Olivier Callot
 *  @date   2005-06-02
 *  @adpate to A-Team framework 2007-08-20 SHM
 */


  class PatCounter : public GaudiTool {
  public: 
    
    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatCounter; }
    
    /// Standard constructor
    PatCounter( const std::string& type, 
		const std::string& name,
		const IInterface* parent);
    
    virtual ~PatCounter( ); ///< Destructor
    
    virtual StatusCode finalize();  
    
    void initEvent();
    
    void setSelectId( int data )   { m_selectId = data; }
    
    int count( const LHCb::MCParticle* part, 
	       std::vector<bool> flags, 
	       std::vector<LHCb::LHCbID>& ids );
    
    void setContainer( std::string name )  { m_container = name; }
    
    void addSelection ( std::string name );
    
    void printStatistics();
    
  protected:
    
  private:
    typedef LinkerTool<LHCb::Track, LHCb::MCParticle> MyAsct;
    typedef MyAsct::DirectType            Table;
    typedef MyAsct::DirectType::Range     Range;
    typedef Table::iterator               iterator;
    
    typedef MyAsct::InverseType           InvTable;
    typedef InvTable::Range               InvRange;
    typedef InvTable::iterator            InvIterator;
    
    std::string m_container;
    int         m_selectId;
    
    bool m_validData;
    // Event variables  
    int m_nbTrack;                 ///< Tracks for this event
    int m_nbGhost;                 ///< Ghosts for this event
    int m_nbGhostSpc;                 ///< Ghosts for this event
    int m_nbGhostGen;                 ///< Ghosts for this event
    int m_nbGhostGrl;                 ///< Ghosts for this event
    int m_nbGhostOpn;                 ///< Ghosts for this event
    
    // total variables
    int m_totTrackSpc;                ///< Total number of tracks processed
    int m_totTrackGrl;                ///< Total number of tracks processed
    int m_totTrackGen;                ///< Total number of tracks processed
    int m_totTrackOpn;                ///< Total number of tracks processed
    int m_totTrack;                ///< Total number of tracks processed
    int m_totGhost;                ///< Total number of ghosts
    int m_totGhostSpc;                ///< Total number of ghosts
    int m_totGhostGrl;                ///< Total number of ghosts
    int m_totGhostGen;                ///< Total number of ghosts
    int m_totGhostOpn;                ///< Total number of ghosts
    double m_fracGhost;
    double m_nEvent;
    
    MyAsct* m_link;
    const InvTable* m_invTable;
  
    std::vector<std::string> m_name;    ///< Name of the sub-counters
    std::vector<int> m_wanted;     ///< Nb MC tracks measurable.
    std::vector<int> m_counted;    ///< counters for statistics
    std::vector<int> m_clone;      ///< counters for clones
    std::vector<double> m_purity;  ///< Sum of purity (linker weight)
    std::vector<double> m_hitEff;  ///< Sum of hitEfficiency
  };

#endif // PATCOUNTER_H
