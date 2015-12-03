#ifndef STSPILLOVERTOOL_H 
#define STSPILLOVERTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrackInterfaces/ISTSpilloverTool.h"

// local
#include "boost/assign/list_of.hpp"

// ROOT
#include "TH2D.h"
#include "TString.h"

// forward declarations
namespace LHCb
{
  class Track;
  class FitNode; 
}

/** @class STSpilloverTool STSpilloverTool.h
 * 
 *
 *  @author Vincenzo Battista
 *  @date   2015-06-26
 *  Purpose: compute PDF(ADC,costheta|central)/PDF(ADC|spillover) ratio 
 *
 */

//static const InterfaceID IID_STSpilloverTool ( "STSpilloverTool", 1, 0 );

class STSpilloverTool : public GaudiTool, virtual public ISTSpilloverTool{

public: 

  // Return the interface ID
  //static const InterfaceID& interfaceID() { return IID_STSpilloverTool; }

  /// Standard constructor
  STSpilloverTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~STSpilloverTool( ); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization

  /// Return PDF(ADC,costheta|central)/PDF(ADC|spillover) starting from a track node
  double pdfRatio( const LHCb::FitNode* fNode );

protected:

private:

  /// Configurables quantities
  std::string m_detType;  ///< Detector type (deprecated: uses whole ST in any case)
  std::vector<float> m_adcLowEdges;  ///< Array of ADC bins
  std::vector<float> m_cosThetaLowEdges; ///< Array of costheta bins
  std::vector<double> m_TTweightVals;   ///< Array of TT weights
  std::vector<double> m_ITweightVals;  ///< Array of IT weights
  std::vector<int> m_weightBins;  ///< Array of weight bins

  /// Pointers to wrap TH2D
  float *adcLowEdges;
  float *cosThetaLowEdges;

  /// Weight map binning
  int adcBins;
  int cosThetaBins;

  /// Weight maps
  TH2D *TTWeightMap;
  TH2D *ITWeightMap;

  double cosTheta( const LHCb::State & aState ); ///<  Function to get track angle wrt z direction (perpendicular to ST silicon surface)

};

#endif // STSPILLOVERTOOL_H
