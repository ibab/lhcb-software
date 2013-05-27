#ifndef PATLHCBIDUP2MCPARTICLE_H 
#define PATLHCBIDUP2MCPARTICLE_H 1

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
 

/** @class PatLHCbIDUp2MCParticle PatLHCbIDUp2MCParticle.h
 *  A clone of PatLHCbIDs2MCParticle by Olivier Callot, to use with upgraded
 *  detectors. Aim is to avoid switches to upgrade in the current code.
 *
 *  @author Victor Coco (Original PatLHCbIDs2MCParticle by Olivier Callot)
 *  @date   2010-03-22
 */

  class PatLHCbIDUp2MCParticle : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatLHCbIDUp2MCParticle( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatLHCbIDUp2MCParticle( ); ///< Destructor
    
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
    
    struct increasingSensor {
     bool operator() ( LHCb::VPCluster* clust1, LHCb::VPCluster* clust2 ) const{
       return clust1->channelID().sensor() < clust2->channelID().sensor();
     }
  }  increasingSensor;

  private:
    std::string m_targetName;
    std::vector<const LHCb::MCParticle*> m_partList;
    ToolHandle<Tf::IOTHitCreator> m_othitcreator;
    bool m_linkOT;
    bool m_linkIT;
    bool m_linkTT;
    bool m_linkVELO;
    bool m_linkVP;
    // add your sub detector
  };
#endif // PATLHCBIDUP2MCPARTICLE_H
