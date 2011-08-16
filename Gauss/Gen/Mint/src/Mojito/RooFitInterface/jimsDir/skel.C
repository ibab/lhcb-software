// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Mon Apr 16 06:50:44 2007 by ROOT version5.12/00e)
//   from TTree results/Pull studies results
//   found on file: cartesian_1001/KpipiResults.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("cartesian_1001/KpipiResults.root");
   if (!f) {
      f = new TFile("cartesian_1001/KpipiResults.root");
   }
   TTree *results = (TTree*)gDirectory->Get("results");

//Declaration of leaves types
   Float_t         toyresult_xp;
   Float_t         toyresult_exp;
   Float_t         toyresult_yp;
   Float_t         toyresult_eyp;
   Float_t         toyresult_rhop;
   Float_t         toyresult_xm;
   Float_t         toyresult_exm;
   Float_t         toyresult_ym;
   Float_t         toyresult_eym;
   Float_t         toyresult_rhom;

   // Set branch addresses.
   results->SetBranchAddress("toyresult",&toyresult_xp);
   results->SetBranchAddress("toyresult",&toyresult_exp);
   results->SetBranchAddress("toyresult",&toyresult_yp);
   results->SetBranchAddress("toyresult",&toyresult_eyp);
   results->SetBranchAddress("toyresult",&toyresult_rhop);
   results->SetBranchAddress("toyresult",&toyresult_xm);
   results->SetBranchAddress("toyresult",&toyresult_exm);
   results->SetBranchAddress("toyresult",&toyresult_ym);
   results->SetBranchAddress("toyresult",&toyresult_eym);
   results->SetBranchAddress("toyresult",&toyresult_rhom);

//     This is the loop skeleton
//       To read only selected branches, Insert statements like:
// results->SetBranchStatus("*",0);  // disable all branches
// TTreePlayer->SetBranchStatus("branchname",1);  // activate branchname

   Long64_t nentries = results->GetEntries();

   Long64_t nbytes = 0;
//   for (Long64_t i=0; i<nentries;i++) {
//      nbytes += results->GetEntry(i);
//   }
}
