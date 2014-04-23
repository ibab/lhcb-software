// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
void twoDStyle(float s=1.0){
  cout << "running twoDStyle.C" << endl;

  float margin=0.12;
  float marginX, marginY;
  float marginXL,marginXR, marginYT, marginYB;

  TStyle *twoDStyle = new TStyle("twoDStyle", "Jonas' 2D Default");

  marginX=margin;
  marginY=margin;
  marginXL=marginX; marginXR=marginX;
  marginYT=marginY; marginYB=marginY;

  float dim = 500;
  twoDStyle->SetCanvasDefH(dim);
  twoDStyle->SetCanvasDefW(dim);

  twoDStyle->SetOptStat(111110);
  twoDStyle->SetOptFit(1110);
  float LabelSize=s*0.05;
  twoDStyle->SetLabelSize(LabelSize,"X");
  twoDStyle->SetLabelSize(LabelSize,"Y");
  twoDStyle->SetLabelSize(LabelSize,"Z");
  //  twoDStyle->SetLabelOffset(LabelSize);

  twoDStyle->SetTextSize(s*0.05);
  twoDStyle->SetTitleSize(s*0.05,"X");
  twoDStyle->SetTitleSize(s*0.05,"Y");
  twoDStyle->SetTitleSize(s*0.05,"Z");
  twoDStyle->SetStatFontSize(s*0.05);

  twoDStyle->SetPadBottomMargin(marginYB);
  twoDStyle->SetPadTopMargin(marginYT);
  twoDStyle->SetPadLeftMargin(marginXL);
  twoDStyle->SetPadRightMargin(marginXR);

  twoDStyle->SetStatX(1.0-marginXR);
  twoDStyle->SetStatY(1.0-marginYT);
  twoDStyle->SetStatH(0.22);
  twoDStyle->SetStatW(0.22);

  twoDStyle->SetStatStyle(0);
  twoDStyle->SetStatBorderSize(1);
  twoDStyle->SetTitleBorderSize(0);
  twoDStyle->SetDrawBorder(0);
  twoDStyle->SetFrameBorderMode(0);
  twoDStyle->SetFrameBorderSize(0);
  twoDStyle->SetPadBorderMode(0);
  twoDStyle->SetPadBorderSize(0);
  twoDStyle->SetCanvasBorderMode(0);
  twoDStyle->SetCanvasBorderSize(0);

  twoDStyle->SetPadColor(0);
  twoDStyle->SetCanvasColor(0);
  twoDStyle->SetTitleColor(0);
  twoDStyle->SetStatColor(0);

  twoDStyle->SetLineWidth(2);

  twoDStyle->SetMarkerStyle(1);
  twoDStyle->SetMarkerSize(1.0);
  twoDStyle->SetMarkerColor(1);

  twoDStyle->SetOptTitle(0);

  gROOT->SetStyle("twoDStyle");

  gROOT->ForceStyle();
  cout << "done twoDStyle.C" << endl;
}
