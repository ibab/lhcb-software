#ifndef ALIGNMENTMONITORINGHISTHELPER_H
#define ALIGNMENTMONITORINGHISTHELPER_H
// ROOT
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TString.h"
// STL
#include <string>
#include <vector>

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class AlignmentMonitoringHistHelper
    {
    public:
      // Constructors
      AlignmentMonitoringHistHelper() {}
      // Destructor
      ~AlignmentMonitoringHistHelper() {}
      
      // Methods
      TCanvas* getCanvas(const TString& name, const TString& title, int w=700, int h=500);
      void createPage(TCanvas* canvas, const char* title, int nx, int ny);
      TCanvas* createPage(const char* name, const char* title, int nx, int ny, int w=700, int h=500);
      TFile* getFile(const TString& filename);
      TObject * getObject(const TString& filename, const TString& directory, const TString& hisname);
      TObject * getObject(const TString& filename, const TString& name);
      TH1* getH1(const TString& filename, const TString& hisname);
      TH2* getH2(const TString& filename, const TString& hisname);
      bool is1D( TH1* h1 );
      bool isProfile1D( TH1* h1 );
      double max1D( const TH1& h1 );
      double min1D( const TH1& h1 );
      TH1* drawH1(TString filename, TString hisname, const char* drawopt="", int color=1, bool normalize = false);
      TH1* drawH2(TString filename, TString hisname, const char* drawopt="", int color=1);
      int colorFromIndex(int index);
      TH1* drawH1(const std::vector<TString>& files, const TString& hisname, TString drawopt = "", bool normalize = false);
      TH1* drawH2(std::vector<TString> files, TString hisname, TString drawopt);
    private:
    };
    
  }// namespace AlignmentMonitoring
}// namespace Alignment

#endif
