// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
int drawResult(){

  int i=0;
  static const int n=28;
  double dindex[n];
  for(int j=0; j<n; j++) dindex[j]=(double)j +2;

  std::string names[n];
  double result[n], error[n], input[n];

  double ey[n]={0};
  
  names[i]="D0->K(1)(1270)+(->K(0)*(1430)0(->K+,pi-),pi+),K-_Amp";   result[i] = 2.40707e+00; error[i] = 6.16982e+00; i++;
  names[i]="D0->K(1)(1270)+(->K(0)*(1430)0(->K+,pi-),pi+),K-_Phase"; result[i] = 1.16326e+01; error[i] = 2.47648e+00; i++;
  names[i]="D0->K(1)(1270)+(->K*(892)0(->K+,pi-),pi+),K-_Amp";       result[i] = 2.13075e-01; error[i] = 1.01695e-01; i++;
  names[i]="D0->K(1)(1270)+(->K*(892)0(->K+,pi-),pi+),K-_Phase";     result[i] = 3.04626e+00; error[i] = 3.82323e-01; i++;
  names[i]="D0->K(1)(1270)bar-(->K(0)*(1430)bar0(->K-,pi+),pi-),K+_Amp"; result[i]= -3.90356e+00; error[i] = 7.70580e+00;i++;
  names[i]="D0->K(1)(1270)bar-(->K(0)*(1430)bar0(->K-,pi+),pi-),K+_Phase"; result[i]= 5.69060e+00; error[i] =   1.65112e+00; i++;
  names[i]="D0->K(1)(1270)bar-(->K*(892)bar0(->K-,pi+),pi-),K+_Amp"; result[i] = 2.13366e-01; error[i] = 9.93695e-02; i++;
  names[i]="D0->K(1)(1270)bar-(->K*(892)bar0(->K-,pi+),pi-),K+_Phase"; result[i]=3.20339e+00; error[i] = 3.80663e-01; i++;
  names[i]="D0->K(1)(1270)bar-(->rho(770)0(->pi-,pi+),K-),K+_Amp"; result[i]=1.00000e+00; error[i] = 0; i++;
  names[i]="D0->K(1)(1270)bar-(->rho(770)0(->pi-,pi+),K-),K+_Phase"; error[i]=0.00000e+00;error[i] = 0; i++;
  names[i]="D0->K(1)(1400)+(->K*(892)0(->K+,pi-),pi+),K-_Amp";       result[i] = 2.09295e-01; error[i] = 1.37115e-01; i++;
  names[i]="D0->K(1)(1400)+(->K*(892)0(->K+,pi-),pi+),K-_Phase";     result[i] = 7.24271e+00; error[i] = 6.00105e-01; i++;
  names[i]="D0->K(1)(1400)bar-(->K*(892)bar0(->K-,pi+),pi-),K+_Amp"; result[i] = 2.14160e-01; error[i] = 1.37832e-01; i++;
  names[i]="D0->K(1)(1400)bar-(->K*(892)bar0(->K-,pi+),pi-),K+_Phase"; result[i] = 7.63231e+00; error[i] =  5.87937e-01; i++;

  names[i]="D0->K*(892)0(->K+,pi-),K*(892)bar0(->K-,pi+)_Amp"; result[i] = 1.68098e-01; error[i] =   7.82965e-02; i++;
  names[i]="D0->K*(892)0(->K+,pi-),K*(892)bar0(->K-,pi+)_Phase"; result[i] =   3.37551e+00; error[i] =   3.61000e-01; i++;
  names[i]="D0->K*(892)0(->K+,pi-),K-,pi+_Amp"; result[i] = 2.61528e-01; error[i] =   2.09230e-01; i++;
  names[i]="D0->K*(892)0(->K+,pi-),K-,pi+_Phase"; result[i] =   6.10073e+00; error[i] =   1.01876e+00; i++;
  names[i]="D0->K*(892)bar0(->K-,pi+),K+,pi-_Amp"; result[i] = 3.03358e-01; error[i] =   2.39305e-01; i++;
  names[i]="D0->K*(892)bar0(->K-,pi+),K+,pi-_Phase"; result[i] =   7.12201e+00; error[i] =   7.34636e-01; i++;
  names[i]="D0->f(0)(980)0(->pi+,pi-),K+,K-_Amp"; result[i] =-5.39231e-01; error[i] =   1.15462e+00; i++;
  names[i]="D0->f(0)(980)0(->pi+,pi-),K+,K-_Phase"; result[i] =   3.47470e+00; error[i] =   1.62595e+00; i++;
  names[i]="D0->phi(1020)0(->K+,K-),pi+,pi-_Amp"; result[i] = 3.13356e-01; error[i] =   2.31994e-01; i++;
  names[i]="D0->phi(1020)0(->K+,K-),pi+,pi-_Phase"; result[i] =   5.87012e+00; error[i] =   5.33445e-01; i++;
  names[i]="D0->phi(1020)0(->K+,K-),rho(770)0(->pi+,pi-)_Amp"; result[i] = 3.39268e-01 ; error[i] =  1.55717e-01; i++;
  names[i]="D0->phi(1020)0(->K+,K-),rho(770)0(->pi+,pi-)_Phase"; result[i] =   3.71908e+00; error[i] =   3.68959e-01; i++;
  names[i]="D0->rho(770)0(->pi+,pi-),K+,K-_Amp"; result[i] = 8.36666e+00; error[i] =   3.73637e+00; i++;
  names[i]="D0->rho(770)0(->pi+,pi-),K+,K-_Phase"; result[i] =  7.71871e+00 ; error[i] =  3.37813e-01; i++;

  cout << " first i " << i << endl;
  i=0;

  

  input[i]=2.70000e-01; i++;
  input[i]=6.18000e+00; i++;
  input[i]=9.40000e-01; i++;
  input[i]=2.10000e-01; i++;
  input[i]=2.70000e-01; i++;
  input[i]=6.18000e+00; i++;
  input[i]=9.40000e-01; i++;
  input[i]=2.10000e-01; i++;
  input[i]=1.00000e+00; i++;
  input[i]=0.00000e+00; i++;
  input[i]=1.18000e+00; i++;
  input[i]=4.52000e+00; i++;
  input[i]=1.18000e+00; i++;
  input[i]=4.52000e+00; i++;
  input[i]=7.80000e-01; i++;
  input[i]=4.90000e-01; i++;
  input[i]=8.30000e-01; i++;
  input[i]=4.08000e+00; i++;
  input[i]=8.30000e-01; i++;
  input[i]=4.08000e+00; i++;
  input[i]=1.82000e+00; i++;
  input[i]=4.19000e+00; i++;
  input[i]=6.00000e-01; i++;
  input[i]=2.84000e+00; i++;
  input[i]=1.30000e+00; i++;
  input[i]=8.60000e-01; i++;
  input[i]=3.20000e+01; i++;
  input[i]=4.85000e+00; i++;

  cout << " i = " << i << endl;

  double xsub[n];
  for(int j=0; j<n; j++)xsub[j] = result[j] - input[j];

  double amps[n/2], ampsE[n/2];
  for(int j=0; j< n/2; j++){
    amps[j] = xsub[2*j];
    ampsE[j] = error[2*j];
  }
  double ampsN[n/2], ampsEN[n/2];
  for(int j=0; j< n/2; j++){
    if(ampsE[j] > 0) ampsN[j] = amps[j]/ampsE[j];
    else ampsN[j]=0;
    ampsEN[j] = 1.0;
  }

  double phases[n/2], phasesE[n/2];
  for(int j=0; j< n/2; j++){
    phases[j] = xsub[2*j+1];
    phasesE[j] = error[2*j+1];
  }
  double phasesN[n/2], phasesEN[n/2];
  for(int j=0; j< n/2; j++){
    if(phasesE[j] > 0) phasesN[j] = phases[j]/phasesE[j];
    else phasesN[j]=0;
    phasesEN[j] = 1.0;
  }

  TCanvas* cow = new TCanvas("cow");
  TGraphErrors *gr = new TGraphErrors(n/2, ampsN, dindex, ampsEN, ey);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);

  cow->Clear();
  gr->Draw("AP");
  cow->Print("ampPull.eps");
  
  gr = new TGraphErrors(n/2, amps, dindex, ampsE, ey);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  cow->Clear();
  gr->Draw("AP");
  cow->Print("amp.eps");
  
  gr = new TGraphErrors(n/2, phasesN, dindex, phasesEN, ey);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  cow->Clear();
  gr->Draw("AP");
  cow->Print("phasesPull.eps");
  
  gr = new TGraphErrors(n/2, phases, dindex, phasesE, ey);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  cow->Clear();
  gr->Draw("AP");
  cow->Print("phases.eps");
  
  return 0;
}
