// $Id: PhotonDetector.cpp,v 1.1.1.1 2001-10-17 11:42:03 jonrob Exp $

// Rich
#include "RichDet/Rich.h"
// PhotonSpectrum
#include "RichDet/PhotonSpectrum.h"
// PhotonDetector
#include "RichDet/PhotonDetector.h"
// RichParameters
#include "RichDet/RichParameters.h"
// LocalPosition
#include "RichDet/LocalPosition.h"
// Pixel
#include "RichDet/Pixel.h"
// Photon
#include "RichRec/Photon.h"


PhotonDetector::PhotonDetector (const Rich &rich)
  : m_rich(&rich)
{
}

PhotonDetector::~PhotonDetector() {}

