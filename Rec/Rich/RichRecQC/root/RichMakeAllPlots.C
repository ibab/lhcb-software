
#include "RichPIDPerformance.h"
#include "RichCKResolution.h"
#include "RichTrackResolutions.h"
#include "RichNCKPhotons.h"

namespace RICH
{

  // Creates images files for Aerogel CK resolution, for given root file
  void aerogel( TFile * file,
                const std::string & prodTag,
                const std::string & nameTag,
                const std::string & imageType )
  {
    RICH::CKResolution::ckRes(file,prodTag,nameTag,imageType,"Aerogel");
    //RICH::TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Aerogel");
  }

  // Creates images files for Aerogel CK resolution, for given root file
  void c4f10( TFile * file,
              const std::string & prodTag,
              const std::string & nameTag,
              const std::string & imageType )
  {
    RICH::CKResolution::ckRes(file,prodTag,nameTag,imageType,"Rich1Gas");
    //RICH::TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Rich1Gas");
  }

  // Creates images files for Aerogel CK resolution, for given root file
  void cf4( TFile * file,
            const std::string & prodTag,
            const std::string & nameTag,
            const std::string & imageType )
  {
    RICH::CKResolution::ckRes(file,prodTag,nameTag,imageType,"Rich2Gas");
    //RICH::TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Rich2Gas");
  }

  // shortcut to producing all resolution figures
  void createAllFigures( TFile * file,
                         const std::string & prodTag,
                         const std::string & nameTag )
  {
    if (!file) return;
    gSystem->Exec( ("mkdir -p "+prodTag).c_str() );

    // Create EPS files
    aerogel(file,prodTag,nameTag,"eps");
    c4f10(file,prodTag,nameTag,"eps");
    cf4(file,prodTag,nameTag,"eps");

    // create PNG files
    //aerogel(file,prodTag,nameTag,"png");
    //c4f10(file,prodTag,nameTag,"png");
    //cf4(file,prodTag,nameTag,"png");

    // CK res V theta from ntuple
    //RICH::CKResolution::ckResVtheta(file,prodTag,"Aerogel");
    //RICH::CKResolution::ckResVtheta(file,prodTag,"Rich1Gas");
    //RICH::CKResolution::ckResVtheta(file,prodTag,"Rich2Gas");

  }

  void performance( const std::string hfile,
                    const std::string tag,
                    const std::string title )
  {
    // load data file
    TFile * file = TFile::Open(hfile.c_str());
    if ( !file ) return;

    // create figures from resolution plots
    //createAllFigures( file, tag, title );

    // PID curves
    RICH::PIDPerformance::allPlots( file, tag, title );

    // Number photons
    //RICH::NCKPhotons::createAllFigures( file, tag, title );

    pFile->Close();
  }

}
