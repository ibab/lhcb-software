#ifndef INCLUDE_BEAMGASPROTOVERTEX_H
#define INCLUDE_BEAMGASPROTOVERTEX_H 1

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

class BeamGasProtoVertex : public HltAlgorithm {

 public: 

  // Standard methods and con/destructor
  BeamGasProtoVertex(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~BeamGasProtoVertex();
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:

  // Member functions
  typedef std::vector<double> VectorD;
  typedef std::vector<Gaudi::XYZPoint> Vector3DPoints;
  void    getMeanAndSigma(VectorD::const_iterator begin, VectorD::const_iterator end, double& sMean, double& sSigma) const;
  double  sigmaBad(double z) const;
  //### function to apply the zMin, zMax and lumi-region exclusion cuts
  bool    passZCuts(double z) const {
    return ((z > m_zTrMin && z < m_zTrMax) && (z < m_zTrExclLRLow || z > m_zTrExclLRUp)) ;
  }
  VectorD get1DVector(const Vector3DPoints& p, int coord) const;
  bool    findProtoVertex(const VectorD& aVector);
  void    printVector(VectorD::const_iterator begin, VectorD::const_iterator end, const std::string& aText) const;


  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_trackSelection; // tracks we'll be looking at
  std::string m_outputSelectionName; // track selection name
  bool        m_boolRevSort;  // use reverse sorting of the z list
  double      m_zTrMin;       // minimal possible z-value (z-list will be trimmed)
  double      m_zTrMax;       // maximal possible z-value (z-list will be trimmed)
  double      m_zTrExclLRLow; // lower limit of the lumi-region exclusion zone
  double      m_zTrExclLRUp;  // upper limit of the lumi-region exclusion zone
  unsigned    m_minNumTracks; // number a bit higher than the number of tracks needed for a trigger
  unsigned    m_minTracksToAccept; // in case of good variance
  unsigned    m_stepSize1;    // size of the main steps
  unsigned    m_stepSize2;    // size of the extension steps
  unsigned    m_maxNumExtends; //max allowed number of extension steps
  double      m_sigmaBadSlope; // asume straight lines symmetric around z=250mm (sigma(250) = sigmaMin = 30. mm)
  double      m_sigmaBadConst1;
  double      m_sigmaBadConst2;
  double      m_degradationFactor; // sets how much the variance can degrade during the expansion steps
};
#endif // INCLUDE_BEAMGASTRIGPROTOVERTEX_H 
