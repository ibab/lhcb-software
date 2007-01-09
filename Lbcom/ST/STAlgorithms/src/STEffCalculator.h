// $Id: STEffCalculator.h,v 1.3 2007-01-09 15:34:37 jvantilb Exp $
#ifndef STEFFCALCULATOR_H
#define STEFFCALCULATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// local interface
#include "ISTEffCalculator.h"

/** @class STEffCalculator STEffCalculator.h STAlgorithms/STEffCalculator.h
 *
 *  Class for applying inefficiency for Silicon Tracker 
 *
 *  @author M.Needham
 *  @date   21/5/2002
 */

class STEffCalculator : public GaudiTool, virtual public ISTEffCalculator {

public:

  ///  Constructor
  STEffCalculator(const std::string& type, const std::string& name, 
                  const IInterface* parent);

  /// destructor
  virtual ~STEffCalculator();

  virtual StatusCode initialize();

  /// tool 'operation' - returns false if hit is to be killed 
  virtual bool accept();
  
private:

  // smart interface to generator
  SmartIF<IRndmGen> m_GenEff;
 
  // Job option: 
  double m_eff;  ///< ST readout channel efficiency
 
};

#endif // STEFFCALCULATOR_H
