#ifndef PRFITTOOL_H 
#define PRFITTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GaudiKernel/Point3DTypes.h"

#include "PrFitPolinomial.h"

static const InterfaceID IID_PrFitTool ( "PrFitTool", 1, 0 );

/** @class PrFitTool PrFitTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2006-12-08
 */
class PrFitTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrFitTool; }

  /// Standard constructor
  PrFitTool( const std::string& type, 
           const std::string& name,
           const IInterface* parent);

  virtual ~PrFitTool( ); ///< Destructor

  void fitLine( std::vector<Gaudi::XYZPoint>& hit, int mode, 
                double z0, double& a, double& b);

  void fitParabola( std::vector<Gaudi::XYZPoint>& hit, int mode, 
                    double z0, double& a, double& b, double& c);

  void fitCubic( std::vector<Gaudi::XYZPoint>& hit, int mode, 
                 double x0, double& a, double& b, double& c, double& d);


protected:

private:

};
#endif // PRFITTOOL_H
