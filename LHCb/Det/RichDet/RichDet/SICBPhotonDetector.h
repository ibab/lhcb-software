#ifndef SICBPhotonDetector_h
#define SICBPhotonDetector_h 1

#include <cmath>
#include <vector>
//#include "RichGlobal/Definitions.h"
#include "RichDet/CLHEP_Geometry.h"
using namespace std;

#include "RichDet/Rich.h"
#include "RichDet/PhotonDetector.h"

class Rich;
class PhotonSpectrum;
class RichParameters;
class Pixel;
class Photon;
class RichXLocalPosition;
class SICBPixel;

class SICBPhotonDetector : public PhotonDetector {

public:

  SICBPhotonDetector (const Rich &rich);

  virtual ~SICBPhotonDetector();

  virtual void update (const RichParameters &parameter);

  HepDouble checkInPhotonDPlane( const int side,
                                 const HepPoint3D &p ) const;

  HepPlane3D setPlane (const double tiltx,
                       const double tilty,
                       const double tiltz,
                       const double offsetx,
                       const double offsety,
                       const double xLen,
                       const double yLen,
                       const double thickness,
                       const double zpos);

  HepTransform3D defineGlobalToLocal (const double tiltx,
                                      const double tilty,
                                      const double tiltz,
                                      const double offsetx,
                                      const double offsety,
                                      const double xLen,
                                      const double yLen,
                                      const double thickness,
                                      const double zpos);

  HepTransform3D defineLocalToGlobal (const double tiltx,
                                      const double tilty,
                                      const double tiltz,
                                      const double offsetx,
                                      const double offsety,
                                      const double xLen,
                                      const double yLen,
                                      const double thickness,
                                      const double zpos);

  PhotonSpectrum * photonEfficiency () const;

  void detect (Photon &photon) const;

  Pixel * newPixel (const HepPoint3D &position) const;

  Pixel * newPixel (const LocalPosition &local) const;

  LocalPosition * newLocalPosition (const Pixel &pixel) const;

  LocalPosition * newLocalPosition (const HepPoint3D &position) const;

  HepPoint3D globalPosition (const LocalPosition &local) const;

  HepPoint3D globalPosition (const Pixel &pixel) const;

  double area (const Pixel &pixel) const;

  int nrNoisePixelToGenerate () const;

  Pixel * newNoisePixel () const;

  double expSignalBackground (const Pixel &pixel) const;

  int nsides () const;

protected:

private:

  SICBPhotonDetector();

  SICBPixel * newSICBPixel (const RichXLocalPosition &local) const;

  SICBPixel * newSICBPixel (const HepPoint3D &position) const;

  RichXLocalPosition * newRichXLocalPosition (const SICBPixel &pixel) const;

  RichXLocalPosition * newRichXLocalPosition (const HepPoint3D &position) const;

  HepPoint3D globalPosition (const RichXLocalPosition &local) const;

  HepPoint3D globalPosition (const SICBPixel &pixel) const;

  int side (const HepPoint3D &position) const;

  bool verifyLocal (const int side,
                    const double x,
                    const double y) const;

  // Data Members for Class Attributes

  double tubeSize_;

  double pixelSize_;

  double pixelActive_;

  double pixelNoise_;

  double zPos_;

  double offsetx_;

  double offsety_;

  double width_;

  double length_;

  double thickness_;

  double tiltx_;

  double tilty_;

  double tiltz_;

  vector<HepPlane3D> plane_;

  vector<HepTransform3D> localToGlobal_;

  vector<HepTransform3D> globalToLocal_;

  int nrXTube_;

  int nrYTube_;

  int nrXPixel_;

  int nrYPixel_;

  int m_Nsides;

private:

};

inline int SICBPhotonDetector::side (const HepPoint3D &position) const
{
  int side = ( position.x() > 0 ? 0 : 1 );
  side += ( 4 == this->nsides() ? ( position.y() > 0 ? 0 : 2 ) : 0 );

  return side;
}

inline int SICBPhotonDetector::nsides () const {
  assert (m_Nsides != 0);
  return m_Nsides;
}


inline HepDouble 
SICBPhotonDetector::checkInPhotonDPlane( const int side,
                                         const HepPoint3D &pos ) const
{
  return plane_[side].distance(pos);
}

#endif
