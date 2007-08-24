// $Id: PatTTMagnetTool.h,v 1.2 2007-08-24 12:44:07 jonrob Exp $ 
#ifndef PATTTMAGNETTOOL_H
#define PATTTMAGNETTOOL_H 1


// Include files
#include "GaudiAlg/GaudiTool.h"

// Forward declarations
class IMagneticFieldSvc;
class TableForFunction;
#include "STDet/DeSTDetector.h"


static const InterfaceID IID_PatTTMagnetTool ( "PatTTMagnetTool", 1, 0 );


namespace Tf {

  /** @class PatTTMagnetTool PatTTMagnetTool.h newtool/PatTTMagnetTool.h
   *
   *  Magnet tool for Pat
   *
   *  @author Mariusz Witek
   *  @date   2006-09-25
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PatTTMagnetTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatTTMagnetTool; }

    /// Standard constructor
    PatTTMagnetTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /// Standard Destructor
    virtual ~PatTTMagnetTool();

    virtual StatusCode initialize();

    /// Actual operator function
    double bdlIntegral(double ySlopeVelo,double zOrigin,double zVelo);
    double zBdlMiddle(double ySlopeVelo,double zOrigin,double zVelo);
    double dist2mom(double ySlope);
    void dxNormFactorsTT(double ySlope, std::vector<double>& nfact);

    double zMidTT();
    double zMidField();
    double averageDist2mom();
    void prepareBdlTables();
    void prepareDeflectionTables();

  protected:
    void f_bdl(double slopeY, double zOrigin , double zStart, double zStop);

  private:

    double m_BdlTrack;
    double m_zHalfBdlTrack;
    std::vector<double> m_zLayers;

    std::vector<double> m_bdlTmp, m_zTmp;

    /// pointer to mag field service
    IMagneticFieldSvc* m_magFieldSvc;
    TableForFunction* m_lutBdl;
    TableForFunction* m_lutZHalfBdl;

    double m_zCenterTT;
    double m_zMidField;

    double m_dist2mom;
    TableForFunction* m_lutDxLay;
    TableForFunction* m_lutDxToMom;

    std::vector<double> m_lutVar; // auxiliary vector used as argument

    DeSTDetector*       m_STDet;         ///< ST Detector element

    bool m_noField;
    double m_bdlIntegral_NoB;
    double m_zBdlMiddle_NoB;
    double m_zMidField_NoB;
    double m_averageDist2mom_NoB;

  };
}

#endif // PATTTMAGNETTOOL_H
