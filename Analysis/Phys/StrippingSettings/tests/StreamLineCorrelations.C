void StreamLineCorrelations(TString filename = "bmuon") {
  gROOT->Reset();
  
  int ind=0;
  
  const int MAX_STREAMS = 10;
  
  TObjArray* list[MAX_STREAMS];
  TTree* tree[MAX_STREAMS];
  
  TString strname[MAX_STREAMS];
  
  int n=0;
  do { 
    int ind2 = filename.Index(TString(" "),ind);
    if (ind2 == kNPOS) ind2 = filename.Length();
    printf("Adding %s\n", TString(filename(ind, ind2-ind)).Data());

    TString fn("etc_" + filename(ind, ind2-ind) + ".root");
    
    strname[n] = filename(ind, ind2-ind);

    TFile* f = new TFile(fn);
    f->cd();
    tree[n] = (TTree*)gROOT->FindObject("<local>_TagCreator_EventTuple");
    list[n] = tree[n]->GetListOfBranches();
    printf("Branches: %d\n", list[n]->GetEntries());
    n++;
    
    if (n==MAX_STREAMS) break;

    if (ind2 == filename.Length()) break;
    ind = ind2+1;
  } while(1);
  
  TH1F h("h"," ",2,-0.5,1.5);
  
  int streams = 0;
  int lines = 0;
  
  int ln = 1;
  int sn = 1;
  
  FILE* of = fopen("corr.txt","w");

  printf("\n\n==================== STREAM-STREAM CORRELATIONS ====================\n");
  fprintf(of,"\n\n==================== STREAM-STREAM CORRELATIONS ====================\n");

  printf("                                 ");
  fprintf(of,"                                 ");
  for (int k=0; k<n; k++) {
    printf("%6s ", strname[k].Data());
    fprintf(of,"%6s ", strname[k].Data());
  }
  printf("\n");
  printf("--------------------------------------------------------------------\n");
  fprintf(of,"\n");
  fprintf(of,"--------------------------------------------------------------------\n");

  for (int i=0; i<list[0]->GetEntries(); i++) {
    TBranch* br = (TBranch*)(*list[0])[i];
    TString sel = br->GetName();
    if (sel.Index(TString("StrippingStream")) == 0) {
      printf("  %2d: %25s: ", sn, sel.Data() + 9);
      fprintf(of,"  %2d: %25s: ", sn, sel.Data() + 9);
      ln = 1;
      sn++;
      streams++;
      for (int k=0; k<n; k++) {
        h.Reset();
        tree[k]->Project("h", sel, sel+"==1");
        double num = h.GetEntries();
        printf("%6.1d ", num);
        fprintf(of,"%6.1d ", num);
      }
      printf("\n");  
      fprintf(of,"\n");  
    }
  }

  ln = 1; 
  sn = 1;

  printf("\n\n==================== STREAM-LINE CORRELATIONS ======================\n");
  fprintf(of,"\n\n==================== STREAM-LINE CORRELATIONS ======================\n");

  printf("                                 ");
  fprintf(of,"                                 ");
  for (int k=0; k<n; k++) {
    printf("%6s ", strname[k].Data());
    fprintf(of,"%6s ", strname[k].Data());
  }
  printf("\n");
  printf("--------------------------------------------------------------------\n");
  fprintf(of,"\n");
  fprintf(of,"--------------------------------------------------------------------\n");

  for (int i=0; i<list[0]->GetEntries(); i++) {
    TBranch* br = (TBranch*)(*list[0])[i];
    TString sel = br->GetName();
    if (sel.Index(TString("StrippingGlobal")) == 0) {
      printf("    %27s: ", sel.Data() + 9);
      fprintf(of,"    %27s: ", sel.Data() + 9);
      for (int k=0; k<n; k++) {
        h.Reset();
        tree[k]->Project("h", sel, sel+"==1");
        double num = h.GetEntries();
        printf("%6.1d ", num);
        fprintf(of,"%6.1d ", num);
      }
      printf("\n");
      fprintf(of,"\n");
    }
    else if (sel.Index(TString("StrippingStream")) == 0) {
      printf("--------------------------------------------------------------------\n");
      printf("  %2d: %25s: ", sn, sel.Data() + 9);
      fprintf(of,"--------------------------------------------------------------------\n");
      fprintf(of,"  %2d: %25s: ", sn, sel.Data() + 9);
      ln = 1;
      sn++;
      streams++;
      for (int k=0; k<n; k++) {
        h.Reset();
        tree[k]->Project("h", sel, sel+"==1");
        double num = h.GetEntries();
        printf("%6.1d ", num);
        fprintf(of,"%6.1d ", num);
      }
      printf("\n");  
      fprintf(of,"\n");  
    }
    else if (sel.Index(TString("Stripping")) == 0) {
      printf("    %2d: %23s: ", ln, sel.Data() + 9);
      fprintf(of,"    %2d: %23s: ", ln, sel.Data() + 9);
      ln++;
      lines++;
      for (int k=0; k<n; k++) {
        h.Reset();
        tree[k]->Project("h", sel, sel+"==1");
        double num = h.GetEntries();
        printf("%6.1d ", num);
        fprintf(of,"%6.1d ", num);
      }
      printf("\n");  
      fprintf(of,"\n");  
    }
  }
  
  fclose(of);
}
