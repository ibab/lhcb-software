// $Id: PreLoadParticles.h,v 1.1 2002-10-13 21:24:13 gcorti Exp $
#ifndef PRELOADPARTICLES_H 
#define PRELOADPARTICLES_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from DaVinci
#include "DaVinciTools/IPhysDesktop.h"


/** @class PreLoadParticles PreLoadParticles.h
 *  Trivial algorithm to load and save Particles without doing
 *  anything.
 *
 *  @author Gloria Corti
 *  @date   2002-08-22
 */
class PreLoadParticles : public Algorithm {
public:
  /// Standard constructor
  PreLoadParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PreLoadParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  IPhysDesktop* desktop() const;            ///< Accessor to desktop
  
private:

  IPhysDesktop*  m_pDesktop;           ///< Reference to desktop
  
};
#endif // PRELOADPARTICLES_H
