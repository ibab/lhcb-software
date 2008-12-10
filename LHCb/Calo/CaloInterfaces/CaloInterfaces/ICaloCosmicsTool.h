// $Id: ICaloCosmicsTool.h,v 1.4 2008-12-10 09:57:07 cattanem Exp $
#ifndef ICALOCOSMICSTOOL_H 
#define ICALOCOSMICSTOOL_H 1

// Include files
// from STL
#include <utility>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"

// forward declarations
class DeCalorimeter;

static const InterfaceID IID_ICaloCosmicsTool ( "ICaloCosmicsTool", 1, 1 );

/** @class ICaloCosmicsTool ICaloCosmicsTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-04-07
 */
class ICaloCosmicsTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloCosmicsTool; }

  virtual StatusCode finalize()=0;
  virtual StatusCode processing()=0;

  virtual Gaudi::XYZPoint referencePoint()=0;
  virtual Gaudi::XYZPoint referencePointVariance()=0;
  virtual const std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>& extrema()=0;
  virtual double deposit()=0;
  virtual double phi()=0;
  virtual double phiVariance()=0;
  virtual double asymmetry()=0;
  virtual double asymmetryVariance()=0;
  virtual double slot()=0;
  virtual double time()=0;
  virtual double timeVariance()=0;
  virtual double timeDispersion()=0;
  virtual DeCalorimeter* det()=0;
  virtual StatusCode tupling(unsigned int unit)=0;
  virtual bool tracked()=0;
  virtual bool timed()=0;
  virtual double kernel()=0;
 

protected:

private:


};
#endif // ICALOCOSMICSTOOL_H
