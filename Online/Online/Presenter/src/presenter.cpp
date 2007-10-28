#include <iostream>
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TH1.h>

#include "presenter.h"
#include "PresenterMainFrame.h"

//TODO: ACLiC
#define STANDALONE

void presenter()
{
  new PresenterMainFrame("LHCb Online Presenter", 10, 10, 800, 600, Silent);
}
#ifdef STANDALONE
int main(int argc, char** argv)
{
  //TODO: Put cli param parser here: w/h/verbosity

  TApplication* presenterApp = new TApplication("Presenter", &argc, argv);
  gROOT->Reset("a");
  TH1::AddDirectory(kFALSE);
  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  presenter();
  presenterApp->Run();
  return 0;
}
#endif
