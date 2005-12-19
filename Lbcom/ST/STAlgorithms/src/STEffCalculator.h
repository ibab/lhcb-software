// $Id: STEffCalculator.h,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
#ifndef _ITEFFCALCULATOR_H
#define _ITEFFCALCULATOR_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

#include "STAlgorithms/ISTEffCalculator.h"

/** @class STEffCalculator STEffCalculator.h STAlgorithms/STEffCalculator.h
 *
 *  Class for applying inefficiency for Inner Tracker 
 *
 *  @author M.Needham
 *  @date   21/5/2002
 */

class STEffCalculator : public GaudiTool, virtual public ISTEffCalculator {

public:

  ///  Constructor
  STEffCalculator(const std::string& type, 
                  const std::string& name, 
                  const IInterface* parent);

  /// destructor
  virtual ~STEffCalculator();

  virtual StatusCode initialize();

  /// tool 'operation' - returns false if hit is to be killed 
  virtual bool accept();
  
private:

  // smart interface to generator
  SmartIF<IRndmGen> m_GenEff;
 
  // jobOptions  
  double m_eff;  
 
};

#endif // _STEFFCALCULATOR_H









