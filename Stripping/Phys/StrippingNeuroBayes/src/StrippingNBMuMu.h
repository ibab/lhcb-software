// $Id: $
#ifndef STRIPPINGNBMUMU_H 
#define STRIPPINGNBMUMU_H 1

#include "Kernel/DaVinciHistoAlgorithm.h"
#include "Kernel/FPEGuard.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "math.h"
#include <string>
#include <sstream>

// NeuroBayes (only on linux with gcc for the moment)
#ifdef __GNUC__
#include "NeuroBayesExpert.hh"
#include "nb_param.hh"
#endif


/** @class StrippingNBMuMu StrippingNBMuMu.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2010-10-15
 */
class StrippingNBMuMu : public DaVinciHistoAlgorithm 
{

public: 

  /// Standard constructor
  StrippingNBMuMu( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StrippingNBMuMu( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  StatusCode getInputVar(const LHCb::Particle& particle);
  double     minIPChi2(const LHCb::Particle& particle);

  double     m_NetworkCut;
  bool       m_PlotHisto;

  double     m_PlotMassMin;
  double     m_PlotMassMax;
  int        m_PlotNBins;

#ifdef __GNUC__
  Expert*  m_NBExpert;  
  float*   m_inArray; 
#endif

  std::string m_ExpertiseName;
  std::string m_netVersion;
  std::string m_pvLocation;

};

#endif // STRIPPINGNBMUMU_H
