
#include <cassert>

// Rich
#include "RichDet/Rich.h"
// PhotonSpectrum
#include "RichDet/PhotonSpectrum.h"
// PhotonDetector
#include "RichDet/PhotonDetector.h"
// RichParameters
#include "RichDet/RichParameters.h"
// Photon
//#include "RichDet/Photon.h"
// Rich1PhotonReflector
#include "RichDet/Rich1PhotonReflector.h"


Rich1PhotonReflector::Rich1PhotonReflector (const Rich &rich)
  : PhotonReflector(rich)
{
}

Rich1PhotonReflector::~Rich1PhotonReflector(){}

void Rich1PhotonReflector::update (const RichParameters &parameter)
{
  // m_Nsides       = parameter.getValue("Rich1.Nsides",   4 );
  // m_radius       = parameter.getValue("Rich1.Refl.Radius", 190.   );
  // m_reflectivity = parameter.getValue("Rich1.Refl.Reflectivity", 0.95 );

  m_reflectivity = parameter.getValue("CDF.RIC1.REFMIRSPH1", 0.90 );
  m_Nsides       = 4;

  // spherical Mirror
  m_radius       = parameter.getValue("CDF.RIC1.RCMIR1",   170. );

  //m_zPos         = parameter.getValue("Rich1.Refl.ZPos",   198. );
  //m_tiltx        = parameter.getValue("Rich1.Refl.Tiltx",    0. );
  //m_tilty        = parameter.getValue("Rich1.Refl.Tilty",    0.25 );
  //m_center[0] = setCenter(m_radius, m_tiltx,-m_tilty, m_zPos);
  //m_center[1] = setCenter(m_radius, m_tiltx, m_tilty, m_zPos);
  //m_center[2] = setCenter(m_radius,-m_tiltx,-m_tilty, m_zPos);
  //m_center[3] = setCenter(m_radius,-m_tiltx, m_tilty, m_zPos);

  m_xCen         = parameter.getValue("CDF.RIC1.XMIR1",   50.0 );
  m_yCen         = parameter.getValue("CDF.RIC1.YMIR1",   11.0 );
  m_zCen         = parameter.getValue("CDF.RIC1.ZMIR1",   27.5 );
  m_center[0] = HepPoint3D( m_xCen, m_yCen, m_zCen);
  m_center[1] = HepPoint3D(-m_xCen, m_yCen, m_zCen);
  m_center[2] = HepPoint3D( m_xCen,-m_yCen, m_zCen);
  m_center[3] = HepPoint3D(-m_xCen,-m_yCen, m_zCen);

}

PhotonSpectrum * Rich1PhotonReflector::newPhotonEfficiency () const
{

  PhotonSpectrum * spectrum = rich().detector().photonEfficiency();
  assert(spectrum != NULL);
  (*spectrum) *= m_reflectivity;
  return spectrum;

}
