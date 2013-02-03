// $Id: PatUTMagnetTool.h,v 1.5 2009-10-30 13:20:50 wouter Exp $ 
#ifndef PATUTMAGNETTOOL_H
#define PATUTMAGNETTOOL_H 1


// Include files
#include "GaudiAlg/GaudiTool.h"

// Forward declarations
class IMagneticFieldSvc;
class PatTableForFunction;
#include "STDet/DeSTDetector.h"


static const InterfaceID IID_PatUTMagnetTool ( "PatUTMagnetTool", 1, 0 );



  /** @class PatUTMagnetTool PatUTMagnetTool.h newtool/PatUTMagnetTool.h
   *
   *  Magnet tool for Pat
   *
   *  @author Mariusz Witek
   *  @date   2006-09-25
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PatUTMagnetTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatUTMagnetTool; }

    /// Standard constructor
    PatUTMagnetTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /// Standard Destructor
    virtual ~PatUTMagnetTool();

    virtual StatusCode initialize();

    /// Actual operator function
    double bdlIntegral(double ySlopeVelo,double zOrigin,double zVelo);
    double zBdlMiddle(double ySlopeVelo,double zOrigin,double zVelo);
    double dist2mom(double ySlope);
    void dxNormFactorsUT(double ySlope, std::vector<double>& nfact);

    double zMidUT();
    double zMidField();
    double averageDist2mom();
    void prepareBdlTables();
    void prepareDeflectionTables();
    StatusCode updateField() ;

  protected:
    void f_bdl(double slopeY, double zOrigin , double zStart, double zStop);
    
  private:

    double m_BdlTrack;
    double m_zHalfBdlTrack;
    std::vector<double> m_zLayers;

    std::vector<double> m_bdlTmp, m_zTmp;

    /// pointer to mag field service
    IMagneticFieldSvc* m_magFieldSvc;
    PatTableForFunction* m_lutBdl;
    PatTableForFunction* m_lutZHalfBdl;

    double m_zCenterUT;
    double m_zMidField;

    double m_dist2mom;
    PatTableForFunction* m_lutDxLay;
    PatTableForFunction* m_lutDxToMom;

    std::vector<double> m_lutVar; // auxiliary vector used as argument

    DeSTDetector*       m_STDet;         ///< ST Detector element
    std::string m_detLocation;

    bool m_noField;
    double m_bdlIntegral_NoB;
    double m_zBdlMiddle_NoB;
    double m_zMidField_NoB;
    double m_averageDist2mom_NoB;

  };


#endif // PATUTMAGNETTOOL_H
