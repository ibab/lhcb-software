// $Id: UnpackMCVertex.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef UNPACKMCVERTEX_H 
#define UNPACKMCVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackMCVertex UnpackMCVertex.h
 *  Unpack the MCVertices
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class UnpackMCVertex : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  UnpackMCVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackMCVertex(); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

};

#endif // UNPACKMCVERTEX_H
