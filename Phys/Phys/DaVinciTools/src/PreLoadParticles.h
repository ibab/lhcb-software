// $Id: PreLoadParticles.h,v 1.3 2004-07-16 11:20:48 pkoppenb Exp $
#ifndef PRELOADPARTICLES_H 
#define PRELOADPARTICLES_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/DVAlgorithm.h"


/** @class PreLoadParticles PreLoadParticles.h
 *  Trivial algorithm to load and save Particles without doing
 *  anything.
 *
 *  @author Gloria Corti
 *  @date   2002-08-22
 */

class PreLoadParticles : public DVAlgorithm {
public:
  /// Standard constructor
  PreLoadParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PreLoadParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  
private:

};
#endif // PRELOADPARTICLES_H
