// $Id: Particle2VertexAsctAlg.h,v 1.2 2006-08-22 15:32:15 jpalac Exp $
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

protected:

private:

  double m_max;            ///< maximum value to enter table..
  bool m_useSignificance;
  std::string m_outputTableLocation;
  std::string m_inputPartLocation;
  std::string m_inputVertLocation;
  IGeomDispCalculator* m_ipTool;
  IParticle2VertexAsct* m_asctTool;
  std::string m_ipToolName;
  std::string m_asctToolName;
  
  
};
#endif // V13R1_PARTICLE2VERTEXASCTALG_H
