// $Id: FilterByBestParticles.h,v 1.1 2004-07-16 11:20:48 pkoppenb Exp $
#ifndef FILTERBYBESTPARTICLES_H 
#define FILTERBYBESTPARTICLES_H 1

// Include files
#include "DaVinciTools/DVAlgorithm.h"

#include "DaVinciTools/IBestParticles.h"
#include "DaVinciTools/IParticleMaker.h"

/** @class FilterByBestParticles FilterByBestParticles.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-15
 */
class FilterByBestParticles : public DVAlgorithm {
public: 
  /// Standard constructor
  FilterByBestParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterByBestParticles( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IBestParticles* m_best ;
  std::string  m_bestFilterName ;
  IParticleMaker* m_pm;

};
#endif // FILTERBYBESTPARTICLES_H
