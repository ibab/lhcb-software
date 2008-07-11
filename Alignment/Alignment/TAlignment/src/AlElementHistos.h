#ifndef TALIGNMENT_ALELEMENTHISTOS_H
#define TALIGNMENT_ALELEMENTHISTOS_H

class IHistogram1D ;
class IHistogram2D ;
class AlignmentElement ;
class GaudiHistoAlg ;

struct AlElementHistos
{
  AlElementHistos( GaudiHistoAlg& parent, const AlignmentElement& elem, size_t niter) ;
  void reset() ;
  IHistogram2D* m_resHisto;
  IHistogram2D* m_pullHisto;
  IHistogram1D* m_nHitsHisto;
  IHistogram1D* m_deltaTxHisto;
  IHistogram1D* m_deltaTyHisto;
  IHistogram1D* m_deltaTzHisto;
  IHistogram1D* m_deltaRxHisto;
  IHistogram1D* m_deltaRyHisto;
  IHistogram1D* m_deltaRzHisto;
  IHistogram2D* m_autoCorrHisto;
  IHistogram1D* m_residualPullHistos[6] ;
} ;


#endif

