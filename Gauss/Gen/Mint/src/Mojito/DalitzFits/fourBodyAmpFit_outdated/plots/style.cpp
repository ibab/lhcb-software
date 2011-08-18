// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
void style(float s=1.0){
  cout << "running style.C" << endl;

  float margin=0.17;
  float marginX, marginY;
  float marginXL,marginXR, marginYT, marginYB;

  TStyle *style = new TStyle("style", "Jonas' 2D Default");

  style->SetHistMinimumZero();

  style->SetLabelOffset(1.2);

  marginX=margin;
  marginY=margin*0.75;
  marginXL=marginX; 
  marginXR=marginX;
  marginYT=0.15*marginY;//marginY; 
  marginYB=marginY;

  float dim = 400;
  style->SetCanvasDefH(dim*(1+marginYT+marginYB));
  style->SetCanvasDefW(dim*(1+marginXL+marginXR));

  //  style->SetOptStat(111110);
  style->SetOptStat(0);
  style->SetOptFit(1110);
  float LabelSize=s*0.05;
  style->SetLabelSize(LabelSize,"X");
  style->SetLabelSize(LabelSize,"Y");
  style->SetLabelSize(LabelSize,"Z");
  style->SetLabelOffset(0.2*LabelSize);

  style->SetTextSize(s*0.05);
  style->SetTitleSize(s*0.05,"X");
  style->SetTitleSize(s*0.05,"Y");
  style->SetTitleSize(s*0.05,"Z");
  style->SetStatFontSize(s*0.05);

  style->SetPadBottomMargin(marginYB);
  style->SetPadTopMargin(marginYT);
  style->SetPadLeftMargin(marginXL);
  style->SetPadRightMargin(marginXR);

  style->SetStatX(1.0-marginXR);
  style->SetStatY(1.0-marginYT);
  style->SetStatH(0.22);
  style->SetStatW(0.22);

  style->SetStatStyle(0);
  style->SetStatBorderSize(1);
  style->SetTitleBorderSize(0);
  style->SetDrawBorder();
  style->SetFrameBorderMode(0);
  style->SetFrameBorderSize(0);
  style->SetPadBorderMode(0);
  style->SetPadBorderSize(0);
  style->SetCanvasBorderMode(0);
  style->SetCanvasBorderSize(0);

  style->SetPadColor(0);
  style->SetCanvasColor(0);
  style->SetTitleColor(1);
  style->SetStatColor(0);

  style->SetLineWidth(2);

  style->SetMarkerStyle(4);
  style->SetMarkerSize(1.0);
  style->SetMarkerColor(1);

  style->SetOptTitle(0);

  gROOT->SetStyle("style");

  gROOT->ForceStyle();
  cout << "done style.C" << endl;
}
