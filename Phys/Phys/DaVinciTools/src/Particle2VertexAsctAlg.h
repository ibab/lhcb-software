// $Id: Particle2VertexAsctAlg.h,v 1.1 2006-06-08 16:08:38 jpalac Exp $
#ifndef V13R1_PARTICLE2VERTEXASCTALG_H 
#define V13R1_PARTICLE2VERTEXASCTALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


class IParticle2VertexAsct;
class IGeomDispCalculator;


/** @class Particle2VertexAsctAlg Particle2VertexAsctAlg.h v13r1/Particle2VertexAsctAlg.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-06-07
 */
class Particle2VertexAsctAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  Particle2VertexAsctAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2VertexAsctAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  template <typename T>
  StatusCode makeConstVector(typename T::ConstVector& things,
                             const std::vector<std::string>& locations);

  template <typename T>
  StatusCode makeConstVector(typename T::ConstVector& things,
                             const std::string& location) ;

protected:

private:

  double m_max;            ///< maximum value to enter table..
  bool m_useSignificance;
  std::string m_outputTableLocation;
  std::vector<std::string> m_inputData;
  IGeomDispCalculator* m_ipTool;
  IParticle2VertexAsct* m_asctTool;
  std::string m_ipToolName;
  std::string m_asctToolName;
  
  
};
#endif // V13R1_PARTICLE2VERTEXASCTALG_H
