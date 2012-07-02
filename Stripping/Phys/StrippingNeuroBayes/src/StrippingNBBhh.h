// $Id: $
#ifndef STRIPPINGNBBHH_H 
#define STRIPPINGNBBHH_H 1

#include "Kernel/DaVinciHistoAlgorithm.h"
#include "Kernel/FPEGuard.h"

#include <string>

// NeuroBayes (only on linux with gcc for the moment)
#ifdef __GNUC__
#include "NeuroBayesExpert.hh"
#include "nb_param.hh"
#endif

/** @class StrippingNBBhh.cpp StrippingNBBhh.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2011-01-25
 */
class StrippingNBBhh : public DaVinciHistoAlgorithm
{

public: 

  /// Standard constructor
  StrippingNBBhh( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StrippingNBBhh( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  bool       getInputVar(const LHCb::Particle& particle);

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

#endif // STRIPPINGNBBHH_H
