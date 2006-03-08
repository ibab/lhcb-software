// $Id: VeloDigit2MCParticleLinker.h,v 1.1 2006-03-08 11:19:01 szumlat Exp $
#ifndef VELODIGIT2MCPARTICLELINKER_H 
#define VELODIGIT2MCPARTICLELINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SmartRef.h"

// linkers
#include "Linker/LinkerTool.h"
#include "Linker/LinkerWithKey.h"

// event model
#include "Event/VeloDigit.h"
#include "Event/MCHit.h"

/** @class VeloDigit2MCParticleLinker VeloDigit2MCParticleLinker.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-05
 */

class VeloDigit;
class MCHit;

class VeloDigit2MCParticleLinker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloDigit2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDigit2MCParticleLinker( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
 
  std::string asctName();
  
private:

  std::string m_asctName;
  std::string m_inputDigits;
  std::string m_inputParts;
  
};
inline std::string VeloDigit2MCParticleLinker::asctName()
{
  return ( m_asctName );
}
//
#endif // VELODIGIT2MCPARTICLELINKER_H
