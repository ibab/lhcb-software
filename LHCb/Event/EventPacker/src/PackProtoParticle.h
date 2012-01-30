// $Id: PackProtoParticle.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
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

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing
};
#endif // PACKPROTOPARTICLE_H
