// $Id: RichG4ReconHpd.h,v 1.1 2004-02-04 13:53:01 seaso Exp $
#ifndef RICHANALYSIS_RICHG4RECONHPD_H 
#define RICHANALYSIS_RICHG4RECONHPD_H 1

// Include files

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

  HepPoint3D ReconHitOnPhCathFromLocalHitCoord( HepPoint3D aLocalHitCoord);
  
  HepPoint3D ConvertLocalHitToHpdSystem(HepPoint3D aLocalHit);
  double ConvertHpdSiliconZToHpdSystem(double aZcoord );
  
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
  double m_HpdPixelXNumCol;
  double m_HpdPixelXNumRow;
  



};
#endif // RICHANALYSIS_RICHG4RECONHPD_H
