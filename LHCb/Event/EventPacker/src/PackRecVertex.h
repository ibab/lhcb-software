// $Id: PackRecVertex.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
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

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKRECVERTEX_H
