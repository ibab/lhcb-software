#ifndef PATLHCBIDUP2MCHIT_H 
#define PATLHCBIDUP2MCHIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"

/** @class PatLHCbIDUp2MCHit PatLHCbIDUp2MCHit.h
 *  A clone of PatLHCbIDs2MCHit by Olivier Callot, to use with upgraded
 *  detectors. Aim is to avoid switches to upgrade in the current code.
 *
 *  @author Victor Coco (Original PatLHCbIDs2MCHit by Olivier Callot)
 *  @date   2010-03-22
 */


  class PatLHCbIDUp2MCHit : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatLHCbIDUp2MCHit( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatLHCbIDUp2MCHit( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:
    
    void addToList( const LHCb::MCHit* hit )  {
      bool found = false;
      for ( std::vector<const LHCb::MCHit*>::iterator itP = 
	      m_hitList.begin();  m_hitList.end() != itP; itP++ ) {
	if ( hit == (*itP) ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) m_hitList.push_back( hit );
    }  
    
  private:
    std::string m_targetName;
    std::vector<const LHCb::MCHit*> m_hitList;
    bool m_linkVP;
    ILHCbIDsToMCHits* m_linkTool;
  };

inline ILHCbIDsToMCHits* PatLHCbIDUp2MCHit::linkTool() const{
  return m_linkTool;
}

#endif // PATLHCBIDUP2MCHIT_H
