// $Id: CompareProtoParticle.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef COMPAREPROTOPARTICLE_H 
#define COMPAREPROTOPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareProtoParticle CompareProtoParticle.h
 *  Compare two containers of ProtoParticles
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class CompareProtoParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareProtoParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareProtoParticle( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPAREPROTOPARTICLE_H
