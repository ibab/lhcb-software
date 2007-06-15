// $Id: Particle2MCWithChi2.h,v 1.9 2007-06-15 16:05:37 pkoppenb Exp $
#ifndef Particle2MCWithChi2_H 
#define Particle2MCWithChi2_H 1

// Include files
// from STL
#include <string>

// histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"


// vectors and matrices
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "Kernel/Particle2MCLinker.h"
#include "AsctAlgorithm.h"

#include "GaudiKernel/PhysicalConstants.h"
/** @class Particle2MCWithChi2 Particle2MCWithChi2.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   11/04/2002
 */
class Particle2MCWithChi2 : public AsctAlgorithm {
  friend class AlgFactory<Particle2MCWithChi2>;
  
public:
  /// Standard constructor
  Particle2MCWithChi2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCWithChi2( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Fill vector of MCParticles parameters in "Track" state
  void get6Vector( const LHCb::MCParticle* mcPart, const double axz, 
                   Gaudi::Vector6& mcpVector);

private:

  /// Histograms                    
  bool m_histos;            ///< Flag to fill histograms
  IHistogram1D* m_hisChi2;           ///< Chi2 histogram
  IHistogram2D* m_hisChi2vsDiffP;    ///< Chi2 vs Dp/p histo
  IHistogram2D* m_hisMinChi2vsDiffP; ///< Min Chi2 vs Dp/p histo

  // Chi2 cut
  double m_chi2SpeedUpCut ; ///< Chi2 cut of parent Particle2MCWithChi2 algorithm, if present
   
};
#endif // Particle2MCWithChi2_H
