// $Id: PrimaryVertexFinderAlg.h,v 1.2 2002-07-26 19:27:55 gcorti Exp $
#ifndef PRIMARYVERTEXFINDERALG_H 
#define PRIMARYVERTEXFINDERALG_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

///< forward declarations
class IPrimaryVertexFitterTool;


/** @class PrimaryVertexFinderAlg PrimaryVertexFinderAlg.h
 *  
 *
 *  @author Krzysztof Ciba
 *  @date   03/04/2002
 */
class PrimaryVertexFinderAlg : public Algorithm {
public:
  /// Standard constructor
  PrimaryVertexFinderAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrimaryVertexFinderAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: ///< private members

  std::string m_inputContainer; ///< paths in TES
  std::string m_outputContainer;

  std::vector <MyTrack> m_tr;
  std::vector <MyVertex> m_vtx;

  IPrimaryVertexFitterTool* m_pvft;
  // velo det elements :)
  DeVelo* m_velo;
  ////////////////////////////////////////////////////////////////////////
  double m_chi2; ///< chi2 (norm)
  double m_chi2min; ///< minimal value of chi squared (norm)
  double m_chi2cut; ///< how many chi2 is accepted? (norm)
  double m_zcut; ///< abs window in z (mm)
  

  ///////////////////////////////////////////////////////////////////////
  int m_NumberOfTracks; ///< number of tracks in VELO
  int m_NumberOfTracksUsed; ///< number of tracks used to fit vertex
  int m_minNumberOfTracks; ///< minimal tracks to find PV
  int m_maxIteration; ///< how many iterations allowed 
  //////////////////////////////////////////////////////////////////////

};
#endif // PRIMARYVERTEXFINDERALG_H
