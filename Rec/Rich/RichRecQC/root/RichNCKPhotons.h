// $Id: RichNCKPhotons.h,v 1.3 2008-05-08 13:26:40 jonrob Exp $
#ifndef MACROS_RICHNCKPHOTONS_H
#define MACROS_RICHNCKPHOTONS_H 1

#include <iostream>
#include <map>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"

#include "Utilities.h"

namespace Rich
{
  
 //-----------------------------------------------------------------------------
  /** @namespace PIDPerformance
   *
   *  General namespace for RICH ROOT utilities for photon yield studies
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace NCKPhotons
  {

    void createAllFigures( TFile * file,
                           const std::string prodTag,
                           const std::string nameTag );


  }
}
#endif // MACROS_RICHNCKPHOTONS_H
