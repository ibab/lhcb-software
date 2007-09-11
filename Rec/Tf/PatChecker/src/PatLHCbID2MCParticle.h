// $Id: PatLHCbID2MCParticle.h,v 1.4 2007-09-11 12:33:17 krinnert Exp $
#ifndef PATLHCBID2MCPARTICLE_H 
#define PATLHCBID2MCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
 

/** @class PatLHCbID2MCParticle PatLHCbID2MCParticle.h
 *  Build the linker table relating LHCbID used in pattern to MCParticles
 *
 *  @author Olivier Callot
 *  @date   2005-06-01
 *  @update for A-Team framework 2007-08-20 SHM
 */

namespace Tf {

  class PatLHCbID2MCParticle : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatLHCbID2MCParticle( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatLHCbID2MCParticle( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
    void addToList( const LHCb::MCParticle* part )  {
      bool found = false;
      for ( std::vector<const LHCb::MCParticle*>::iterator itP = 
	      m_partList.begin();  m_partList.end() != itP; itP++ ) {
	if ( part == (*itP) ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) m_partList.push_back( part );
    }  
    
  private:
    bool        m_level1;
    std::string m_targetName;
    std::vector<const LHCb::MCParticle*> m_partList;
    ToolHandle<IOTHitCreator> m_othitcreator;
    ToolHandle<IITHitCreator> m_ithitcreator;
    ToolHandle<ITTHitCreator> m_tthitcreator;
    bool m_linkOT;
    bool m_linkIT;
    bool m_linkTT;
    bool m_linkVELO;
  };
}
#endif // PATLHCBID2MCPARTICLE_H
