// $Id: STSmearedPosition.h,v 1.1 2010-02-26 09:08:10 mneedham Exp $
#ifndef STSmearedPosition_H
#define STSmearedPosition_H 1

// Gaudi
#include "Kernel/STToolBase.h"

// From LHCbKernel
#include "TrackInterfaces/ISTClusterPosition.h"

// rndm numbers
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"


/** @class STSmearedPosition STSmearedPosition.h
 *
 *  Tool for smearing cluster position, it takes as input the 
 *  result of the online or offline tool and then adds an error 
 *  in quadrature based on the cluster size
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STSmearedPosition: public ST::ToolBase, virtual public ISTClusterPosition {

public: 
   
  /// constructer
  STSmearedPosition( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  /// destructer
  virtual ~STSmearedPosition();

  /// initialize
  virtual StatusCode initialize();

  /// method
  virtual ISTClusterPosition::Info estimate(const LHCb::STCluster* 
                                            aCluster) const;

  virtual ISTClusterPosition::Info estimate(const LHCb::STCluster* aCluster,
                                            const LHCb::StateVector& ) const;

  virtual ISTClusterPosition::Info 
  estimate(const SmartRefVector<LHCb::STDigit>& digits) const;

  virtual double error(const unsigned int nStrips) const;

  virtual double error(const unsigned int nStrips, 
                       const LHCb::StateVector&) const;

private:

  std::string m_baseToolName;
  std::string m_baseToolType;
  ISTClusterPosition* m_baseTool;

  std::vector<double> m_corrections;
    
  SmartIF<IRndmGen> m_gaussDist;

  double applySmear(double value, unsigned int nStrips) const ;

};

#endif // STSmearedPosition_H



