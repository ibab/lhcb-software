// $Id: RichG4MirrorReflPointTag.h,v 1.1 2004-06-03 12:42:34 seaso Exp $
#ifndef MISC_RICHG4MIRRORREFLPOINTTAG_H 
#define MISC_RICHG4MIRRORREFLPOINTTAG_H 1

// Include files
#include "G4Track.hh"
#include "G4ThreeVector.hh"
extern void RichG4MirrorReflPointTag(const G4Track* aPhotonTk,
                                     const G4ThreeVector & aReflPoint,
                                     int aRichDetNum, int aMirrorTypeNum,
                                     int aMirrorCopyNum);

  // Here aRichDetNum is 0 for rich1 and 1 for rich2.
  //      aMirrorTypeNum is 0 for Spherical mirror where the first
  //                                  reflection occurs.
  //                     is 1 for Flat mirror where the second 
  //                                   reflection occurs.
  //     aMirrorCopyNum is the copy number of the mirror.

#endif // MISC_RICHG4MIRRORREFLPOINTTAG_H
