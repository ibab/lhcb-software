// $Id: PackTwoProngVertex.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef PACKTWOPRONGVERTEX_H 
#define PACKTWOPRONGVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackTwoProngVertex PackTwoProngVertex.h
 *  Pack a two prong vertex.
 *
 *  @author Olivier Callot
 *  @date   2009-01-21
 */
class PackTwoProngVertex : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  PackTwoProngVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTwoProngVertex(); ///< Destructor

  virtual StatusCode execute();///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;   ///< Flag to turn on the creation of output, even when input is missing

};

#endif // PACKTWOPRONGVERTEX_H
