// $Id: PhotonDetector.cpp,v 1.3 2001-10-31 16:50:12 rihill Exp $

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichDet/LocalPosition.h"
#include "RichDet/Pixel.h"
#include "RichDet/Trajectory.h"


PhotonDetector::PhotonDetector (const Rich &rich)
  : m_rich(&rich)
{
}

PhotonDetector::~PhotonDetector() {}


void PhotonDetector::updateTrajectory(Trajectory& trajectory, 
                                      const HepPoint3D& pos) const
{
  trajectory.m_position = pos;
}

