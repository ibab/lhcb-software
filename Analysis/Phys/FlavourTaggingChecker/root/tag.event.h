

cout <<" ----------------------------- event"<<endl;

vector<TArc>  vpoints(0);
vector<TLine> vlines(0);

      
double limitz = 10;
      
//tutte le tracce
for ( int ii=0; ii<N; ii++ ) {

  if(MCID[ii]==0) continue;
  if(fabs(MCz[ii]-kz)>limitz) continue;
  if(fabs(MCx[ii]-kx)>limitz/2) continue;


  TLorentzVector part = 
    build4V( MCP[ii], MCPt[ii], MCphi[ii], MCID[ii] );
  double newx = (limitz-(MCz[ii]-kz))*part.Px()/part.Pz()+(MCx[ii]-kx);
  TLine ar(MCz[ii]-kz, MCx[ii]-kx, limitz, newx );
  ar.SetLineColor(1); 

  cout <<"Signal "<<MCID[ii]<<"  "<<MCz[ii]-kz<<" newx="<<newx<<endl;

  if(bFlag[ii]==1) ar.SetLineColor(4);
  if(bele)  if(ii==iele)  ar.SetLineWidth(4);
  if(bmuon) if(ii==imuon) ar.SetLineWidth(4);
  if(bkaon) if(ii==ikaon) ar.SetLineWidth(4);
  if(vFlag[ii]>0)         ar.SetLineWidth(4);
  vlines.push_back(ar);
 }

//segnale
for (unsigned int ii=0; ii<M; ii++){

  TArc psig(sVz[ii]-kz, sVx[ii]-kx, .1);
  cout <<"Signal "<<sMCID[ii]<<"  mother="<<sMCMothID[ii]
       <<"  "<<sVx[ii]-kx<<"  "<<sVz[ii]-kz<<endl;

  psig.SetFillColor(2); psig.SetLineColor(2); 
  if(sVz[ii]!=20000) vpoints.push_back(psig);


  for (unsigned int jj=0; jj<M; jj++){

    if(ii!=jj && sMCMothID[ii]==sMCID[jj] && sMCMothID[ii]!=0 ) {

      if(sVx[ii] != 0){
	TLine ar(sVz[jj]-kz, sVx[jj]-kx, 
		 sVz[ii]-kz, sVx[ii]-kx );
	ar.SetLineColor(2); ar.SetLineWidth(4); 
	vlines.push_back(ar);
      } else {
	TLorentzVector part = 
	  build4V( sP[ii], sPt[ii], sPhi[ii], sMass[ii] );
	double newx = (limitz-(sVz[jj]-kz))*part.Px()/part.Pz()+(sVx[jj]-kx);
	TLine ar(sVz[jj]-kz, sVx[jj]-kx, limitz, newx );
	ar.SetLineColor(2); ar.SetLineWidth(4); 
	cout <<"Signal "<<sMCID[ii]<<" newx="<<newx<<endl;
	vlines.push_back(ar);
	

      }
    } 
  }
 }

//b oppo
TArc pBO(BOz-kz, BOx-kx, .1);
pBO.SetFillColor(4);
pBO.SetLineColor(4); 
vpoints.push_back(pBO);
TLine mybo(RVz-kz, RVx-kx, BOz-kz, BOx-kx );
mybo.SetLineColor(4); 
vlines.push_back(mybo);

//segnale
TArc pPV(RVz-kz, RVx-kx, .1);
pPV.SetFillColor(3);
pPV.SetLineColor(3); 
vpoints.push_back(pPV);
TLine mybs(RVz-kz, RVx-kx, BSz-kz, BSx-kx );
mybs.SetLineColor(2); 
vlines.push_back(mybs);


for (int i=0; i!=vlines.size();  ++i) vlines.at(i).Draw();
for (int i=0; i!=vpoints.size(); ++i) vpoints.at(i).Draw();
 

//hxz->Fill(BSz-kz, BSx-kx);
//hxz->Draw("same");
visual.Update();
visual.WaitPrimitive();

