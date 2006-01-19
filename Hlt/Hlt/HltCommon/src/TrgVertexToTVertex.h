// $Id: TrgVertexToTVertex.h,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
#ifndef TRGVERTEXTOTVERTEX_H 
#define TRGVERTEXTOTVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "TrgTools/TrgDataProvider.h"
#include "PatTools/PatDataStore.h"

/** @class TrgVertexToTVertex TrgVertexToTVertex.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-12-16
 */
class TrgVertexToTVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrgVertexToTVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrgVertexToTVertex( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void copy(const TrgVertex& vertex, TVertex& vertex);

protected:

  std::string m_inputVerticesName;
  std::string m_outputVerticesName;

protected:

  TrgVertexContainer* m_ivertices;
  PatVertexContainer* m_overtices;

private:

};
#endif // TRGVERTEXTOTVERTEX_H
