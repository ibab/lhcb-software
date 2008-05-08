// $Id: RichTrackResolutions.h,v 1.2 2008-05-08 13:26:40 jonrob Exp $
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

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace TrackResolution
   *
   *  General namespace for RICH ROOT utilities for Track resolution studies
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
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
