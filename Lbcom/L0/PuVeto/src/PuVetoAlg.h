// $Id: PuVetoAlg.h,v 1.1.1.1 2002-01-29 09:37:12 ocallot Exp $
#ifndef PUVETOALG_H 
#define PUVETOALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

/** @class PuVetoAlg PuVetoAlg.h
 *  Computes the Veto algorithm, from the PuVetoDigits
 *
 *  @author Olivier Callot
 *  @date   11/01/2002
 */
class PuVetoAlg : public Algorithm {
public:
  /// Standard constructor
  PuVetoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PuVetoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// fill the histogram
  void fillHisto( ObjectVector<L1Buffer>* digs );

  /// return the bin number in the variable size histogram;
  long    zBin( double z ) {
    int bin = -1;
    if ( m_lowBound[0] < z ) {
      for ( unsigned int j=0 ; m_step.size() > j ; j++ ) {
        if ( m_lowBound[j+1] > z ) {
          bin = m_nBin[j] + (int)( ( z-m_lowBound[j]) / m_step[j] );
          break;
        }
      }
    }
    return bin;
  }

  /// Returns the position of the maximum, and as argument the peak height 
  /// and the integral.
  double peakValue( double& height, double& sum, double& width ) ;

  /// Mask the hits contributing to a peak at a given vertex position
  void maskHits(  ObjectVector<L1Buffer>* digs, double zVertex, double zTol );

private:
  std::string    m_inputContainer;
  DeVelo*        m_velo;
  double         m_lowThreshold;
  double         m_highThreshold;
  double         m_highPosition;
  double         m_secondPosition;

  // Storage for the variable bound histogram

  std::vector<double> m_lowBound;
  std::vector<double> m_step;
  std::vector<int>    m_nBin;
  int                 m_totBin;
  std::vector<double> m_hist;
  std::vector<double> m_binCenter;
};
#endif // PUVETOALG_H
