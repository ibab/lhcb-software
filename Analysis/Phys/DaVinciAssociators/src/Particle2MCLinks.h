// $Id: Particle2MCLinks.h,v 1.2 2002-07-12 15:27:34 phicharp Exp $
#ifndef Particle2MCLinks_H 
#define Particle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


// local
#include "DaVinciAssociators/Particle2MCAsct.h"

/** @class Particle2MCLinks Particle2MCLinks.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/05/2002
 */
class Particle2MCLinks : public Algorithm {
  friend AlgFactory<Particle2MCLinks>;
  
public:
  /// Standard constructor
  Particle2MCLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; }
  
protected:

private:
  std::vector<std::string> m_inputData;  ///< location of Particles to associate
  std::string m_outputTable; ///< location of relations table  
};
#endif // Particle2MCLinks_H
