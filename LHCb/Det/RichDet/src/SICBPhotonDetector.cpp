#include "RichDet/CLHEP_Geometry.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/RichParameters.h"
#include "RichDet/Pixel.h"
#include "RichDet/SICBPhotonDetector.h"
#include "RichDet/RichXLocalPosition.h"
#include "RichDet/SICBPixel.h"

#include "RichDet/Trajectory.h"

SICBPhotonDetector::SICBPhotonDetector (const Rich &rich)
  : PhotonDetector(rich),
    plane_(4),
    localToGlobal_(4),
    globalToLocal_(4)
{
}

SICBPhotonDetector::~SICBPhotonDetector() { }

HepPlane3D SICBPhotonDetector::setPlane (const double tiltx,
                                         const double tilty,
                                         const double tiltz,
                                         const double offsetx,
                                         const double offsety,
                                         const double xLen,
                                         const double yLen,
                                         const double thickness,
                                         const double zpos)
{
  HepPlane3D detPlane(0., 0., 1., 0.);
  return detPlane.transform( defineLocalToGlobal(tiltx, tilty, tiltz, offsetx,
                                                 offsety,xLen, yLen, thickness, 
                                                 zpos) );
}


HepTransform3D SICBPhotonDetector::defineLocalToGlobal( const double tiltx,
                                                        const double tilty,
                                                        const double tiltz,
                                                        const double offsetx,
                                                        const double offsety,
                                                        const double xLen,
                                                        const double yLen,
                                                        const double thickness,
                                                        const double zpos)
{
  HepTransform3D rotTrans = HepRotateY3D(tilty) * 
    ( HepRotateX3D(tiltx) * HepRotateZ3D(tiltz) );
    
  // See where centre of detector planes are and adjust translation
  // to account for shifts.
  HepPoint3D centrePoint;
  if (4 == this->nsides() ) {   
    centrePoint = HepPoint3D(0.5*xLen, 0.5*yLen, -0.5*thickness);
  } else {
    centrePoint = HepPoint3D(0.5*xLen, 0.0,       0.5*thickness);
  }
  centrePoint.transform(rotTrans);
  return HepTranslate3D(offsetx-centrePoint.x(),
                        offsety-centrePoint.y(),
                        zpos-centrePoint.z()) * rotTrans;
}

HepTransform3D SICBPhotonDetector::defineGlobalToLocal( const double tiltx,
                                                        const double tilty,
                                                        const double tiltz,
                                                        const double offsetx,
                                                        const double offsety,
                                                        const double xLen,
                                                        const double yLen,
                                                        const double thickness,
                                                        const double zpos)
{
  return defineLocalToGlobal(tiltx, tilty, tiltz, offsetx, offsety, 
                             xLen, yLen, thickness, zpos).inverse();
}

PhotonSpectrum * SICBPhotonDetector::photonEfficiency () const
{
  //
  // Quantum efficiency for hamamatsu bialkali phototube
  //

  double hamamatsu[] =
  { 0.040, 0.120, 0.200, 0.260, 0.280, 0.280, 0.270, 0.260,
    0.255, 0.245, 0.235, 0.225, 0.215, 0.205, 0.200, 0.195,
    0.190, 0.185, 0.180, 0.175, 0.170, 0.165, 0. };

  return new PhotonSpectrum(2.0,0.25,hamamatsu);
}

Pixel* SICBPhotonDetector::detect (Trajectory &photon) const
{
  // ajb 181001

  // simulate geometrical efficiency
  if ( RandFlat::shoot(0.,1.) > pixelActive_ ) {
    //    photon.absorbed();
    return NULL;
  };

  // intersect to plane
  int side = this->side( photon.position() );
  double dist;
  if ( ! photon.intersect(plane_[side],dist) ) {
    //    photon.absorbed();
    return NULL;
  };

  // Get hit point
  HepPoint3D pos = photon.position(dist);
  SICBPixel * pixel;

  // Check if Photon has switched sides
  int newside = this->side( pos );
  double newdist;
  if ( newside != side ) {
    if ( ! photon.intersect(plane_[newside],newdist) ) {
      //      photon.absorbed();
      return NULL;
    };
    HepPoint3D newpos = photon.position(newdist);
    //    SICBPixel * newpixel = newSICBPixel(newpos);
    pixel = newSICBPixel(newpos);
    PhotonDetector::updateTrajectory(photon, /*new*/pos);
    //    photon.observed(pos,newpixel);

  } else {

    pixel = newSICBPixel(pos);
    PhotonDetector::updateTrajectory(photon, /*new*/pos);
    //    photon.observed(pos,pixel);

  }; 
  return pixel;
}

Pixel * SICBPhotonDetector::newPixel (const HepPoint3D &position) const
{
  return newSICBPixel(position);
}

Pixel * SICBPhotonDetector::newPixel (const LocalPosition &local) const
{
#ifndef NDEBUG
  const RichXLocalPosition & localPosition =
    dynamic_cast<const RichXLocalPosition &> (local);
#else
  const RichXLocalPosition & localPosition =
    static_cast<const RichXLocalPosition &> (local);
#endif
  return newSICBPixel(localPosition);
}

SICBPixel * SICBPhotonDetector::newSICBPixel (const RichXLocalPosition &local) const
{
  assert( rich().id() == local.rich() );

  HepPoint3D global = localToGlobal_[local.side()] *
    HepPoint3D(local.x(),local.y(),0.);

  //    return new SICBPixel(local.rich(),global.x(),global.y(),global.z());
  return new SICBPixel(local.rich(),global);
}

SICBPixel * SICBPhotonDetector::newSICBPixel (const HepPoint3D &position) const
{
  RichXLocalPosition * local = newRichXLocalPosition(position);
  if ( local == NULL ) {
    return NULL;
  } else {
    SICBPixel * pixel = newSICBPixel(*local);
    delete local;
    return pixel;
  };
}

LocalPosition * SICBPhotonDetector::newLocalPosition (const Pixel &pixel) const
{
#ifndef NDEBUG
  const SICBPixel & sicbPixel = dynamic_cast<const SICBPixel &> (pixel);
#else
  const SICBPixel & sicbPixel = static_cast<const SICBPixel &> (pixel);
#endif
  return newRichXLocalPosition(sicbPixel);
}

LocalPosition * SICBPhotonDetector::newLocalPosition
(const HepPoint3D &position) const
{
  return newRichXLocalPosition(position);
}

RichXLocalPosition * SICBPhotonDetector::newRichXLocalPosition
(const SICBPixel &pixel) const
{
  assert( pixel.rich() == rich().id() );

  HepPoint3D global = pixel.position();
  //cout << "global " << global << endl;
  int side = this->side(global);
  //cout << "side " << side << endl;
  HepPoint3D proj = plane_[side].point(global);
  //cout << " Proj" << proj << endl;
  HepPoint3D local  = globalToLocal_[side] * global;
  //cout << "Local " << local << endl;

  //CRJ: skip assert !
  //assert(abs(local.z())<0.1);

  if ( verifyLocal(side,local.x(),local.y()) ) {
    //       cout << "returning position" << endl;
    return new RichXLocalPosition(pixel.rich(),side,local.x(),local.y());
  } else {
    //      cout << "Outside borders " << side << " : " << x
    //           << " : " << y << endl;
    cout << "returning null " << side << " " << pixel.position().x()
         << " " << pixel.position().y() << " " << pixel.rich() << endl;
    return NULL;
  };
}

RichXLocalPosition *
SICBPhotonDetector::newRichXLocalPosition (const HepPoint3D &position)
  const {

  int side = this->side(position);

  HepPoint3D local = globalToLocal_[side] * position;

  //assert( abs(local.z()) < 0.1 );

  double x = local.x();
  double y = local.y();

  // check if inside area
  if ( ! verifyLocal(side,x,y) ) return NULL;

  return new RichXLocalPosition(rich().id(),side,x,y);
}

HepPoint3D SICBPhotonDetector::globalPosition (const LocalPosition &local)
  const {
#ifndef NDEBUG
  const RichXLocalPosition & localPosition =
    dynamic_cast<const RichXLocalPosition &> (local);
#else
  const RichXLocalPosition & localPosition =
    static_cast<const RichXLocalPosition &> (local);
#endif
  return globalPosition(localPosition);
}

HepPoint3D SICBPhotonDetector::globalPosition (const Pixel &pixel) const
{
#ifndef NDEBUG
  const SICBPixel & simplePixel = dynamic_cast<const SICBPixel &> (pixel);
#else
  const SICBPixel & simplePixel = static_cast<const SICBPixel &> (pixel);
#endif
  return globalPosition(simplePixel);
}

HepPoint3D SICBPhotonDetector::globalPosition
(const RichXLocalPosition &local) const
{
  assert( local.rich() == rich().id() );

  int side = local.side();
  double x = local.x();
  double y = local.y();

  assert( verifyLocal(side,x,y) );

  return localToGlobal_[side] * HepPoint3D(x,y,0.);
}

HepPoint3D SICBPhotonDetector::globalPosition (const SICBPixel &pixel) const
{
  RichXLocalPosition * local = newRichXLocalPosition(pixel);
  assert( local != NULL );
  HepPoint3D global = globalPosition(*local);
  delete local;
  return global;
}

double SICBPhotonDetector::area (const Pixel &pixel) const {
  return pixelSize_ * pixelSize_ ;
}

int SICBPhotonDetector::nrNoisePixelToGenerate () const {

  double noisePixel = pixelNoise_ *
    ( 2 * nrXTube_ * nrYTube_ * nrXPixel_ * nrYPixel_ );
  return RandPoisson::shoot(noisePixel);
}

Pixel * SICBPhotonDetector::newNoisePixel () const {

  // the random generator does not reach the highest value ... to be on the safe side
  return NULL;

  //    int tube;
  //    do {
  //      tube = RandFlat::shootInt(nrXTube_*nrYTube_)+1;
  //    } while( tube < 1 || tube > nrXTube_*nrYTube_ );
  //
  //    if ( RandFlat::shoot(0.,1.) < 0.5 ) tube = -tube;
  //
  //    int pix;
  //    do {
  //      pix  = RandFlat::shootInt(nrXPixel_*nrYPixel_)+1;
  //    } while( pix < 1 || pix > nrXPixel_*nrYPixel_ );
  //
  //    SICBPixel * pixel = new RichXSimplePixel(rich().id(),tube,pix);
  //    RichXLocalPosition * position = newRichXLocalPosition(*pixel);
  //    if ( position != NULL ) {
  //      delete position;
  //      return pixel;
  //    } else {
  //      delete pixel;
  //      return NULL;
  //    };
}

double SICBPhotonDetector::expSignalBackground (const Pixel &pixel) const {
  return pixelNoise_;
}

void SICBPhotonDetector::update (const RichParameters &parameter) {

  switch ( this->rich().id() ) {
  case Rich::Rich1 :

    //zPos_        = parameter.getValue("Rich1.PDet.ZPos",     113.0  );
    //width_       = parameter.getValue("Rich1.PDet.Width",    100.0  );
    //length_      = parameter.getValue("Rich1.PDet.Length",    60.0  );
    //offsetx_      = parameter.getValue("Rich1.PDet.Offsetx",    40.0  );
    //m_Nsides      = parameter.getValue("Rich1.Nsides",       4 );

    // These parameters are needed by RichSim only.
    tubeSize_    = parameter.getValue("Rich1.PDet.TubeSize",  12.0  );
    pixelSize_   = parameter.getValue("Rich1.PDet.PixelSize",  0.2  );
    pixelActive_ = parameter.getValue("Rich1.PDet.PixelActive",0.73 );
    pixelNoise_  = parameter.getValue("Rich1.PDet.PixelNoise", 0.01 );

    zPos_        = parameter.getValue("CDF.RIC1.ZPHPD1",     110.4  );
    // Note width and length are extended articifically by 25% to fix 
    // local coordinate verification problem
    width_       = 1.25 * 2.0 * parameter.getValue("CDF.RIC1.YHPD1", 29.0 );
    length_      = 1.25 * 2.0 * parameter.getValue("CDF.RIC1.XHPD1", 32.5 );
    thickness_   = parameter.getValue("CDF.RIC1.THPD1",     10.0 );
    offsetx_     = parameter.getValue("CDF.RIC1.XPHPD1",      40.0  ) ;
    offsety_     = parameter.getValue("CDF.RIC1.YPHPD1",    40.0  ) ;
    tiltx_       = parameter.getValue("CDF.RIC1.ABHPD1",       0.0  );
    tilty_       = parameter.getValue("CDF.RIC1.AAHPD1",       0.0  );
    tiltz_       = parameter.getValue("CDF.RIC1.ACHPD1",       0.0  );
    m_Nsides     = 4;

    break;
  case Rich::Rich2 :

    // zPos_        = parameter.getValue("Rich2.PDet.ZPos",    1078.0  );
    // width_       = parameter.getValue("Rich2.PDet.Width",    120.0  );
    //length_       = parameter.getValue("Rich2.PDet.Length",    72.0  );
    // tilty_       = parameter.getValue("Rich2.PDet.Tilty",      .00 );
    // tiltx_       = parameter.getValue("Rich2.PDet.Tiltx",      .00 );
    // offsetx_     = parameter.getValue("Rich2.PDet.Offsetx",   340.0  );
    // offsety_     = parameter.getValue("Rich2.PDet.Offsety",    0.0  );

    // These parameters are needed by RichSim only.
    tubeSize_    = parameter.getValue("Rich2.PDet.TubeSize",  12.0  );
    pixelSize_   = parameter.getValue("Rich2.PDet.PixelSize",  0.2  );
    pixelActive_ = parameter.getValue("Rich2.PDet.PixelActive",0.73 );
    pixelNoise_  = parameter.getValue("Rich2.PDet.PixelNoise", 0.01 );

    zPos_        = parameter.getValue("CDF.RIC2.ZPHPD2",    1068.71  );
    // Note width and length are extended articifically by 25% to fix 
    // local coordinate verification problem
    width_       = 1.25 * 2.0 * parameter.getValue("CDF.RIC2.YHPD2", 120.0  );
    length_      = 1.25 * 2.0 * parameter.getValue("CDF.RIC2.ZHPD2", 72.0  );
    thickness_   = parameter.getValue("CDF.RIC2.THPD2",     10.0 );
    offsetx_     = parameter.getValue("CDF.RIC2.XPHPD2",   340.0  ) ;
    offsety_     = 0.0;
    tiltx_       = 0.0;
    // Tilt angle = pi/2 - cdf_angle
    tilty_       = 1.570796327 - 
                   parameter.getValue("CDF.RIC2.APHPD2",      .711 );
    tiltz_       = 0.0;
    m_Nsides     = 2;

    break;
  default:
    assert(false);
  };

  // Define detector planes and coordinate transforms.
  plane_[0] = setPlane( tiltx_, tilty_,-tiltz_, offsetx_, offsety_, length_, 
                       width_, thickness_, zPos_);
  plane_[1] = setPlane( tiltx_,-tilty_, tiltz_,-offsetx_, offsety_,-length_, 
                       width_, thickness_, zPos_);
  globalToLocal_[0] = defineGlobalToLocal( tiltx_, tilty_,-tiltz_, offsetx_,offsety_, 
                                          length_, width_, thickness_, zPos_);
  globalToLocal_[1] = defineGlobalToLocal( tiltx_,-tilty_, tiltz_,-offsetx_, offsety_,
                                          -length_, width_, thickness_, zPos_);
  localToGlobal_[0] = defineLocalToGlobal( tiltx_, tilty_,-tiltz_,offsetx_, offsety_, 
                                          length_, width_, thickness_, zPos_);
  localToGlobal_[1] = defineLocalToGlobal( tiltx_,-tilty_, tiltz_,-offsetx_, offsety_,
                                          -length_, width_, thickness_, zPos_);
  if ( m_Nsides == 4 ) {
    plane_[2] = setPlane(-tiltx_, tilty_, tiltz_,offsetx_,-offsety_, length_,
                          -width_, thickness_, zPos_);
    plane_[3] = setPlane(-tiltx_,-tilty_,-tiltz_,-offsetx_,-offsety_,-length_,
                          -width_, thickness_, zPos_);
    globalToLocal_[2] = defineGlobalToLocal(-tiltx_, tilty_, tiltz_,offsetx_,-offsety_, 
                                            length_,-width_, thickness_, zPos_);
    globalToLocal_[3] = defineGlobalToLocal(-tiltx_,-tilty_,-tiltz_,-offsetx_,-offsety_,
                                            -length_,-width_, thickness_, zPos_);
    localToGlobal_[2] = defineLocalToGlobal(-tiltx_, tilty_, tiltz_,offsetx_,-offsety_, 
                                             length_,-width_, thickness_, zPos_);
    localToGlobal_[3] = defineLocalToGlobal(-tiltx_,-tilty_,-tiltz_,-offsetx_,-offsety_,
                                             -length_,-width_, thickness_, zPos_);
  }

  // tests on transformations

  HepDouble a = 20.0;
  HepDouble b = -20.0;
  HepDouble c = 110.0;

  HepPoint3D VectA(a, b, c);
  HepPoint3D VectB, VectC;

  for (int i = 0; i<m_Nsides; i++) {
    VectB = globalToLocal_[i] * VectA;
    VectC = localToGlobal_[i] * VectB;
  }

  nrXTube_  = int( ceil( length_ / tubeSize_ ) );
  nrYTube_  = int( ceil( width_ / tubeSize_ ) );
  nrXPixel_ = int( ceil( tubeSize_ / pixelSize_ ) );
  nrYPixel_ = int( ceil( tubeSize_ / pixelSize_ ) );

}

bool SICBPhotonDetector::verifyLocal (const int side,
                                      const double x,
                                      const double y) const
{
  bool inLocal = true;
  switch ( this->rich().id() ) {
  case Rich::Rich1 :
    if ( 0 == side ) {
      if ( x < 0 || x > length_ || y < 0 || y > width_ ) inLocal = false;
    } else if ( 1 == side ) {
      if ( x > 0 || x < -length_ || y < 0 || y > width_ ) inLocal = false;
    } else if ( 2 == side ) {
      if ( x < 0 || x > length_ || y > 0 || y < -width_ ) inLocal = false;
    } else if ( 3 == side ) {
      if ( x > 0 || x < -length_ || y > 0 || y < -width_ ) inLocal = false;
    }
    break;
  case Rich::Rich2 :
    if ( side == 0 ) {
      if ( x < 0 || x > length_ ) inLocal = false;
    } else {
      if ( x > 0 || x < -length_ ) inLocal = false;
    }
    if ( abs(y) > 0.5 * width_ ) inLocal = false;
    break;
  };

  return inLocal;
}
