// $Id: PatCounter.h,v 1.3 2008-06-30 12:37:20 mjohn Exp $
#ifndef PATCOUNTERUPGRADE_H 
#define PATCOUNTERUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"

static const InterfaceID IID_PatCounterUpgrade ( "PatCounterUpgrade", 1, 0 );

/** @class PatCounterUpgrade PatCounterUpgrade.h
 *  This is a counter for track efficiency measurement.
 *
 *  @author Olivier Callot
 *  @date   2005-06-10
 *  Modified by Wenbin Qian for the VP Pat Efficiency
 *  @adpate to A-Team framework 2007-08-20 SHM
 */


  class PatCounterUpgrade : public GaudiTool {
  public: 
    
    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatCounterUpgrade; }
    
    /// Standard constructor
    PatCounterUpgrade( const std::string& type, 
		const std::string& name,
		const IInterface* parent);
    
    virtual ~PatCounterUpgrade( ); ///< Destructor
    
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
    int m_nbGhostVlp;              ///< Ghosts from 'VP'
   
    
    // total variables
    int m_totTrackVlp;             ///< Total number of tracks (Space)
    int m_totTrack;                ///< Total number of tracks processed
    int m_totGhost;                ///< Total number of ghosts
    int m_totGhostVlp;             ///< Total number of ghosts (Space)
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

#endif // PATCOUNTERUPGRADE_H
