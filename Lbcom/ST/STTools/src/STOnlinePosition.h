// $Id: STOnlinePosition.h,v 1.6 2007-01-09 15:02:24 jvantilb Exp $
#ifndef STOnlinePosition_H
#define STOnlinePosition_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// From LHCbKernel
#include "Kernel/ISTClusterPosition.h"

/** @class STOnlinePosition STOnlinePosition.h
 *
 *  Tool for calculating offline cluster position
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STOnlinePosition: public GaudiTool, virtual public ISTClusterPosition {

public: 
   
  /// constructer
  STOnlinePosition( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  /// destructer
  virtual ~STOnlinePosition();

  /// method
  virtual ISTClusterPosition::Info estimate(const LHCb::STCluster* 
                                            aCluster) const;

  virtual ISTClusterPosition::Info 
  estimate(const SmartRefVector<LHCb::STDigit>& digits) const;

  virtual double error(const unsigned int nStrips) const;

private:

  double STOnlinePosition::stripFraction(const double interStripPos) const;
 
  // job options
  int m_nBits;                    ///< Number of bits to decode interstrip frac
  std::vector<double> m_errorVec; ///< Error parametrized by cluster size

};

#endif // STOnlinePosition_H



