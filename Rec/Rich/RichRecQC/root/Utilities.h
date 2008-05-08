
#ifndef MACROS_UTILITIES_H
#define MACROS_UTILITIES_H 1

#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"


namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Utils
   *
   *  General namespace for RICH ROOT script Utils
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Utils
  {
    void systemSaveImage ( TCanvas * canvas,
                           const std::string & name );
  }
}

#endif
