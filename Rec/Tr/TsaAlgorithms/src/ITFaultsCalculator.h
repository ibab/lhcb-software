// $Id: ITFaultsCalculator.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _ITFaultsCalculator_H
#define _ITFaultsCalculator_H

/** @class ITFaultsCalculator ITFaultsCalculator.h
* How many hits do we expect in in the IT
* @author M. Needham
* @date 30.05.2004
*/ 

#include "GaudiAlg/GaudiTool.h"
#include "ITsaFaultsCalculator.h"
#include "Kernel/Point3DTypes.h"
#include "TsaKernel/Line3D.h"
class DeSTDetector;
class DeSTLayer;



class ITFaultsCalculator: public GaudiTool,
                     virtual public ITsaFaultsCalculator {

public:

  /// constructer
  ITFaultsCalculator(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  // destructer
  virtual ~ITFaultsCalculator();

  /// init
  virtual StatusCode initialize();

  // execute method
  virtual double nFaults(const Tsa::Parabola& parab,
                         const Tsa::Line& line, 
                         const std::vector<Tsa::Cluster*>& clusters) const;

private:

  bool haveCluster(const DeSTLayer* layer, 
                   const std::vector<Tsa::Cluster*>& clusters) const;
  

  bool insideLayer(const DeSTLayer* layer,  
		   const Tsa::Line3D& line) const;

  bool insideActive(const DeSTLayer* layer,  
                    const Tsa::Line3D& line) const;

  double m_efficiency;

  double m_tolX;
  double m_tolY;
  double m_tolZ;

  Gaudi::XYZPoint m_tol;

  DeSTDetector* m_tracker;


};

#endif // _ITFaultsCalculator_H
