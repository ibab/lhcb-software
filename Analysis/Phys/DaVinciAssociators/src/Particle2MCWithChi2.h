// $Id: Particle2MCWithChi2.h,v 1.4 2004-06-11 15:26:18 phicharp Exp $
#ifndef Particle2MCWithChi2_H 
#define Particle2MCWithChi2_H 1

// Include files
// from STL
#include <string>

// histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"


// from CLHEP
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "DaVinciAssociators/Particle2MCLink.h"
#include "AsctAlgorithm.h"

#include "CLHEP/Units/PhysicalConstants.h"
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

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; }
  
protected:

  /// Calculate Chi2 agreement
  double Chi2ofParticle2MC( const HepVector& pVector, 
                            const HepVector& mcpVector, 
                            const HepSymMatrix& cov);

  /// Fill vector of Particles parameters in "Track" state
  void get6Vector( const Particle* part, HepVector& pVector);
  
  /// Fill vector of MCParticles parameters in "Track" state
  void get6Vector( const MCParticle* mcPart, HepVector& mcpVector, 
                   const double axz);

private:
  bool m_histos;            ///< Flag to fill histograms

  /// Histograms                    
  IHistogram1D* m_hisChi2;           ///< Chi2 histogram
  IHistogram2D* m_hisChi2vsDiffP;    ///< Chi2 vs Dp/p histo
  IHistogram2D* m_hisMinChi2vsDiffP; ///< Min Chi2 vs Dp/p histo
};
#endif // Particle2MCWithChi2_H
