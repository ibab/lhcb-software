// $Id: $
#ifndef VERTEXCHECKER_H 
#define VERTEXCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VertexChecker VertexChecker.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2011-11-16
 */
class VertexChecker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VertexChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VertexChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  struct LowerByZ {
    bool operator() ( const LHCb::MCVertex* lhs, const LHCb::MCVertex* rhs ) const { 
      return lhs->position().z() < rhs->position().z(); 
    }
  };

  int mcBin( LHCb::MCVertex* vert ) {
    int nPart = vert->products().size();
    int binNum = nPart/m_binSizeMC;
    if ( binNum >= m_nbBin ) binNum = m_nbBin-1;
    return binNum;
  }

  int recBin( LHCb::RecVertex* vert ) {
    int nPart = vert->tracks().size();
    int binNum = nPart/m_binSizeRec;
    if ( binNum >= m_nbBin ) binNum = m_nbBin-1;
    return binNum;
  }

protected:

private:
  std::string m_inputLocation;
  double m_deltaZForMatch;
  double m_minIPForTrack;
  double m_maxIPForTrack;
  double m_maxRadius;
  int    m_nbBin;
  int    m_binSizeMC;
  int    m_binSizeRec;

  std::vector<int> m_mcVertices;
  std::vector<int> m_mcFound;
  std::vector<int> m_recVertices;
  std::vector<int> m_recFake;

  double m_s0;
  double m_sx;
  double m_sx2;
  double m_sy;
  double m_sy2;
  double m_sz;
  double m_sz2;
  int m_nbLargeIP;
  int m_nEvent;
};
#endif // VERTEXCHECKER_H
