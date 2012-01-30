// $Id: PackRecVertex.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef PACKRECVERTEX_H 
#define PACKRECVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackRecVertex PackRecVertex.h
 *  Pack the RecVertex
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class PackRecVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackRecVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackRecVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing
};
#endif // PACKRECVERTEX_H
