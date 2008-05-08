// $Id: RichCKResolution.h,v 1.2 2008-05-08 13:26:40 jonrob Exp $
#ifndef MACROS_RICHCKRESOLUTION_H
#define MACROS_RICHCKRESOLUTION_H 1

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include "Utilities.h"

namespace Rich
{

 //-----------------------------------------------------------------------------
  /** @namespace PIDPerformance
   *
   *  General namespace for RICH ROOT utilities for Cherenkov angle resolution 
   *  studies
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace CKResolution
  {

    void ckResVtheta( TFile * file,
                      const std::string & prodTag,
                      const std::string & radiator );
    void ckRes( TFile * file,
                const std::string & prodTag,
                const std::string & nameTag,
                const std::string & imageType,
                const std::string & radiator );
    
  }
}

#endif // MACROS_RICHCKRESOLUTION_H
