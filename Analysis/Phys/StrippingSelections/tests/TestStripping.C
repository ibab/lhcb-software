void TestStripping(TString filename = "etc.root") {
  gROOT->Reset();

  TFile* f = new TFile(filename);

  TTree *tree = gROOT->FindObject("<local>_TagCreator_EventTuple");
  
  TObjArray* list = tree->GetListOfBranches();
  
  printf("Branches: %d\n", list->GetEntries());

  TH1F h("h"," ",2,-0.5,1.5);
  
  for (int i=0; i<list->GetEntries(); i++) {
    TBranch* br1 = (TBranch*)(*list)[i];
    h.Reset();
    TString cond1 = br1->GetName();
    tree->Project("h", br1->GetName(), cond1+"==1");
    double num = h.GetEntries();
    printf("%2d: %32s %f\n", i, br1->GetName(), num);
  }
}
