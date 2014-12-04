// $Id: RichG4ReconHpd.h,v 1.5 2008-05-30 13:43:23 gcorti Exp $
#ifndef RICHANALYSIS_RICHG4RECONHPD_H 
#define RICHANALYSIS_RICHG4RECONHPD_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"

/** @class RichG4ReconHpd RichG4ReconHpd.h RichAnalysis/RichG4ReconHpd.h
 *  
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-09
 */
#include <vector>


class RichG4ReconHpd {
public:

  /// Standard constructor
  RichG4ReconHpd( ); 

  virtual ~RichG4ReconHpd( ); ///< Destructor

  Gaudi::XYZPoint ReconHitOnPhCathFromLocalHitCoord( const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr);
  
  Gaudi::XYZPoint ConvertLocalHitToHpdSystem(const Gaudi::XYZPoint & aLocalHit);
  double ConvertHpdSiliconZToHpdSystem(double aZcoord );
  double GetCorrectionForQwRefraction( double RCathode  );
  Gaudi::XYZPoint GetLocalPointWithQwCorr(const Gaudi::XYZPoint & aPhCathPoint  );
  
protected:

private:

  int m_NumHpdInternalDimensions;
  
  std::vector<double> m_HpdInternalDimensions;

  int m_NumHpdCrossFocusParameters;  
  std::vector<double> m_HpdCrossFocusParameters;
  double m_HpdSiDetXSize;
  double m_HpdSiDetYSize;
  double m_HpdSiDetZSize;
  double m_HpdSiDetZLocation;
  double m_HpdPhCathodeToSiDetMaxDist;
  double m_HpdPhCathodeRad;
  double m_HpdPixelXSize;
  double m_HpdPixelYSize;
  int m_HpdPixelXNumCol;
  int m_HpdPixelXNumRow;

  //  double m_HpdPixelXNumCol;
  // double m_HpdPixelXNumRow;
  int m_NumParamHpdQwRefractCorr;
  std::vector<double> m_HpdQwRefractCorr;
  double m_HpdQwOuterRadius;
  
};

inline double RichG4ReconHpd::ConvertHpdSiliconZToHpdSystem(double aZcoord ) 
{
 return  (aZcoord + m_HpdSiDetZLocation); 
}


#endif // RICHANALYSIS_RICHG4RECONHPD_H
