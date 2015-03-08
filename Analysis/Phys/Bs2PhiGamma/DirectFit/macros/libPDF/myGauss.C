void myGauss( TH1F* his , double min = 0, double max=300){
  
  if( 0 != hist){
    hist->Draw();
    TF1* fit = new TF1("Fit", "[0]/[2]/sqrt(2*3.14159)*exp( -0.5 * pow((x-[1])/[2],2) ) + [3] + [4]*x +[5]*x*x +[6]*x*x*x",0,500);
    fit->SetParNames("area","mean","sigma","a","b","c","d");
    
    fit->FixParameter(0, 5000);
    fit->FixParameter(1, 135.);
    fit->FixParameter(2, 10);
    hist->Fit(fit,"","",min,max) ;
    fit->ReleaseParameter(0);
    hist->Fit(fit,"","",min,max) ;
    fit->ReleaseParameter(1);
    fit->ReleaseParameter(2);
    hist->Fit(fit,"","",min,max) ;
    hist->Draw() ;
    c->Update();
  }
  else
    cout << "histogram not found" << endl;
  
}

