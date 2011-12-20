// $Id: $
#ifndef OTSIMULATION_OTLUTDOUBLEPULSETOOL_H
#define OTSIMULATION_OTLUTDOUBLEPULSETOOL_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "AIDA/IHistogram2D.h"
#include "IOTDoublePulseTool.h"


#include "TH2D.h"

class TH2D;

/** @class OTLUTDoublePulseCreatorTool OTLUTDoublePulseTool.h \
 *         "OTSimulation/OTLUTDoublePulseTool.h"
 *
 *  Outer tracker Look Up Table double pulse tool.
 *
 *  @author A Bien
 *  @date   18/10/2011
 */

class OTLUTDoublePulseTool : public GaudiTool,
                             virtual public IOTDoublePulseTool {

public:

  /// constructer
  OTLUTDoublePulseTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  /// destructer
  virtual ~OTLUTDoublePulseTool();

  /// initialize method
  virtual StatusCode initialize();

  ///create method
  virtual double timeOffset(const LHCb::MCOTDeposit* deposit) const;

private:

  double findSpillTime(const LHCb::MCHit* aHit) const;

  IRndmGenSvc* randSvc() const;
              
  mutable Rndm::Numbers  m_flat;
  mutable IRndmGenSvc*   m_rgs;
  std::vector<TH2D*>     m_histos;

  // job options:
  std::vector<std::string> m_spillNames;  ///< Spill names
  std::vector<std::string> m_spillVector; ///< Spills to digitize  
  std::vector<double>      m_spillTimes;  ///< Spill times
  std::string              m_histo_path;  ///< Path in the ROOT file
  double                   m_shift;       ///< Additional time shift of the LUT spectrum

};

#endif // OTSIMULATION_OTLUTDOUBLEPULSETOOL_H
