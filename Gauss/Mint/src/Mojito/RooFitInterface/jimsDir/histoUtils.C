// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT

TH2* histoDiff(TH2* h1, TH2* h2){
  TH2* diff = (TH2*) h1->Clone("diff");
  int nx1 = h1->GetNbinsX();
  int nx2 = h2->GetNbinsX();
  if(nx2 != nx1) cout << "differnent bin numbers!" << endl;
  int nx = (nx1 <= nx2 ? nx1 : nx2);
  int ny1 = h1->GetNbinsX();
  int ny2 = h2->GetNbinsX();
  if(ny2 != ny1) cout << "differnent bin numbers!" << endl;
  int ny = (ny1 <= ny2 ? ny1 : ny2);

  for(int i=1; i<= nx; i++){
    for(int j=1; j<=ny; j++){
      double n1 = h1->GetBinContent(i,j);
      double n2 = h2->GetBinContent(i,j);
      double d12 = n1 - n2;
      double err1 = h1->GetBinError(i,j);
      if(err1 <=0 && n1 > 0) err1 = sqrt(n1);
      double err2 = h2->GetBinError(i,j);
      if(err2 <=0 && n2 > 0) err2 = sqrt(n2);
      double err12 = sqrt(err1*err1 + err2*err2);

      diff->SetBinContent(i, j, d12);
      diff->SetBinError(i,j, err12);
    }
  }
  
  return diff;
}

