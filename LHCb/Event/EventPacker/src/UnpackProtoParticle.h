// $Id: UnpackProtoParticle.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef UNPACKPROTOPARTICLE_H 
#define UNPACKPROTOPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackProtoParticle UnpackProtoParticle.h
 *  Unpack a ProtoParticle
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class UnpackProtoParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackProtoParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackProtoParticle( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKPROTOPARTICLE_H
