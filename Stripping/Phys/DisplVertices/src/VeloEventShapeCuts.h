#ifndef DISPLVERTICES_VELOEVENTSHAPECUTS_H
#define DISPLVERTICES_VELOEVENTSHAPECUTS_H 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "VeloDet/DeVelo.h"

/** @class VeloEventShapeCuts VeloEventShapeCuts.h DisplVertices/VeloEventShapeCuts.h
 *
 * Global event cuts to reject events with high Velo track (and vertex) ghost rates
 *
 * @author Wouter Hulsbergen
 * @author Pieter David
 * @date   2013-12-29
 */
class VeloEventShapeCuts : public GaudiHistoAlg {
public:
  /// Standard constructor
  VeloEventShapeCuts( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloEventShapeCuts();                ///< Destructor

  virtual StatusCode initialize();              ///< Algorithm initialization
  virtual StatusCode execute   ();              ///< Algorithm execution
  virtual StatusCode finalize  ();              ///< Algorithm finalization
private:
  double m_maxVeloRatio;
  double m_maxPhiVectorSize;
  DeVelo* m_velo;

  double calculateVeloRatio() const;
  double calculatePhiVectorSize() const;

  // monitoring
  AIDA::IHistogram1D* m_hVeloRatio;
  AIDA::IHistogram1D* m_hPhiVectorSize;
};
#endif // DISPLVERTICES_VELOEVENTSHAPECUTS_H
