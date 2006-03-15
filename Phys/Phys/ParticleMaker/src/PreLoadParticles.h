// $Id: PreLoadParticles.h,v 1.4 2006-03-15 13:47:30 pkoppenb Exp $
#ifndef PRELOADPARTICLES_H 
#define PRELOADPARTICLES_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "Kernel/IPhysDesktop.h"
#include "Kernel/DVAlgorithm.h"


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

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  
  
private:

};
#endif // PRELOADPARTICLES_H
