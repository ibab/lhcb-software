// $Id: PackProtoParticle.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef PACKPROTOPARTICLE_H 
#define PACKPROTOPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackProtoParticle PackProtoParticle.h
 *  Pack a protoparticle container
 *
 *  @author Olivier Callot
 *  @date   2008-11-13
 */
class PackProtoParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackProtoParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackProtoParticle( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKPROTOPARTICLE_H
