// $Id: BdlTool.h,v 1.3 2007-10-05 11:50:50 cattanem Exp $
#ifndef MAGNET_BDLTOOL_H
#define MAGNET_BDLTOOL_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

// From LHCbKernel
#include "Kernel/IBdlTool.h"

// Forward declarations
class IMagneticFieldSvc;
class LutForBdlFunction;

static const InterfaceID IID_PatVTTBdlTool("PatVTTBdlTool", 1 , 0);

/** @class BdlTool BdlTool.h
 * Field integral using lookup tables.
 * @author Mariusz Witek
 *
*/
class BdlTool : public GaudiTool,
                virtual public IBdlTool {

public:

  /// Standard Constructor
  BdlTool(const std::string& type,
          const std::string& name,
          const IInterface* parent);

  /// Standard Destructor
  virtual ~BdlTool();

  /// Initialize
  StatusCode initialize();

  /// Finalize()
  StatusCode finalize();

  /// Actual operator function
  double bdlIntegral(double ySlopeVelo,double zOrigin,double zVelo);
  double zBdlMiddle(double ySlopeVelo,double zOrigin,double zVelo);

protected:
  void f_bdl(double slopeY, double zOrigin , double zStart, double zStop);

private:

  double m_BdlTrack;
  double m_zHalfBdlTrack;

  std::vector<double> m_bdlTmp, m_zTmp;

  /// pointer to mag field service
  IMagneticFieldSvc* m_magFieldSvc;
  LutForBdlFunction* m_lutBdl;
  LutForBdlFunction* m_lutZHalfBdl;

  double m_zCenterTT;

};

#endif // MAGNET_BDLTOOL_H
