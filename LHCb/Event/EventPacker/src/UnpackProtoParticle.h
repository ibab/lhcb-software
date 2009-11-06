// $Id: UnpackProtoParticle.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
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

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
};
#endif // UNPACKPROTOPARTICLE_H
