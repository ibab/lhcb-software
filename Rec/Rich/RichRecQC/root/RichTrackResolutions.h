// $Id: RichTrackResolutions.h,v 1.1 2007-07-25 14:44:22 jonrob Exp $
#ifndef MACROS_RICHTRACKRESOLUTIONS_H
#define MACROS_RICHTRACKRESOLUTIONS_H 1

#include <iostream>
#include <map>
#include <string>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"

#include "Utilities.h"

namespace RICH
{

  // RICH CK resolution functions
  namespace TrackResolution
  {
    void trackRes( TFile * file,
                   const std::string & prodTag,
                   const std::string & nameTag,
                   const std::string & imageType,
                   const std::string & radiator );
  }
}

#endif // MACROS_RICHTRACKRESOLUTIONS_H
