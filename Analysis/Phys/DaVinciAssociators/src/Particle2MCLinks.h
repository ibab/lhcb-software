// $Id: Particle2MCLinks.h,v 1.7 2003-07-29 17:48:12 gcorti Exp $
#ifndef Particle2MCLinks_H 
#define Particle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


// local
#include "DaVinciAssociators/Particle2MCLinksAsct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "AsctAlgorithm.h"

/** @class Particle2MCLinks Particle2MCLinks.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/05/2002
 */
class Particle2MCLinks : public AsctAlgorithm {
  friend class AlgFactory<Particle2MCLinks>;
  
public:
  /// Standard constructor
  Particle2MCLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ProtoParticle2MCAsct::IAsct* m_pAsctCharged;
  ProtoParticle2MCAsct::IAsct* m_pAsctNeutral;

};
#endif // Particle2MCLinks_H
