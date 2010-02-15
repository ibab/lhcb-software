void TestStripping(TString filename = "etc.root") {
  gROOT->Reset();

  TFile* f = new TFile(filename);
  TFile* f2 = new TFile("hist.root");

  f->cd();

  TTree *tree = gROOT->FindObject("<local>_TagCreator_EventTuple");
  
  TObjArray* list = tree->GetListOfBranches();
  
  printf("Branches: %d\n", list->GetEntries());

  TH1F h("h"," ",2,-0.5,1.5);
  
  int n=0;
  
  int streams = 0;
  int lines = 0;

  for (int i=0; i<list->GetEntries(); i++) {
    TBranch* br = (TBranch*)(*list)[i];
    h.Reset();
    TString sel = br->GetName();
    if (sel.Index(TString("StrippingGlobal")) == 0) {
      n++;
      tree->Project("h", sel, sel+"==1");
      double num = h.GetEntries();
      printf("%4d: Global decision %38s: %4.1d\n", n, sel.Data(), num);
      printf("-------------------------------------------------------------------------\n");  
    }
    else if (sel.Index(TString("StrippingStream")) == 0) {
      n++;
      streams++;
      tree->Project("h", sel, sel+"==1");
      double num = h.GetEntries();
      printf("-------------------------------------------------------------------------\n");  
      printf("  %4d: Stream %45s: %4.1d\n", n, sel.Data(), num);
    }
    else if (sel.Index(TString("Stripping")) == 0) {
      n++;
      lines++;
      tree->Project("h", sel, sel+"==1");
      double num = h.GetEntries();
  
      f2->cd(sel);
      TH1F* th = (TH1F*)gROOT->FindObject(sel+" walltime");
      double wt = 0;
      if (th) wt = th->GetMean();
      f->cd();
      
      printf("    %4d: Line %45s: %4.1d %6.3f\n", n, sel.Data(), num, wt);
    }
  }

  printf("============================= SUMMARY ===================================\n");

  for (int i=0; i<list->GetEntries(); i++) {
    TBranch* br = (TBranch*)(*list)[i];
    h.Reset();
    TString sel = br->GetName();
    if (sel.Index(TString("StrippingGlobal")) == 0) {
      n++;
      tree->Project("h", sel, sel+"==1");
      double num = h.GetEntries();
      printf("   Global decision:   %38s: %4.1d\n", sel.Data(), num);
    }
  }
  
  for (int i=0; i<list->GetEntries(); i++) {
    TBranch* br = (TBranch*)(*list)[i];
    h.Reset();
    TString sel = br->GetName();
    if (sel.Index(TString("StrippingStream")) == 0) {
      n++;
      tree->Project("h", sel, sel+"==1");
      double num = h.GetEntries();
      printf("   Stream      %45s: %4.1d\n", sel.Data(), num);
    }
  }

  printf("-------------------------------------------------------------------------\n");  
  
  printf("%d streams, %d lines\n", streams, lines);

  printf("-------------------------------------------------------------------------\n");  
}
