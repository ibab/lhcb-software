// $Id: RichPIDPerformance.h,v 1.2 2007-08-09 16:20:32 jonrob Exp $
#ifndef MACROS_RICHPIDPERFORMANCE_H
#define MACROS_RICHPIDPERFORMANCE_H 1

#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TF1.h"

#include "Utilities.h"

namespace RICH
{
  namespace PIDPerformance
  {

    void compareRads( const std::string & tag,
                      const std::string & filetag,
                      const std::string & aerofile,
                      const std::string & c4f10file,
                      const std::string & cf4file,
                      const std::string & allfile,
                      const std::vector<std::string> & imageType );
    
    void allPlots( TFile * pFile,
                   const std::string & filetag,
                   const std::string & titletag,
                   const std::vector<std::string> & imageType );

  }
}


#endif // MACROS_RICHPIDPERFORMANCE_H
