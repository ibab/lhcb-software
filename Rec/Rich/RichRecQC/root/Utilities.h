
#ifndef MACROS_UTILITIES_H 
#define MACROS_UTILITIES_H 1

#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"


namespace RichUtils
{
  void systemSaveImage ( TCanvas * canvas,
                         const std::string name );
}

#endif 
