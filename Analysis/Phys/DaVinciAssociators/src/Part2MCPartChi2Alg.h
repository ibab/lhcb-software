// $Id: Part2MCPartChi2Alg.h,v 1.1.1.1 2002-04-25 15:57:35 phicharp Exp $
#ifndef PART2MCPARTCHI2ALG_H 
#define PART2MCPARTCHI2ALG_H 1

// Include files
// from STL
#include <string>

// histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"


// from CLHEP
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from Event
#include "LHCbEvent/Event.h"

// from LHCbKernel
#include "DaVinciAssociators/Part2MCPartAsct.h"

#include "CLHEP/Units/PhysicalConstants.h"
/** @class Part2MCPartChi2Alg Part2MCPartChi2Alg.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   11/04/2002
 */
class Part2MCPartChi2Alg : public Algorithm {
  friend AlgFactory<Part2MCPartChi2Alg>;
  
public:
  /// Standard constructor
  Part2MCPartChi2Alg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Part2MCPartChi2Alg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  std::string outputData() { return m_outputData; }
  double Part2MCPartChi2( const HepVector& pVector, 
                          const HepVector& mcpVector, const HepSymMatrix& cov);
  void get6Vector( const AxPartCandidate* axPart, HepVector& pVector);
  void get6Vector( const MCParticle* mcPart, HepVector& mcpVector, 
                   const double axz);
  
protected:

private:
  std::string m_outputData;
  double m_chi2;
  // Histograms
  IHistogram1D* m_hisChi2;
  IHistogram2D* m_hisChi2vsDiffP;
  IHistogram2D* m_hisMinChi2vsDiffP;

};
#endif // PART2MCPARTCHI2ALG_H
