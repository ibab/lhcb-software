// $Id: RichPIDPerformance.h,v 1.1 2007-07-25 14:44:22 jonrob Exp $
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
                      const std::string & allfile );
    void allPlots( TFile * pFile,
                   const std::string & filetag,
                   const std::string & titletag );

  }
}


#endif // MACROS_RICHPIDPERFORMANCE_H
