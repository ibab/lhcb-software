// $Id: PatVeloAddTruth.h,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
#ifndef PATVELOADDTRUTH_H 
#define PATVELOADDTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "PatTools/PatDataStore.h"

/** @class PatVeloAddTruth PatVeloAddTruth.h
 *  Add the truth information on PatVeloCoord
 *
 *  @author Olivier Callot
 *  @date   2006-10-02
 *  @update for A-Team framework 2007-08-20 SHM
 */

namespace Tf {

  class PatVeloAddTruth : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatVeloAddTruth( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatVeloAddTruth( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    PatVeloContainer* m_veloCoords;
  };
}
#endif // PATVELOADDTRUTH_H
  
