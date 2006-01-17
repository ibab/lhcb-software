// $Id: Particle2VertexWithIPS.h,v 1.5 2006-01-17 12:54:34 pkoppenb Exp $
#ifndef Particle2VertexWithIPS_H
#define Particle2VertexWithIPS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

class IGeomDispCalculator;

// local
#include "Kernel/Particle2VertexAsct.h"
#include "GaudiKernel/Algorithm.h"

/** @class Particle2VertexWithIPS Particle2VertexWithIPS.h
 *
 *  @author Gerhard Raven
 *  @date   May 2003
 */

class Particle2VertexWithIPS : public Algorithm
{

  friend class AlgFactory<Particle2VertexWithIPS>;

public:

  /// Standard constructor
  Particle2VertexWithIPS( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2VertexWithIPS( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  inline std::string outputTable() { return m_outputTable; }

private:

  double m_max;            ///< maximum value to enter table..
  bool m_useSignificance;
  std::string m_outputTable;
  std::vector<std::string> m_inputData;
  IGeomDispCalculator* m_ipTool;

};

#endif // Particle2Vertex_H
