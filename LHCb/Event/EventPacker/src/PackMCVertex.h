// $Id: PackMCVertex.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef PACKMCVERTEX_H 
#define PACKMCVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackMCVertex PackMCVertex.h
 *  Pack the MCVertices
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class PackMCVertex : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  PackMCVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackMCVertex( );     ///< Destructor
  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing

};

#endif // PACKMCVERTEX_H
