// $Id: RichG4ReconHpd.h,v 1.5 2008-05-30 13:43:23 gcorti Exp $
#ifndef RICHANALYSIS_RICHG4RECONPMT_H 
#define RICHANALYSIS_RICHG4RECONPMT_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"

/** @class RichG4ReconPmt RichG4ReconPmt.h RichAnalysis/RichG4ReconPmt.h
 *  
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-09
 */
#include <vector>


class RichG4ReconPmt {
public:

  /// Standard constructor
  RichG4ReconPmt( ); 

  virtual ~RichG4ReconPmt( ); ///< Destructor

  Gaudi::XYZPoint ReconHitOnPhCathFromLocalHitCoord( const Gaudi::XYZPoint & aLocalHitCoord);
  
  Gaudi::XYZPoint ConvertLocalHitToPmtSystem(const Gaudi::XYZPoint & aLocalHit);
  double ConvertPmtAnodeZToPmtSystem(double aZcoord );
  Gaudi::XYZPoint ReconHitOnLensSurfaceFromPhCathCoord(const Gaudi::XYZPoint & aLocalPhCathCoord  );
  Gaudi::XYZPoint ReconHitOnLensSurfaceCheatedFromPhCathCoord(const Gaudi::XYZPoint & aLocalPhCathCoord ,
                                        const Gaudi::XYZPoint & aLocallLensExtCoord  );
  
protected:

private:

  //  int m_NumPmtInternalDimensions;
  
  //std::vector<double> m_PmtInternalDimensions;

    

  double m_PmtAnodeXSize;
  double m_PmtAnodeYSize;
  double m_PmtAnodeZSize;
  double m_PmtAnodeZLocation;
  double m_PmtPhCathodeToAnodeMaxDist;

  double m_PmtAnodeSurfaceZLocation;
  double m_PmtPhCathZLocation;
  double m_PmtQwSurfaceZLocation;
  
  double m_PmtPixelXSize;
  double m_PmtPixelYSize;
  int m_PmtPixelXNumCol;
  int m_PmtPixelXNumRow;
  double m_PmtLensMagnificationRatio;
  double m_PmtLensRoc;
  double m_PmtQWToCenterZDist;
  


};

inline double RichG4ReconPmt::ConvertPmtAnodeZToPmtSystem(double aZcoord ) 
{
 return  (aZcoord + m_PmtAnodeZLocation); 
};


#endif // RICHANALYSIS_RICHG4RECONPMT_H
