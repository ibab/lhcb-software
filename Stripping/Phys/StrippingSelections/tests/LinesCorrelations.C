void LinesCorrelations(TString filename = "etc_bmuon", TString stream = "") {
  gROOT->Reset();

  TFile* f = new TFile(filename+".root");
  TTree *tree = gROOT->FindObject("<local>_TagCreator_EventTuple");
  TObjArray* list = tree->GetListOfBranches();
  
  printf("Branches: %d\n", list->GetEntries());

  TH1F h("h"," ",2,-0.5,1.5);
  
  int n=0;
  
  int streams = 0;
  int lines = 0;
  
  int start = -1;
  int stop = -1;

  if (stream == TString("")) {
    start = 0;
    stop = list->GetEntries();
  } else {
  
    for (int i=0; i<list->GetEntries(); i++) {
      TBranch* br1 = (TBranch*)(*list)[i];
      TString sel1 = br1->GetName();
      if (sel1 == "StrippingStream"+stream)) {
        start = i;
        printf("Start index %d\n", start);
      }
      else if (sel1.Index(TString("StrippingStream")) == 0 && start != -1) {
        stop = i;
        printf("Stop index %d\n", stop);
        break;
      }
    }  
  
  }
  
  if (stop == -1) stop = list->GetEntries();
  int n=1; 
  
  FILE* of = fopen(TString(filename+".txt").Data(),"w");

  printf("                             ");
  fprintf(of,"                             ");
  for (int i=start; i<stop; i++) {
    TBranch* br1 = (TBranch*)(*list)[i];
    TString sel1 = br1->GetName();
    if (sel1.Index(TString("Stripping")) == 0 && 
        sel1.Index(TString("StrippingStream")) != 0 && 
        sel1.Index(TString("StrippingGlobal")) != 0) {
      printf("%5.1d ", n);
      fprintf(of,"%5.1d ", n);
      n++;
    }
  }

  TH2F* ch = new TH2F("ch",filename, n, 0.5, n+0.5, n, 0.5, n+0.5);

  printf("\n");
  printf("-----------------------------");
  for (int i=1; i<n; i++) printf("------");
  printf("\n");

  fprintf(of,"\n");
  fprintf(of,"-----------------------------");
  for (int i=1; i<n; i++) fprintf(of,"------");
  fprintf(of,"\n");

  int n=1; 

  for (int i=start; i<stop; i++) {
    TBranch* br1 = (TBranch*)(*list)[i];
    TString sel1 = br1->GetName();
    if (sel1.Index(TString("Stripping")) == 0 && 
        sel1.Index(TString("StrippingStream")) != 0 && 
        sel1.Index(TString("StrippingGlobal")) != 0) {
      printf("%2.1d. %23s: ", n, sel1.Data() + 9);
      fprintf(of,"%2.1d. %23s: ", n, sel1.Data() + 9);
      int m=1;
      for (int j=start; j<stop; j++) {
        TBranch* br2 = (TBranch*)(*list)[j];
        h.Reset();
        TString sel2 = br2->GetName();
        if (sel2.Index(TString("Stripping")) == 0 && 
            sel2.Index(TString("StrippingStream")) != 0 && 
            sel2.Index(TString("StrippingGlobal")) != 0) {
          tree->Project("h", sel1, sel1+"==1 && "+sel2+"==1");
          double num = h.GetEntries();
          printf("%5.1d ", num);
          fprintf(of,"%5.1d ", num);
          ch->SetBinContent(n,m,num);
          m++;
        }
      }
      n++;
      printf("\n");
      fprintf(of,"\n");
    }
  }
  
  fclose(of);
  
  ch->Draw();
}
