// $Id: SmearVertexAlg.h,v 1.2 2005-06-24 16:32:47 gcorti Exp $
#ifndef GENERATORS_SMEARVERTEXALG_H 
#define GENERATORS_SMEARVERTEXALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class SmearVertexAlg SmearVertexAlg.h Algorithms/SmearVertexAlg.h
 *  
 *  Class to smear primary vertex coordinates.
 *  By default the vertex is smeared with gaussian distributions in 
 *  the three coordinate with a possible cut in number of sigmas.
 *  It is also possible to smear the z of the vertex with a flat
 *  distribution within certain allowed values. 
 *
 *  @author Witek Pokorski
 *  @author Angelo Carbone
 *  @author Gloria Corti
 *  @date   2003-02-02 (last modified 2005-06-23)
 */
class SmearVertexAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  SmearVertexAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SmearVertexAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  Rndm::Numbers m_gaudigen;         ///< default random number used (gaussian)
  Rndm::Numbers m_flatgen;          ///< alternative random number used (flat)

  double m_xsigma;   ///< sigma(x) of primary vertex
  double m_ysigma;   ///< sigma(y) of primary vertex
  double m_zsigma;   ///< sigma(z) of primary vertex

  double m_xcut;     ///< cut in number of sigma in x of primary vertex 
  double m_ycut;     ///< cut in number of sigma in y of primary vertex
  double m_zcut;     ///< cut in number of sigma in z of primary vertex

  bool   m_flat;     ///< flag to control generation of flat distribution in z
  double m_zmax;     ///< max z value in case of flat distribution in z 
  double m_zmin;     ///< min z value in case of flat distribution in z

  std::string m_eventLoc;      ///< Location of HepMCEvent in TES
  std::string m_generatorName; ///< Name of generator
 
};
#endif // GENERATORS_SMEARVERTEXALG_H
