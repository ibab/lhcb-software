// $Id: RichNCKPhotons.h,v 1.4 2008-06-04 07:08:35 cattanem Exp $
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
  /** @namespace NCKPhotons
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
