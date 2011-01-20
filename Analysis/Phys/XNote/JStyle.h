
  int colors[5] = {1,2,3,4,6};
//int colors[5] = {1,4,7,9,16};
  int markers[5] = {22 , 21, 20, 23, 24};
  int markerSize = 1.3;
  int lineWidth = 1.3;
  int font = 132;


void makeEps(TCanvas* can, std::string& str){
  // helper to make pdf in a good way
  can->Print(str.c_str());
  std::string toPdf = "epstopdf " + str ; 
  gSystem->Exec(toPdf.c_str());
}

void setAxisFont(TAxis* axis){
  // set the axis font
  axis->SetTitleFont(font);
  axis->SetLabelFont(font);
} 

void StyleTH2D(TH2D* histo2){
  setAxisFont(histo2 ->GetXaxis());  
  setAxisFont(histo2 ->GetYaxis());  
  histo2->SetTitle("");
}

void StyleGraph(TGraph* graph){
  setAxisFont(graph ->GetXaxis());  
  setAxisFont(graph ->GetYaxis());  
  graph->SetTitle("");
}

void StyleTex(TLatex* tex){
  tex->SetLineWidth(2);
  tex->SetTextFont(132); 
}
