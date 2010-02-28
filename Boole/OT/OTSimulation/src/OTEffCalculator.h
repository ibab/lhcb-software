// $Id: OTEffCalculator.h,v 1.9 2010-02-28 11:56:25 nserra Exp $
#ifndef OTSIMULATION_OTEFFCALCULATOR_H
#define OTSIMULATION_OTEFFCALCULATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"

// OTSimulation
#include "IOTEffCalculator.h"

// Forward declaration
namespace LHCb 
{
  class MCOTDeposit;
}

/** @class OTEffCalculator OTEffCalculator.h "OTSimulation/OTEffCalculator.h"
 *
 *  Tool for applying single cell efficiency for Outer Tracker.
 *  This tool is used to simulate the geometric efficiency of 
 *  of 5 mm straw. The parameterization used is:
 * 
 *    @f$ f(l) = \varepsilon_{core} (1 - e^{\rho l}) @f$
 *
 *  (prob for poisson > 0  - ie you have one or more cluster).
 *  The test-beam data indicates this form is reasonable.
 *
 *  @author M.Needham
 *  @date   21/10/2001
 */

class OTEffCalculator : public GaudiTool, virtual public IOTEffCalculator {

public:

  ///  Constructor
  OTEffCalculator(const std::string& type, 
                  const std::string& name, 
                  const IInterface* parent);

  /// destructor
  virtual ~OTEffCalculator();

  /// initialize method
  virtual StatusCode initialize();

  /// tool 'operation' - returns false if hit is to be killed 
  void calculate(LHCb::MCOTDeposit* aDeposit, const double slope, bool& accept) const;
  
private:

  /// parameterization of efficiency
  double effParamFunc(const double driftDistance, const double slope) const;

  SmartIF<IRndmGen> m_genEff;  ///< smart interface to generator
  double m_cellRadius;         ///< cell size - should go elsewhere
  double m_etaZero;            ///< efficiency of cell core
  double m_rho;                ///< primary ionization density

};

#endif // OTSIMULATION_OTEFFCALCULATOR_H
