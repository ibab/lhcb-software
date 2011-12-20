// $Id: $
#ifndef OTSIMULATION_OTSIMPLEDOUBLEPULSETOOL_H
#define OTSIMULATION_OTSIMPLEDOUBLEPULSETOOL_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "AIDA/IHistogram2D.h"
#include "IOTDoublePulseTool.h"

/** @class OTSimpleDoublePulseCreatorTool OTSimpleDoublePulseTool.h \
 *         "OTSimulation/OTSimpleDoublePulseTool.h"
 *
 *  Outer tracker simple double pulse tool.
 *
 *  @author A Bien
 *  @date   18/10/2011
 */

class OTSimpleDoublePulseTool : public GaudiTool,
                                virtual public IOTDoublePulseTool {
  
public:

  /// constructer
  OTSimpleDoublePulseTool(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  
  /// destructer
  virtual ~OTSimpleDoublePulseTool();

  /// initialize method
  virtual StatusCode initialize();

  ///create method
  virtual double timeOffset(const LHCb::MCOTDeposit* deposit) const;

private:

  IRndmGenSvc* randSvc() const;
              
  mutable Rndm::Numbers m_flat;
  mutable IRndmGenSvc*  m_rgs;
  double                m_timeOffset;             ///< double pulse time offset
  double                m_doublePulseProbability; ///< double pulse prob

};

#endif // OTSIMULATION_OTSIMPLEDOUBLEPULSETOOL_H
