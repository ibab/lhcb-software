// $Id: Particle2MCLinks.h,v 1.3 2002-07-17 07:49:21 phicharp Exp $
#ifndef Particle2MCLinks_H 
#define Particle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "DaVinciAssociators/AsctAlgorithm.h"

/** @class Particle2MCLinks Particle2MCLinks.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/05/2002
 */
class Particle2MCLinks : public AsctAlgorithm {
  friend AlgFactory<Particle2MCLinks>;
  
public:
  /// Standard constructor
  Particle2MCLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ProtoParticle2MCAsct::IAsct* m_pAsctProto;
  
};
#endif // Particle2MCLinks_H
