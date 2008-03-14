
{
/** @file
  * @author Onuora Awunor
  * @date   March 2008
  *
  *
  * This Macro is designed to use the Ntuple information from the                
  * output of TuplesSignal.cpp and TuplesData.cpp in order to extract the        
  * FlavourTagging performance and estimate OmegaSS (Wss) of the same side       
  * kaon Tagger.
  *
  * Channels : Bs->DsPi and Bs->DsMuNu  (could also be used for other channels)
  *  Samples : Signal sample -> Monte Carlo truth
  *            Data sample -> data (for now we will use bb-bar inclusive sample)
  * 
  * The macro extracts the required information in 6 stages:                     
  *                                                                                
  *   First using MC Truth signal sample - "TaggingTuplesSignal.root";
  *                                                                                
  *   Stage 1: Extract the Inclusive Tagging Performance for                        
  *            muon, electron, kaon, and VertexCharge                              
  *                                                                                
  *   Stage 2: Extract the Opposite Side Tagging performance for                   
  *            muon, electron, kaon, muon+kaon, elec+kaon and VertexCharge         
  *                                                                                
  *   Stage 3: use Opposite Side Wrong-tag fraction (OmegaOS) from Stage 2 to      
  *            obtain Same Side Wrong-tag fraction (OmegaSS) using DoubleTagging   
  *            Formula (see LHCb Note 2007-127)                                    
  *                                                                                
  *   Next, using output from Data sample i.e. "TaggingTuplesData.root",                      
  *                                                                                
  *   Stage 4: Obtain histograms of:                                               
  *            a - all candidate events                                            
  *            b - all DoubleTagged (DT) events                                    
  *            c - all DoubleTagged events that agree                              
  *            d - all DoubleTagged events that disagree                           
  *                                                                                
  *   Stage 5: Perform fit on the histograms to extract estimates of a,b,c and d   
  *            in Stage 4 including their errors after background subtraction.     
  *            Use numbers to recalculate F, and hence OmegaSS +/- ErrorOmegaSS.
  *            ( Note that in the instance of low statistics it is impractical to 
  *            racalculate OmegaSS for each Category, hence used F+/-Sigma(F) to
  *            obtain Sigma(OmegaSS) only.              
  *                                                                               
  *   Stage 6: Perform projection of ErrorOmegass/OmegaSS                          
  *            Note that in performing projection, the error of OmegaOS must be    
  *            scaled with luminosity. for Bs->DsPi and Bs->DsMuNu use info from   
  *            LHCb Note 2006-058, pg 9. This must be put in by hand(line 912-946) 
  * 
  *            NB: Make sure that you use the correct offline "Global Cuts" for
  *               the correct channel.
  *            
  **/

///  Plotting Parameters

 TStyle *printStyle= new TStyle("PRINT","Print approved plots style");

  /// use plain black on white colors
  printStyle->SetFrameBorderMode(0);
  printStyle->SetCanvasBorderMode(0);
  printStyle->SetPadBorderMode(0);
  printStyle->SetPadColor(10);
  printStyle->SetCanvasColor(10);
  printStyle->SetTitleColor(1);
  printStyle->SetStatColor(10);
  printStyle->SetFillColor(10);

  /// set the paper & margin sizes
  printStyle->SetPaperSize(20,26);
  printStyle->SetPadTopMargin(0.05);
  printStyle->SetPadRightMargin(0.05);
  printStyle->SetPadBottomMargin(0.16);
  printStyle->SetPadLeftMargin(0.12);

  /// use large Times-Roman fonts
  printStyle->SetTextFont(132);
  printStyle->SetTextSize(0.08);
  printStyle->SetLabelFont(132,"x");
  printStyle->SetLabelFont(132,"y");
  printStyle->SetLabelFont(132,"z");
  printStyle->SetLabelSize(0.05,"x");
  printStyle->SetTitleSize(0.06,"x");
  printStyle->SetLabelSize(0.05,"y");
  printStyle->SetTitleSize(0.06,"y");
  printStyle->SetLabelSize(0.05,"z");
  printStyle->SetTitleSize(0.06,"z");

  /// use bold lines and markers
  printStyle->SetMarkerStyle(8);
  printStyle->SetHistLineWidth(1.85);
  printStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  /// do not display any of the standard histogram decorations
  printStyle->SetOptTitle(0);
  printStyle->SetOptStat(0);
  printStyle->SetOptFit(0);

  /// put tick marks on top and RHS of plots
  printStyle->SetPadTickX(1);
  printStyle->SetPadTickY(1);

  cout << endl << "    For approved plots use: gROOT->SetStyle(\"PRINT\");"
       << endl << endl;

  /// restore the plain style
  gROOT->SetStyle("Plain");

  TFile f("~/DC06_test/jobs/ganga/DC06/addTuples/numbers/DC06/bs2dsmunu/v31/bsdsmu_Signalv19r9V31.root");
//  TFile f("TaggingTuplesSignal.root"); // test Signal bsdsmunu dataset from MC Truth
  TTree* t=(TTree*)f.FindObjectAny("200");
  TCanvas* c1 = new TCanvas();
  f.ls();
  t->Print();

 // Define global Ntuple cuts here!
 TCut Total_cut = "DsMass>1900 && BsMass>3000 && BsMass<5400"; // All Reconst. events
 TCut Global_Bs2DsPi_cut = "L0>0"; // For Bs2DsPi channel
 TCut Global_Bs2DsMuNu_cut = "DsMass>1944 && DsMass<1988 && BsMass>3000 && BsMass<5400 && L0>0";//for Bs2dsMuNu channel
 TCut Global_Bs2DsMuNu_bkg_cut = "DsMass>1940 && BsMass>3000 && BsMass<5400";//for Bs2dsMuNu channel (bkg)

//   Extract Right and Wrong Tags for Inclusive Tagging Performance

 TCut OSMuon_Rtag = "OSMuondec!=0 && OSMuondec==Truetag";///At least a Muons right tag
 TCut OSMuon_Wtag = "OSMuondec!=0 && OSMuondec!=Truetag";///At least a Muons wrong tag
 TCut OSElectron_Rtag = "OSElectrondec!=0 && OSElectrondec==Truetag";///At least an Electrons right tag
 TCut OSElectron_Wtag = "OSElectrondec!=0 && OSElectrondec!=Truetag";///At least an Electrons wrong tag
 TCut OSKaon_Rtag = "OSKaondec!=0 && OSKaondec==Truetag";///At least a OSKaons right tag
 TCut OSKaon_Wtag = "OSKaondec!=0 && OSKaondec!=Truetag";///At least a OSKaons wrong tag
 TCut SSKaon_Rtag = "SSKaondec!=0 && SSKaondec==Truetag";///At least a SSKaons right tag
 TCut SSKaon_Wtag = "SSKaondec!=0 && SSKaondec!=Truetag";///At least a SSKaons wrong tag
 TCut VtxChargedec_Rtag = "VtxChargedec!=0 && VtxChargedec==Truetag";///At least a VtxCharge right tag
 TCut VtxChargedec_Wtag = "VtxChargedec!=0 && VtxChargedec!=Truetag";///At least a VtxCharge wrong tag

 // Extract Right and Wrong Tags for OS Tagging Performance by CATEGORIES  

 TCut Category1_Rtag = "OSKaondec==0 && OSMuondec!=0 && OSMuondec==Truetag"; ///Muons Only
 TCut Category1_Wtag = "OSKaondec==0 && OSMuondec!=0 && OSMuondec!=Truetag";///Muons Only
 TCut Category2_Rtag = "OSKaondec==0 && OSMuondec==0 && OSElectrondec!=0 && OSElectrondec==Truetag"; ///Electrons Only
 TCut Category2_Wtag = "OSKaondec==0 && OSMuondec==0 && OSElectrondec!=0 && OSElectrondec!=Truetag";///Electrons Only
 TCut Category3_Rtag = "OSElectrondec==0 && OSMuondec==0 && OSKaondec!=0 && OSKaondec==Truetag"; /// OSKaons only
 TCut Category3_Wtag = "OSElectrondec==0 && OSMuondec==0 && OSKaondec!=0 && OSKaondec!=Truetag"; ///OSKaons only
 TCut Category4_Rtag = "OSMuondec==OSKaondec && OSMuondec!=0 && OSMuondec==Truetag"; ///OSKaons+Muons
 TCut Category4_Wtag = "OSMuondec==OSKaondec && OSMuondec!=0 && OSMuondec!=Truetag";///OSKaons+Muons
 TCut Category5_Rtag = "OSElectrondec==OSKaondec && OSMuondec==0 && OSElectrondec!=0 && OSElectrondec==Truetag"; ///OSKaons + OSElectrons
 TCut Category5_Wtag = "OSElectrondec==OSKaondec && OSMuondec==0 && OSElectrondec!=0 && OSElectrondec!=Truetag";///OSKaons + OSElectrons
 TCut Category6_Rtag = "OSMuondec==0 && OSElectrondec==0 && OSKaondec==0 && VtxChargedec!=0 && VtxChargedec==Truetag"; ///VtxCharge
 TCut Category6_Wtag = "OSMuondec==0 && OSElectrondec==0 && OSKaondec==0 && VtxChargedec!=0 && VtxChargedec!=Truetag";///VtxCharge


 double OSMuon_R=0.,OSMuon_W=0.,OSElectron_R=0.,OSElectron_W=0., OSKaon_R=0., OSKaon_W=0.;
 double SSKaon_R =0.,SSKaon_W=0., VtxChargedec_R=0., VtxChargedec_W=0.;
 double Category1_R=0., Category1_W=0., Category2_R=0., Category2_W=0., Category3_R=0., Category3_W=0.;
 double Category4_R=0., Category4_W=0., Category5_R=0., Category5_W=0., Category6_R=0., Category6_W=0.;
 
  OSMuon_R = t->Draw("DsMass",OSMuon_Rtag && Global_Bs2DsMuNu_cut);
  OSMuon_W = t->Draw("DsMass",OSMuon_Wtag && Global_Bs2DsMuNu_cut);
  OSElectron_R = t->Draw("DsMass",OSElectron_Rtag && Global_Bs2DsMuNu_cut);
  OSElectron_W = t->Draw("DsMass",OSElectron_Wtag && Global_Bs2DsMuNu_cut);
  OSKaon_R = t->Draw("DsMass",OSKaon_Rtag && Global_Bs2DsMuNu_cut);
  OSKaon_W = t->Draw("DsMass",OSKaon_Wtag && Global_Bs2DsMuNu_cut);
  SSKaon_R = t->Draw("DsMass",SSKaon_Rtag && Global_Bs2DsMuNu_cut);
  SSKaon_W = t->Draw("DsMass",SSKaon_Wtag && Global_Bs2DsMuNu_cut);
  VtxChargedec_R = t->Draw("DsMass",VtxChargedec_Rtag && Global_Bs2DsMuNu_cut);
  VtxChargedec_W = t->Draw("DsMass",VtxChargedec_Wtag && Global_Bs2DsMuNu_cut);

  /// cout << "OSMuon_R = " << OSMuon_R <<  endl  ;
  /// cout << "OSMuon_W = " << OSMuon_W <<  endl  ;
  /// cout << "OSElectron_R = " << OSElectron_R <<  endl  ;
  /// cout << "OSElectron_W = " << OSElectron_W <<  endl  ;
  /// cout << "OSKaon_R = " << OSKaon_R <<  endl  ;
  /// cout << "OSKaon_W = " << OSKaon_W <<  endl  ;
  /// cout << "SSKaon_R = " << SSKaon_R <<  endl  ;
  /// cout << "SSKaon_W = " << SSKaon_W <<  endl  ;
  /// cout << "VtxChargedec_R = " << VtxChargedec_R <<  endl  ;
  /// cout << "VtxChargedec_W = " << VtxChargedec_W <<  endl  ;

  Category1_R = t->Draw("DsMass",Category1_Rtag && Global_Bs2DsMuNu_cut);
  Category1_W = t->Draw("DsMass",Category1_Wtag && Global_Bs2DsMuNu_cut);
  Category2_R = t->Draw("DsMass",Category2_Rtag && Global_Bs2DsMuNu_cut);
  Category2_W = t->Draw("DsMass",Category2_Wtag && Global_Bs2DsMuNu_cut);
  Category3_R = t->Draw("DsMass",Category3_Rtag && Global_Bs2DsMuNu_cut);
  Category3_W = t->Draw("DsMass",Category3_Wtag && Global_Bs2DsMuNu_cut);
  Category4_R = t->Draw("DsMass",Category4_Rtag && Global_Bs2DsMuNu_cut);
  Category4_W = t->Draw("DsMass",Category4_Wtag && Global_Bs2DsMuNu_cut );
  Category5_R = t->Draw("DsMass",Category5_Rtag && Global_Bs2DsMuNu_cut);
  Category5_W = t->Draw("DsMass",Category5_Wtag && Global_Bs2DsMuNu_cut);
  Category6_R = t->Draw("DsMass",Category6_Rtag && Global_Bs2DsMuNu_cut);
  Category6_W = t->Draw("DsMass",Category6_Wtag && Global_Bs2DsMuNu_cut);


  double Total = 0.;
   Total = t->Draw("DsMass", Global_Bs2DsMuNu_cut); 
   

//#########################
//                                                     
//          STAGE 1       
//                        
//#########################

 
 cout<<" "<< endl;
 cout<<" Inclusive Tagging Performance "<< endl;
 cout<<" "<< endl;

   // Muon
  if(OSMuon_R+OSMuon_W == 0) continue; //empty category

    double muon_utag = Total-OSMuon_R-OSMuon_W;       // untagged
    double muon_omtag = OSMuon_W/(OSMuon_R+OSMuon_W);
    double muon_eftag = (OSMuon_R+OSMuon_W)/Total;           // tagging efficiency
    double muon_epsil = muon_eftag*pow(1-2*muon_omtag,2);      // effective efficiency
    if(OSMuon_R<OSMuon_W) muon_epsil= -muon_epsil;

    ///errors on efficiency and omega
    double muon_eftag_err= sqrt((OSMuon_R*muon_utag + muon_utag*OSMuon_W)/Total)/Total;
    double muon_omtag_err= sqrt( OSMuon_R*OSMuon_W /(OSMuon_R+OSMuon_W) ) / (OSMuon_R+OSMuon_W);

    double muon_epsilerr = sqrt((pow(OSMuon_R - OSMuon_W,2)*
                     (-(pow(OSMuon_R - OSMuon_W,2)*(OSMuon_R +OSMuon_W))+Total
                      *(pow(OSMuon_R,2) +14*OSMuon_R*OSMuon_W+ pow(OSMuon_W,2))))
                    /(pow(OSMuon_R+OSMuon_W+muon_utag,3)*pow(OSMuon_R + OSMuon_W,3)));

    
    cout<<"     Effec Tag Eff.  "<< "   Tag Eff   " << "   Omega   " << "   Rtag    "<< "     Wtag "<< endl;
    
    cout<<" OSMuon : "<< muon_epsil*100 << "+-" << muon_epsilerr*100 
        <<" "<< muon_eftag*100 << "+-" <<muon_eftag_err*100
        <<" "<< muon_omtag*100 << "+-" <<muon_omtag_err*100
        <<" "<< OSMuon_R
        <<" "<< OSMuon_W 
        << endl;


  // OSElectron
  if(OSElectron_R+OSElectron_W == 0) continue; //empty category

    double oselectron_utag = Total-OSElectron_R-OSElectron_W;       // untagged
    double oselectron_omtag = OSElectron_W/(OSElectron_R+OSElectron_W);
    double oselectron_eftag = (OSElectron_R+OSElectron_W)/Total;           // tagging efficiency
    double oselectron_epsil = oselectron_eftag*pow(1-2*oselectron_omtag,2);      // effective efficiency
    if(OSElectron_R<OSElectron_W) oselectron_epsil= -oselectron_epsil;

    ///errors on efficiency and omega
    double oselectron_eftag_err= sqrt((OSElectron_R*oselectron_utag + oselectron_utag*OSElectron_W)/Total)/Total;
    double oselectron_omtag_err= sqrt( OSElectron_R*OSElectron_W /(OSElectron_R+OSElectron_W) ) / (OSElectron_R+OSElectron_W);

    double oselectron_epsilerr = sqrt((pow(OSElectron_R - OSElectron_W,2)*
                     (-(pow(OSElectron_R - OSElectron_W,2)*(OSElectron_R +OSElectron_W))+Total
                      *(pow(OSElectron_R,2) +14*OSElectron_R*OSElectron_W+ pow(OSElectron_W,2))))
                    /(pow(OSElectron_R+OSElectron_W+oselectron_utag,3)*pow(OSElectron_R + OSElectron_W,3)));


    cout<<" OSElectron : "<< oselectron_epsil*100 << "+-" << oselectron_epsilerr*100 
        <<" "<< oselectron_eftag*100 << "+-" <<oselectron_eftag_err*100
        <<" "<< oselectron_omtag*100 << "+-" <<oselectron_omtag_err*100
        <<" "<< OSElectron_R
        <<" "<< OSElectron_W 
        << endl;

  // OSKaon
  if(OSKaon_R+OSKaon_W == 0) continue; //empty category

    double oskaon_utag = Total-OSKaon_R-OSKaon_W;       // untagged
    double oskaon_omtag = OSKaon_W/(OSKaon_R+OSKaon_W);
    double oskaon_eftag = (OSKaon_R+OSKaon_W)/Total;           // tagging efficiency
    double oskaon_epsil = oskaon_eftag*pow(1-2*oskaon_omtag,2);      // effective efficiency
    if(OSKaon_R<OSKaon_W) oskaon_epsil= -oskaon_epsil;

    ///errors on efficiency and omega
    double oskaon_eftag_err= sqrt((OSKaon_R*oskaon_utag + oskaon_utag*OSKaon_W)/Total)/Total;
    double oskaon_omtag_err= sqrt( OSKaon_R*OSKaon_W /(OSKaon_R+OSKaon_W) ) / (OSKaon_R+OSKaon_W);

    double oskaon_epsilerr = sqrt((pow(OSKaon_R - OSKaon_W,2)*
                     (-(pow(OSKaon_R - OSKaon_W,2)*(OSKaon_R +OSKaon_W))+Total
                      *(pow(OSKaon_R,2) +14*OSKaon_R*OSKaon_W+ pow(OSKaon_W,2))))
                    /(pow(OSKaon_R+OSKaon_W+oskaon_utag,3)*pow(OSKaon_R + OSKaon_W,3)));


    cout<<" OSKaon : "<< oskaon_epsil*100 << "+-" << oskaon_epsilerr*100 
        <<" "<< oskaon_eftag*100 << "+-" <<oskaon_eftag_err*100
        <<" "<< oskaon_omtag*100 << "+-" <<oskaon_omtag_err*100
        <<" "<< OSKaon_R
        <<" "<< OSKaon_W 
        << endl;

  // VtxCharge
  if(VtxChargedec_R+VtxChargedec_W == 0) continue; //empty category

    double vtxcharge_utag = Total-VtxChargedec_R-VtxChargedec_W;       // untagged
    double vtxcharge_omtag = VtxChargedec_W/(VtxChargedec_R+VtxChargedec_W);
    double vtxcharge_eftag = (VtxChargedec_R+VtxChargedec_W)/Total;           // tagging efficiency
    double vtxcharge_epsil = vtxcharge_eftag*pow(1-2*vtxcharge_omtag,2);      // effective efficiency
    if(VtxChargedec_R<VtxChargedec_W) vtxcharge_epsil= -vtxcharge_epsil;

    ///errors on efficiency and omega
    double vtxcharge_eftag_err= sqrt((VtxChargedec_R*vtxcharge_utag + vtxcharge_utag*VtxChargedec_W)/Total)/Total; 
  double vtxcharge_omtag_err=sqrt(VtxChargedec_R*VtxChargedec_W/(VtxChargedec_R+VtxChargedec_W))/(VtxChargedec_R+VtxChargedec_W);

    double vtxcharge_epsilerr = sqrt((pow(VtxChargedec_R - VtxChargedec_W,2)*
                     (-(pow(VtxChargedec_R - VtxChargedec_W,2)*(VtxChargedec_R +VtxChargedec_W))+Total
                      *(pow(VtxChargedec_R,2) +14*VtxChargedec_R*VtxChargedec_W+ pow(VtxChargedec_W,2))))
                    /(pow(VtxChargedec_R+VtxChargedec_W+vtxcharge_utag,3)*pow(VtxChargedec_R + VtxChargedec_W,3)));


    cout<<" VtxChargedec : "<< vtxcharge_epsil*100 << "+-" << vtxcharge_epsilerr*100 
        <<" "<< vtxcharge_eftag*100 << "+-" <<vtxcharge_eftag_err*100
        <<" "<< vtxcharge_omtag*100 << "+-" <<vtxcharge_omtag_err*100
        <<" "<< VtxChargedec_R
        <<" "<< VtxChargedec_W 
        << endl;

  // SSKaon
  if(SSKaon_R+SSKaon_W == 0) continue; //empty category

    double sskaon_utag = Total-SSKaon_R-SSKaon_W;       // untagged
    double sskaon_omtag = SSKaon_W/(SSKaon_R+SSKaon_W);
    double sskaon_eftag = (SSKaon_R+SSKaon_W)/Total;           // tagging efficiency
    double sskaon_epsil = sskaon_eftag*pow(1-2*sskaon_omtag,2);      // effective efficiency
    if(SSKaon_R<SSKaon_W) sskaon_epsil= -sskaon_epsil;

    ///errors on efficiency and omega
    double sskaon_eftag_err= sqrt((SSKaon_R*sskaon_utag + sskaon_utag*SSKaon_W)/Total)/Total;
    double sskaon_omtag_err= sqrt( SSKaon_R*SSKaon_W /(SSKaon_R+SSKaon_W) ) / (SSKaon_R+SSKaon_W);

    double sskaon_epsilerr = sqrt((pow(SSKaon_R - SSKaon_W,2)*
                     (-(pow(SSKaon_R - SSKaon_W,2)*(SSKaon_R +SSKaon_W))+Total
                      *(pow(SSKaon_R,2) +14*SSKaon_R*SSKaon_W+ pow(SSKaon_W,2))))
                    /(pow(SSKaon_R+SSKaon_W+sskaon_utag,3)*pow(SSKaon_R + SSKaon_W,3)));


    cout<<" SSKaon : "<< sskaon_epsil*100 << "+-" << sskaon_epsilerr*100 
        <<" "<< sskaon_eftag*100 << "+-" <<sskaon_eftag_err*100
        <<" "<< sskaon_omtag*100 << "+-" <<sskaon_omtag_err*100
        <<" "<< SSKaon_R
        <<" "<< SSKaon_W 
        << endl;



//#########################
//                                                     
//          STAGE 2       
//                        
//#########################

 cout<<"=================================================== "<< endl;
 cout<<" "<< endl;
 cout<<" OS Tagging Performance "<< endl;
 cout<<" "<< endl;

// Categories

  // Category1 - OS Muons Only
  if(Category1_R+Category1_W == 0) continue; ///empty category

    double category1_utag = Total-Category1_R-Category1_W;       /// untagged
    double category1_omtag = Category1_W/(Category1_R+Category1_W);
    double category1_eftag = (Category1_R+Category1_W)/Total;           /// tagging efficiency
    double category1_epsil = category1_eftag*pow(1-2*category1_omtag,2);      /// effective efficiency
    if(Category1_R<Category1_W) category1_epsil= -category1_epsil;

    ///errors on efficiency and omega
    double category1_eftag_err= sqrt((Category1_R*category1_utag + category1_utag*Category1_W)/Total)/Total;
    double category1_omtag_err= sqrt( Category1_R*Category1_W /(Category1_R+Category1_W) ) / (Category1_R+Category1_W);

    double category1_epsilerr = sqrt((pow(Category1_R - Category1_W,2)*
                     (-(pow(Category1_R - Category1_W,2)*(Category1_R +Category1_W))+Total
                      *(pow(Category1_R,2) +14*Category1_R*Category1_W+ pow(Category1_W,2))))
                    /(pow(Category1_R+Category1_W+category1_utag,3)*pow(Category1_R + Category1_W,3)));


    cout<<" Category1 : "<< category1_epsil*100 << "+-" << category1_epsilerr*100 
        <<" "<< category1_eftag*100 << "+-" <<category1_eftag_err*100
        <<" "<< category1_omtag*100 << "+-" <<category1_omtag_err*100
        <<" "<< Category1_R
        <<" "<< Category1_W 
        <<"//Mu"<< endl;

  // Category2 - OS Electrons Only
  if(Category2_R+Category2_W == 0) continue; ///empty category

    double category2_utag = Total-Category2_R-Category2_W;       /// untagged
    double category2_omtag = Category2_W/(Category2_R+Category2_W);
    double category2_eftag = (Category2_R+Category2_W)/Total;           /// tagging efficiency
    double category2_epsil = category2_eftag*pow(1-2*category2_omtag,2);      /// effective efficiency
    if(Category2_R<Category2_W) category2_epsil= -category2_epsil;

    ///errors on efficiency and omega
    double category2_eftag_err= sqrt((Category2_R*category2_utag + category2_utag*Category2_W)/Total)/Total;
    double category2_omtag_err= sqrt( Category2_R*Category2_W /(Category2_R+Category2_W) ) / (Category2_R+Category2_W);

    double category2_epsilerr = sqrt((pow(Category2_R - Category2_W,2)*
                     (-(pow(Category2_R - Category2_W,2)*(Category2_R +Category2_W))+Total
                      *(pow(Category2_R,2) +14*Category2_R*Category2_W+ pow(Category2_W,2))))
                    /(pow(Category2_R+Category2_W+category2_utag,3)*pow(Category2_R + Category2_W,3)));

    cout<<" Category2 : "<< category2_epsil*100 << "+-" << category2_epsilerr*100 
        <<" "<< category2_eftag*100 << "+-" <<category2_eftag_err*100
        <<" "<< category2_omtag*100 << "+-" <<category2_omtag_err*100
        <<" "<< Category2_R
        <<" "<< Category2_W 
        <<"//Elec "<< endl;

  // Category3 - OS Kaons Only
          if(Category3_R+Category3_W == 0) continue; ///empty category

    double category3_utag = Total-Category3_R-Category3_W;       /// untagged
    double category3_omtag = Category3_W/(Category3_R+Category3_W);
    double category3_eftag = (Category3_R+Category3_W)/Total;           /// tagging efficiency
    double category3_epsil = category3_eftag*pow(1-2*category3_omtag,2);      /// effective efficiency
    if(Category3_R<Category3_W) category3_epsil= -category3_epsil;

    ///errors on efficiency and omega
    double category3_eftag_err= sqrt((Category3_R*category3_utag + category3_utag*Category3_W)/Total)/Total;
    double category3_omtag_err= sqrt( Category3_R*Category3_W /(Category3_R+Category3_W) ) / (Category3_R+Category3_W);

    double category3_epsilerr = sqrt((pow(Category3_R - Category3_W,2)*
                     (-(pow(Category3_R - Category3_W,2)*(Category3_R +Category3_W))+Total
                      *(pow(Category3_R,2) +14*Category3_R*Category3_W+ pow(Category3_W,2))))
                    /(pow(Category3_R+Category3_W+category3_utag,3)*pow(Category3_R + Category3_W,3)));

    cout<<" Category3 : "<< category3_epsil*100 << "+-" << category3_epsilerr*100 
        <<" "<< category3_eftag*100 << "+-" <<category3_eftag_err*100
        <<" "<< category3_omtag*100 << "+-" <<category3_omtag_err*100
        <<" "<< Category3_R
        <<" "<< Category3_W 
        <<"//Kaon"<< endl;

  // Category4 - OS Kaons+OSMuons Only 
  if(Category4_R+Category4_W == 0) continue; ///empty category

    double category4_utag = Total-Category4_R-Category4_W;       /// untagged
    double category4_omtag = Category4_W/(Category4_R+Category4_W);
    double category4_eftag = (Category4_R+Category4_W)/Total;           /// tagging efficiency
    double category4_epsil = category4_eftag*pow(1-2*category4_omtag,2);      /// effective efficiency
    if(Category4_R<Category4_W) category4_epsil= -category4_epsil;

    ///errors on efficiency and omega
    double category4_eftag_err= sqrt((Category4_R*category4_utag + category4_utag*Category4_W)/Total)/Total;
    double category4_omtag_err= sqrt( Category4_R*Category4_W /(Category4_R+Category4_W) ) / (Category4_R+Category4_W);

    double category4_epsilerr = sqrt((pow(Category4_R - Category4_W,2)*
                     (-(pow(Category4_R - Category4_W,2)*(Category4_R +Category4_W))+Total
                      *(pow(Category4_R,2) +14*Category4_R*Category4_W+ pow(Category4_W,2))))
                    /(pow(Category4_R+Category4_W+category4_utag,3)*pow(Category4_R + Category4_W,3)));

    cout<<" Category4 : "<< category4_epsil*100 << "+-" << category4_epsilerr*100 
        <<" "<< category4_eftag*100 << "+-" <<category4_eftag_err*100
        <<" "<< category4_omtag*100 << "+-" <<category4_omtag_err*100
        <<" "<< Category4_R
        <<" "<< Category4_W 
        <<"//Mu+Kaon"<< endl;

  // Category5 - OSKaons + OSElectrons Only
  if(Category5_R+Category5_W == 0) continue; ///empty category

    double category5_utag = Total-Category5_R-Category5_W;       /// untagged
    double category5_omtag = Category5_W/(Category5_R+Category5_W);
    double category5_eftag = (Category5_R+Category5_W)/Total;           /// tagging efficiency
    double category5_epsil = category5_eftag*pow(1-2*category5_omtag,2);      /// effective efficiency
    if(Category5_R<Category5_W) category5_epsil= -category5_epsil;

    ///errors on efficiency and omega
    double category5_eftag_err= sqrt((Category5_R*category5_utag + category5_utag*Category5_W)/Total)/Total;
    double category5_omtag_err= sqrt( Category5_R*Category5_W /(Category5_R+Category5_W) ) / (Category5_R+Category5_W);

    double category5_epsilerr = sqrt((pow(Category5_R - Category5_W,2)*
                     (-(pow(Category5_R - Category5_W,2)*(Category5_R +Category5_W))+Total
                      *(pow(Category5_R,2) +14*Category5_R*Category5_W+ pow(Category5_W,2))))
                    /(pow(Category5_R+Category5_W+category5_utag,3)*pow(Category5_R + Category5_W,3)));


    cout<<" Category5 : "<< category5_epsil*100 << "+-" << category5_epsilerr*100 
        <<" "<< category5_eftag*100 << "+-" <<category5_eftag_err*100
        <<" "<< category5_omtag*100 << "+-" <<category5_omtag_err*100
        <<" "<< Category5_R
        <<" "<< Category5_W
        << "// elec+Kaon" << endl;

  // Category6 - Vertex Only
  if(Category6_R+Category6_W == 0) continue; ///empty category

    double category6_utag = Total-Category6_R-Category6_W;       /// untagged
    double category6_omtag = Category6_W/(Category6_R+Category6_W);
    double category6_eftag = (Category6_R+Category6_W)/Total;           /// tagging efficiency
    double category6_epsil = category6_eftag*pow(1-2*category6_omtag,2);      /// effective efficiency
    if(Category6_R<Category6_W) category6_epsil= -category6_epsil;

    ///errors on efficiency and omega
    double category6_eftag_err= sqrt((Category6_R*category6_utag + category6_utag*Category6_W)/Total)/Total;
    double category6_omtag_err= sqrt( Category6_R*Category6_W /(Category6_R+Category6_W) ) / (Category6_R+Category6_W);

    double category6_epsilerr = sqrt((pow(Category6_R - Category6_W,2)*
                     (-(pow(Category6_R - Category6_W,2)*(Category6_R +Category6_W))+Total
                      *(pow(Category6_R,2) +14*Category6_R*Category6_W+ pow(Category6_W,2))))
                    /(pow(Category6_R+Category6_W+category6_utag,3)*pow(Category6_R + Category6_W,3)));

    cout<<" Category6 : "<< category6_epsil*100 << "+-" << category6_epsilerr*100 
        <<" "<< category6_eftag*100 << "+-" <<category6_eftag_err*100
        <<" "<< category6_omtag*100 << "+-" <<category6_omtag_err*100
        <<" "<< Category6_R
        <<" "<< Category6_W 
        << "//Vtx "<< endl;

    double eftag = category1_eftag + category2_eftag + category3_eftag + category4_eftag + category5_eftag + category6_eftag ;
    double epsil = category1_epsil + category2_epsil + category3_epsil + category4_epsil + category5_epsil + category6_epsil ;
    double rtt = Category1_R + Category2_R + Category3_R + Category4_R + Category5_R + Category6_R ;
    double wtt = Category1_W + Category2_W + Category3_W + Category4_W + Category5_W + Category6_W ;
    double avw_invert=(1-sqrt(fabs(epsil)/eftag))/2;
    if(epsil<0) avw_invert= 1-avw_invert;
    double utt = Total-rtt-wtt;
    double eftagerr= sqrt((rtt*utt + utt*wtt)/Total)/Total;
    double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);
    double epsilerr = sqrt((category1_epsilerr*category1_epsilerr)+(category2_epsilerr*category2_epsilerr)+
                           (category3_epsilerr*category3_epsilerr)+(category4_epsilerr*category4_epsilerr)+
                           (category5_epsilerr*category5_epsilerr)+(category6_epsilerr*category6_epsilerr));

 cout << "TDR Combined Performance = " << endl;
 cout<<" "<< endl;
 cout << "Effective Combined Tagging Efficiency =  "<< epsil*100 << " +/- "<<epsilerr*100<< " %"<<endl;
 cout << "Tagging Efficiency =  "<< eftag*100 << " +/- "<<eftagerr*100<< " %"<< endl;
 cout << "Wrong Tag fraction =  "<< avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endl;
 cout<<" "<< endl;
 cout<<"========================================================= "<< endl;

 //  Cuts for Calculating OmegaSS using DoubleTagging
  
 gROOT->cd();///change directory

 TCut Ndt_cut = "SSKaondec!=0 && (OSMuondec!=0||OSKaondec!=0||OSElectrondec!=0||VtxChargedec!=0)"; /// Cut to select All double tagged events
 TCut Nagree_cut = "SSKaondec!=0 && (SSKaondec==OSMuondec||SSKaondec==OSKaondec||SSKaondec==OSElectrondec||SSKaondec==VtxChargedec)"; /// cut to select All double tagged that agree
  TCut Ndisagree_cut = "((SSKaondec!=0&&OSMuondec!=0&&SSKaondec!=OSMuondec)||(SSKaondec!=0&&OSElectrondec!=0&&SSKaondec!=OSElectrondec)||(SSKaondec!=0&&OSKaondec!=0&&SSKaondec!=OSKaondec)||(SSKaondec!=0&&VtxChargedec!=0&&SSKaondec!=VtxChargedec))"; /// cut to select All  double tagged events that disagree



 double Ndt = 0., Nagree = 0., Ndisagree = 0.;
 
 Ndt = t->Draw("DsMass", Ndt_cut && Global_Bs2DsMuNu_cut);/// extracting double tagged events
 Nagree = t->Draw("DsMass",  Nagree_cut && Global_Bs2DsMuNu_cut);/// extracting double tagged events that agree
 Ndisagree = t->Draw("DsMass", Ndisagree_cut && Global_Bs2DsMuNu_cut);/// extracting double tagged events that disagree
 cout << "Total = " << Total <<  endl  ;
 cout << "Ndt = " << Ndt <<  endl  ;
 cout << "Nagree = " << Nagree <<  endl  ;
 cout << "Ndisagree = " << Ndisagree <<  endl  ;
 cout << "============== "<<  endl  ;
 
 // The Categories

 TCut Nagree_Category1 = "OSKaondec==0 && OSMuondec!=0 && SSKaondec==OSMuondec";
 TCut Ndt_Category1 = "OSKaondec==0 && OSMuondec!=0 && SSKaondec!=0";
 TCut Nagree_Category2 = "OSKaondec==0 && OSMuondec==0 && OSElectrondec!=0 && SSKaondec==OSElectrondec";
 TCut Ndt_Category2 =  "OSKaondec==0 && OSMuondec==0 && OSElectrondec!=0 && SSKaondec!=0";
 TCut Nagree_Category3 = "OSElectrondec==0 && OSMuondec==0 && OSKaondec!=0 && SSKaondec==OSKaondec";
 TCut Ndt_Category3 = "OSElectrondec==0 && OSMuondec==0 && OSKaondec!=0 && SSKaondec!=0";
 TCut Nagree_Category4 = "OSMuondec==OSKaondec && OSMuondec!=0 && SSKaondec==OSMuondec";
 TCut Ndt_Category4 = "OSMuondec==OSKaondec && OSMuondec!=0 && SSKaondec!=0";
 TCut Nagree_Category5 = "OSElectrondec==OSKaondec && OSMuondec==0 && OSElectrondec!=0 && SSKaondec==OSElectrondec"; 
 TCut Ndt_Category5 = "OSElectrondec==OSKaondec && OSMuondec==0 && OSElectrondec!=0 && SSKaondec!=0";
 TCut Nagree_Category6 = "OSMuondec==0 && OSElectrondec==0 && OSKaondec==0 && VtxChargedec!=0 && SSKaondec==VtxChargedec";  
 TCut Ndt_Category6 = "OSMuondec==0 && OSElectrondec==0 && OSKaondec==0 && VtxChargedec!=0 && SSKaondec!=0";
 
 double Nagree_cat =0., Nagree_cat2 =0., Nagree_cat3 =0., Nagree_cat4 =0., Nagree_cat5 =0., Nagree_cat6 =0.;
 double Ndt_cat =0., Ndt_cat2 =0., Ndt_cat3 =0., Ndt_cat4 =0., Ndt_cat5 =0., Ndt_cat6 =0.;
 
 
 Nagree_cat = t->Draw("DsMass", Nagree_Category1 && Global_Bs2DsMuNu_cut);
 Ndt_cat = t->Draw("DsMass", Ndt_Category1 && Global_Bs2DsMuNu_cut);
 Nagree_cat2 = t->Draw("DsMass", Nagree_Category2 && Global_Bs2DsMuNu_cut);
 Ndt_cat2 = t->Draw("DsMass", Ndt_Category2 && Global_Bs2DsMuNu_cut);
 Nagree_cat3 = t->Draw("DsMass", Nagree_Category3 && Global_Bs2DsMuNu_cut);
 Ndt_cat3 = t->Draw("DsMass", Ndt_Category3 && Global_Bs2DsMuNu_cut);
 Nagree_cat4 = t->Draw("DsMass", Nagree_Category4 && Global_Bs2DsMuNu_cut);
 Ndt_cat4 = t->Draw("DsMass", Ndt_Category4 && Global_Bs2DsMuNu_cut);
 Nagree_cat5 = t->Draw("DsMass", Nagree_Category5 && Global_Bs2DsMuNu_cut);
 Ndt_cat5 = t->Draw("DsMass", Ndt_Category5 && Global_Bs2DsMuNu_cut);
 Nagree_cat6 = t->Draw("DsMass", Nagree_Category6 && Global_Bs2DsMuNu_cut);
 Ndt_cat6 = t->Draw("DsMass", Ndt_Category6 && Global_Bs2DsMuNu_cut);
 cout << " Nagree_cat = " << Nagree_cat <<  endl  ;
 cout << " Ndt_cat = " << Ndt_cat <<  endl  ;
 cout << " Nagree_cat2 = " << Nagree_cat2 <<  endl  ;
 cout << " Ndt_cat2 = " << Ndt_cat2 <<  endl  ;
 cout << " Nagree_cat3 = " << Nagree_cat3 <<  endl  ;
 cout << " Ndt_cat3 = " << Ndt_cat3 <<  endl  ;
 cout << " Nagree_cat4 = " << Nagree_cat4 <<  endl  ;
 cout << " Ndt_cat4 = " << Ndt_cat4 <<  endl  ;
 cout << " Nagree_cat5 = " << Nagree_cat5 <<  endl  ;
 cout << " Ndt_cat5 = " << Ndt_cat5 <<  endl  ;
 cout << " Nagree_cat6 = " << Nagree_cat6 <<  endl  ;
 cout << " Ndt_cat6 = " << Ndt_cat6 <<  endl  ;

// Calculating OmegaSS per Category

 if (Ndt_cat == 0.) double OmegaSS_Category1, ErrorOmegaSS_Category1 = 0.;
 if (Ndt_cat != 0.)
 double OmegaSS_Category1 = (1-category1_omtag-(Nagree_cat/Ndt_cat))/(1-2*category1_omtag); 
 if (Ndt_cat != 0.)
 double ErrorOmegaSS_Category1 =  sqrt(((1.-2.*(Nagree_cat/Ndt_cat))*(1.-2.*(Nagree_cat/Ndt_cat))*
                                        (category1_omtag_err*category1_omtag_err)/((1.-2.*category1_omtag)*
                                                                                   (1.-2.*category1_omtag)*
                                                                                   (1.-2.*category1_omtag)*
                                                                                   (1.-2.*category1_omtag)))+
                                       (((Nagree_cat/Ndt_cat)*(1-(Nagree_cat/Ndt_cat)))/((1.-2.*category1_omtag)*
                                                                                             (1.-2.*category1_omtag)*Ndt_cat)));
 
 if (Ndt_cat2 == 0.) double OmegaSS_Category2, ErrorOmegaSS_Category2 = 0.;
 if (Ndt_cat2 != 0.)
   double OmegaSS_Category2 = (1-category2_omtag-(Nagree_cat2/Ndt_cat2))/(1-2*category2_omtag);
 if (Ndt_cat2 != 0.)
   double ErrorOmegaSS_Category2 =  sqrt(((1.-2.*(Nagree_cat2/Ndt_cat2))*(1.-2.*(Nagree_cat2/Ndt_cat2))*
                                          (category1_omtag_err*category2_omtag_err)/((1.-2.*category2_omtag)*
                                                                                   (1.-2.*category2_omtag)*
                                                                                   (1.-2.*category2_omtag)*
                                                                                   (1.-2.*category2_omtag)))+
                                       (((Nagree_cat2/Ndt_cat2)*(1-(Nagree_cat2/Ndt_cat2)))/((1.-2.*category2_omtag)*
                                                                                             (1.-2.*category2_omtag)*Ndt_cat2)));
 if (Ndt_cat3 == 0.) double OmegaSS_Category3, ErrorOmegaSS_Category3 = 0.;
 if (Ndt_cat3 != 0.)
   double OmegaSS_Category3 = (1-category3_omtag-(Nagree_cat3/Ndt_cat3))/(1-2*category3_omtag);
 if (Ndt_cat3 != 0.)
   double ErrorOmegaSS_Category3 =  sqrt(((1.-2.*(Nagree_cat3/Ndt_cat3))*(1.-2.*(Nagree_cat3/Ndt_cat3))*
                                        (category3_omtag_err*category3_omtag_err)/((1.-2.*category3_omtag)*
                                                                                   (1.-2.*category3_omtag)*
                                                                                   (1.-2.*category3_omtag)*
                                                                                   (1.-2.*category3_omtag)))+
                                       (((Nagree_cat3/Ndt_cat3)*(1-(Nagree_cat3/Ndt_cat3)))/((1.-2.*category3_omtag)*
                                                                                             (1.-2.*category3_omtag)*Ndt_cat3)));
 if (Ndt_cat4 == 0.) double OmegaSS_Category4, ErrorOmegaSS_Category4 = 0.;
 if (Ndt_cat4 != 0.)
 double OmegaSS_Category4 = (1-category4_omtag-(Nagree_cat4/Ndt_cat4))/(1-2*category4_omtag);
 if (Ndt_cat4 != 0.)
 double ErrorOmegaSS_Category4 =  sqrt(((1.-2.*(Nagree_cat4/Ndt_cat4))*(1.-2.*(Nagree_cat4/Ndt_cat4))*
                                        (category4_omtag_err*category4_omtag_err)/((1.-2.*category4_omtag)*
                                                                                   (1.-2.*category4_omtag)*
                                                                                   (1.-2.*category4_omtag)*
                                                                                   (1.-2.*category4_omtag)))+
                                       (((Nagree_cat4/Ndt_cat4)*(1-(Nagree_cat4/Ndt_cat4)))/((1.-2.*category4_omtag)*
                                                                                             (1.-2.*category4_omtag)*Ndt_cat4)));
 if (Ndt_cat5 == 0.) double OmegaSS_Category5, ErrorOmegaSS_Category5 = 0.; 
 if (Ndt_cat5 != 0. )
   double OmegaSS_Category5 = (1-category5_omtag-(Nagree_cat5/Ndt_cat5))/(1-2*category5_omtag);
 if (Ndt_cat5 != 0.)
  double ErrorOmegaSS_Category5 =  sqrt(((1.-2.*(Nagree_cat5/Ndt_cat5))*(1.-2.*(Nagree_cat5/Ndt_cat5))*
                                         (category5_omtag_err*category5_omtag_err)/((1.-2.*category5_omtag)*
                                                                                    (1.-2.*category5_omtag)*
                                                                                    (1.-2.*category5_omtag)*
                                                                                    (1.-2.*category5_omtag)))+
                                        (((Nagree_cat5/Ndt_cat5)*(1-(Nagree_cat5/Ndt_cat5)))/((1.-2.*category5_omtag)*
                                                                                              (1.-2.*category5_omtag)*Ndt_cat5)));
 if (Ndt_cat6 == 0.) double OmegaSS_Category6, ErrorOmegaSS_Category6 = 0.;
 if (Ndt_cat6 != 0.)
   double OmegaSS_Category6 = (1-category6_omtag-(Nagree_cat6/Ndt_cat6))/(1-2*category6_omtag);
 if (Ndt_cat6 != 0.)
 double ErrorOmegaSS_Category6 = sqrt(((1.-2.*(Nagree_cat6/Ndt_cat6))*(1.-2.*(Nagree_cat6/Ndt_cat6))*
                                        (category6_omtag_err*category6_omtag_err)/((1.-2.*category6_omtag)*
                                                                                   (1.-2.*category6_omtag)*
                                                                                   (1.-2.*category6_omtag)*
                                                                                   (1.-2.*category6_omtag)))+
                                       (((Nagree_cat6/Ndt_cat6)*(1-(Nagree_cat6/Ndt_cat6)))/((1.-2.*category6_omtag)*
                                                                                           (1.-2.*category6_omtag)*Ndt_cat6)));


//#########################
//                                                     
//          STAGE 3       
//                        
//#########################
 
  cout<<" "<< endl;
  cout << "===== Now Printing OmegaSS(Wss)DT =====" << endl;
  cout<<" "<< endl;
  
  cout << "Wss_Category1 = " << OmegaSS_Category1 << " +/-" << ErrorOmegaSS_Category1<< endl  ;
  cout << "Wss_Category2 = " << OmegaSS_Category2 << " +/- " << ErrorOmegaSS_Category2 << endl  ;
  cout << "Wss_Category3 = " << OmegaSS_Category3 << " +/- " << ErrorOmegaSS_Category3 <<endl  ;
  cout << "Wss_Category4 = " << OmegaSS_Category4 << " +/- " << ErrorOmegaSS_Category4 <<endl  ;
  cout << "Wss_Category5 = " << OmegaSS_Category5 <<" +/- " << ErrorOmegaSS_Category5 << endl  ;
  cout << "Wss_Category6 = " << OmegaSS_Category6 <<" +/- " << ErrorOmegaSS_Category6 <<  endl  ;


  // now combining OmegaSS over All Categories
  
  if (ErrorOmegaSS_Category1 == 0 ) double z1=0., x1=0.;
  if (ErrorOmegaSS_Category1 != 0)
    double z1 = 1/(ErrorOmegaSS_Category1*ErrorOmegaSS_Category1);
  if (OmegaSS_Category1 != 0 && ErrorOmegaSS_Category1 != 0 )
    double x1= OmegaSS_Category1/(ErrorOmegaSS_Category1*ErrorOmegaSS_Category1);
  if (ErrorOmegaSS_Category2 == 0 || OmegaSS_Category2 == 0 ) double z2=0., x2=0.;
  if (ErrorOmegaSS_Category2 != 0)
    double z2 = 1/(ErrorOmegaSS_Category2*ErrorOmegaSS_Category2);  
  if (OmegaSS_Category2 != 0 && ErrorOmegaSS_Category2 != 0)
    double x2= OmegaSS_Category2/(ErrorOmegaSS_Category2*ErrorOmegaSS_Category2);
  if (ErrorOmegaSS_Category3 == 0) double z3=0., x3=0;
  if (ErrorOmegaSS_Category3 != 0)
    double z3 = 1/(ErrorOmegaSS_Category3*ErrorOmegaSS_Category3);
  if (OmegaSS_Category3 != 0  && ErrorOmegaSS_Category3 != 0)
    double x3= OmegaSS_Category3/(ErrorOmegaSS_Category3*ErrorOmegaSS_Category3);
  if (ErrorOmegaSS_Category4 == 0) double z4=0., x4=0.;
  if (ErrorOmegaSS_Category4 != 0)
    double z4 = 1/(ErrorOmegaSS_Category4*ErrorOmegaSS_Category4);
  if (OmegaSS_Category4 != 0 && ErrorOmegaSS_Category4 != 0 )
    double x4= OmegaSS_Category4/(ErrorOmegaSS_Category4*ErrorOmegaSS_Category4);
  if (ErrorOmegaSS_Category5 == 0 || OmegaSS_Category5 == 0 ) double z5=0.,  x5=0.;
  if (ErrorOmegaSS_Category5 != 0)
    double z5 = 1/(ErrorOmegaSS_Category5*ErrorOmegaSS_Category5);
  if (OmegaSS_Category5 != 0 && ErrorOmegaSS_Category5 != 0 )
    double x5= OmegaSS_Category5/(ErrorOmegaSS_Category5*ErrorOmegaSS_Category5);
  if (ErrorOmegaSS_Category6 == 0) double z6=0., x6=0.;
  if (ErrorOmegaSS_Category6 != 0)
    double z6 = 1/(ErrorOmegaSS_Category6*ErrorOmegaSS_Category6);
  if (OmegaSS_Category6 != 0 && ErrorOmegaSS_Category6 != 0 )
      double x6= OmegaSS_Category6/(ErrorOmegaSS_Category6*ErrorOmegaSS_Category6);
  double Combined_Wss = ((x1+x2+x3+x4+x5+x6)/(z1+z2+z3+z4+z5+z6));
  double Combined_WssgError = sqrt(1/(z1+z2+z3+z4+z5+z6));
  cout << "TDR Combined  Wss = " << Combined_Wss << " +/- " << Combined_WssgError << endl;
  cout<<" "<< endl;
  cout<<" ==================================================="<< endl;

//#########################
//                                                     
//          STAGE 4       
//                        
//#########################
  c1->Close();
  TFile f1("~/DC06_test/jobs/ganga/DC06/addTuples/numbers/DC06/bs2dsmunu/v31/New_bsmuData_v19r9v31.root");   
//  TFile f1("TaggingTuplesData.root"); //  Data
  TTree* t1=(TTree*)f1.FindObjectAny("200");
  TH1F *PlotTotal = new TH1F("PlotTotal", "PlotNdt", 100, 1940, 2190);
  t1->Draw("DsMass>>PlotTotal", Total_cut && Global_Bs2DsMuNu_bkg_cut);
  TH1F *PlotNdt = new TH1F("PlotNdt", "PlotNdt", 100, 1920, 2190);
  t1->Draw("DsMass>>PlotNdt", Ndt_cut && Global_Bs2DsMuNu_bkg_cut);
  TH1F *PlotNagree = new TH1F("PlotNagree", "PlotNagree", 100, 1920, 2190);
  t1->Draw("DsMass>>PlotNagree", Nagree_cut && Global_Bs2DsMuNu_bkg_cut);
  TH1F *PlotNdisagree = new TH1F("PlotNdisagree", "PlotNdisagree", 100, 1920, 2190);
  t1->Draw("DsMass>>PlotNdisagree", Ndisagree_cut && Global_Bs2DsMuNu_bkg_cut);
   f.Close();/// close first root file 
  TCanvas*  Histograms = new TCanvas("Histograms");
  Histograms ->Divide(2,2);
  Histograms->cd(1);
  PlotTotal->Draw();
  Histograms->cd(2);
  PlotNdt->Draw();
  Histograms->cd(3);
  PlotNagree->Draw();
  Histograms->cd(4);
  PlotNdisagree->Draw();
  Histograms->Update();
  Histograms.Print("Histograms2.eps");

//#########################
//                                                     
//          STAGE 5       
//                        
//#########################

 /// Now to carry out Fit using RooFit

  int seed=123987;
  gSystem->Load("libRooFit");
  using namespace RooFit;

  TRandom* generator = RooRandom::randomGenerator();
  generator->SetSeed(seed);

  double mDsPDG = 1970.;
  
  // Measureable
   RooRealVar m("m","m",1940,2170,"MeV/c^{2}");

  // Fit variables
  RooRealVar mDs("m(D_{s})","m(D_{s})",mDsPDG, mDsPDG-20.,mDsPDG+20.,
                 "MeV/c^{2}");
  RooRealVar sigmaDs("#sigma(D_{s})","#sigma(D_{s})",16,5,20,"MeV/c^{2}");
  RooRealVar nSig("n_{sig}","n_{sig}",200,10,1000);
  RooRealVar nBkg("n_{bkg}","n_{bkg}",200,10,1000);
  RooRealVar decay("decay","decay",-0.003,-0.010,-0.001);

  RooGaussian signal("signal","signal",m,mDs,sigmaDs);
  RooExponential bkg("bkg","background",m,decay);
  RooAddPdf massPdf("massPDF","massPdf",RooArgList(signal,bkg),
                    RooArgList(nSig,nBkg));
  
   gROOT->cd();
 
// Create a nice plot
  TCanvas*  Fit = new TCanvas(" Fit");
  Fit->Divide(2,2);
  Fit->cd(1);

 RooDataHist data("data","data",RooArgList(m),PlotTotal);
  
  // Fitting to datasets

  massPdf.fitTo(data,Extended(true));
  RooPlot* frame=m.frame(Title("Fit Results for Total candidates"));
  data.plotOn(frame);
  data.statOn(frame, Layout(0.50,0.90,0.60));	
  massPdf.plotOn(frame);
  massPdf.paramOn(frame,Layout(0.50,0.90,0.90));
  frame->Draw();
  double FitTotalSignal=nSig.getVal();
  double FitTotalSignalError=nSig.getError();
  double FitTotalBkg=nBkg.getVal();
  double FitTotalBkgError=(nBkg.getAsymErrorHi()+ abs(nBkg.getAsymErrorLo()))/2;
  
  ///keep width and DsMass constant
  sigmaDs.setConstant(); 
  mDs.setConstant();  
 
  Fit->cd(2);  
  RooDataHist data("data","data",RooArgList(m),PlotNdt);
  massPdf.fitTo(data,Extended(true));
  RooPlot* frame=m.frame(Title("Fit Results for Total DT candidates"));
  data.plotOn(frame);
  data.statOn(frame, Layout(0.50,0.90,0.71));	
  massPdf.plotOn(frame);
  massPdf.paramOn(frame,Layout(0.50,0.90,0.90));
  frame->Draw();
  double FitNdtSignal=nSig.getVal();
  double FitNdtSignalError=nSig.getError();
  double FitNdtBkg=nBkg.getVal();
  double FitNdtBkgError=(nBkg.getAsymErrorHi()+abs(nBkg.getAsymErrorLo()))/2;
 
  Fit->cd(3);
  RooDataHist data("data","data",RooArgList(m),PlotNagree);
  massPdf.fitTo(data,Extended(true));
  RooPlot* frame=m.frame(Title("Fit Results for DT candidates that Agree"));
  data.plotOn(frame);
  data.statOn(frame, Layout(0.50,0.90,0.71));	
  massPdf.plotOn(frame);
  massPdf.paramOn(frame,Layout(0.50,0.90,0.90));
  frame->Draw();
  double FitNagreeSignal=nSig.getVal();
  double FitNagreeSignalError=nSig.getError();
  double FitNagreeBkg=nBkg.getVal();
  double FitNagreeBkgError=(nBkg.getAsymErrorHi()+ abs(nBkg.getAsymErrorLo()))/2;
  
  Fit->cd(4);  
  RooDataHist data("data","data",RooArgList(m),PlotNdisagree);
  massPdf.fitTo(data,Extended(true));
  RooPlot* frame=m.frame(Title("Fit Results for DT candidates that Disagree"));
  data.plotOn(frame);
  data.statOn(frame, Layout(0.50,0.90,0.71));	
  massPdf.plotOn(frame);
  massPdf.paramOn(frame,Layout(0.50,0.90,0.90));
  frame->Draw();
  double FitNdisagreeSignal=nSig.getVal();
  double FitNdisagreeSignalError=nSig.getError();
  double FitNdisagreeBkg=nBkg.getVal();
  double FitNdisagreeBkgError=(nBkg.getAsymErrorHi()+ abs(nBkg.getAsymErrorLo()))/2;
  Fit->Update();
  Fit.Print("Fit2.eps");

  cout<<"====================================== "<< endl;
  cout<<"Now fitting model to data ....plots: data is [Histograms.eps], fit is [Fit.eps]"<< endl;
  cout << " Now printing results of Fit to Data ....." << endl;
  cout<<" "<< endl;
  cout<<"SIGNAL "<< endl;
  cout << "Total = " << FitTotalSignal << " +/- " << FitTotalSignalError << endl;
  cout << "Ndt = " << FitNdtSignal << " +/- " << FitNdtSignalError << endl;
  cout << "Nagree = " << FitNagreeSignal << " +/- " << FitNagreeSignalError << endl;
  cout << "Ndisagree = " << FitNdisagreeSignal << " +/- " << FitNdisagreeSignalError << endl;
  cout<<"BACKGROUND "<< endl;
  cout << "Total = " << FitTotalBkg << " +/- " << FitTotalBkgError << endl;
  cout << "Ndt = " << FitNdtBkg << " +/- " << FitNdtBkgError << endl;
  cout << "Nagree = " << FitNagreeBkg << " +/- " << FitNagreeBkgError << endl;
  cout << "Ndisagree = " << FitNdisagreeBkg << " +/- " << FitNdisagreeBkgError << endl;

  double FSignal = FitNagreeSignal/FitNdtSignal;
  double FSignalError = sqrt((FitNagreeSignalError*FitNagreeSignalError*(1-FSignal)*(1-FSignal)+
                              (FitNdisagreeSignalError*FitNdisagreeSignalError*FSignal*FSignal))
                             /(FitNdtSignal*FitNdtSignal));
  
  cout << "F_signal = " <<FSignal<< " +/- " << FSignalError << endl;



//#########################
//                                                     
//          STAGE 6       
//                        
//#########################

  gROOT->cd();///change directory
  cout<<" "<< endl;
  cout << "Now Carrying Out Projections of Sigma-Wss/Wss....." << endl;

  cout << "  Important!!! Make sure Sigma-Wos is varied with Integrated Luminosity!  " << endl;
  cout << "  Insert OmegaOS_Proj & EpsilonOS_Proj and Yeild factor by hand "<< endl;
  cout << "  Output is Projection.eps" << endl;
  
    
  int len=100;
  ///double events[len];
  double events[len], performance[len],YeildAtTwoInversefb[len];
  double Ndt[len], Na[len],Nd[len],YeildFactor[len], errorOS[len];
  double constError2[len], scaleError2a[len], scaleError2b[len], errorSS[len];
  double epsilonSS = 0.;
  double omegaSS = 0.; //from DT ;
  double omegaOS_Proj = 0.;
  double epsilonOS_Proj = 0.;
  for (int i=0; i<len; i++) {
    events[i] = 2.5/len*i+0.001;
    //YeildAtTwoInversefb[i] = (140000./2.)*events[i];  /// Put in estimated Yeild here. Divide by 2 to normalise to 2fb-1 For Bs2DsPi.
    YeildAtTwoInversefb[i] = (1060000./2.)*events[i]; ///for Bs2DsMuNu

  double errorScalingFactor = 0.0;
  errorScalingFactor[i] = sqrt(FitTotalSignal*FitTotalSignal/(YeildAtTwoInversefb[i]*YeildAtTwoInversefb[i]));

  Ndt[i] = YeildAtTwoInversefb[i]*eftag*sskaon_eftag; /// Projected Ndt
  Na[i] = (FSignal+(errorScalingFactor[i]*FSignalError))*Ndt[i]; /// Projected Nagree
  Nd[i] = (1.-(FSignal+(errorScalingFactor[i]*FSignalError)))*Ndt[i]; /// Projected Ndisagree
 
  /// IMPORTANT!!!!!
  /// estimating error(W_os) as it varies with Integrated luminosity
  /// using data (w_os, e_os) from B+-> dbar0Pi lhcb-2006-058

  epsilonSS = sskaon_eftag;///from MC
  omegaSS = Combined_Wss; ///from DT ;
  omegaOS_Proj = 0.368; /// from LHCb Note 2006-058, Table 7, pg 9
  epsilonOS_Proj = 0.471; /// from LHCb Note 2006-058, Table 7, pg 9
  //YeildFactor[i] =  (1./0.14)*YeildAtTwoInversefb[i]; //bs2dspi
  YeildFactor[i] =  (2.4/1.06)*YeildAtTwoInversefb[i]; //bs2dsmunu
  errorOS[i] = sqrt((omegaOS_Proj*(1.-omegaOS_Proj))/(epsilonOS_Proj*YeildFactor[i])); ///scaled error on Wos

   constError2[i] = (1.-2.*FSignal)*(1.-2.*FSignal)*errorOS[i]*errorOS[i]/
    ((1.-2.*eftag)*(1.-2.*eftag)*(1.-2.*eftag)*(1.-2.*eftag));

   scaleError2a[i] = 1./((1.-2.*eftag)*(1.-2.*eftag)*(Ndt[i]*Ndt[i]));
   scaleError2b[i] = ((Na[i]*(1.-FSignal)*(1.-FSignal))+(Nd[i]*FSignal*FSignal));
   errorSS[i] = sqrt( constError2[i] + (scaleError2a[i]*scaleError2b[i]));

  performance[i] = errorSS[i]/Combined_Wss;
 }

  TCanvas* Projection = new TCanvas("Projection");
  
  TH2F* hist = new TH2F(" ","(A)  Projected #sigma(#omega_{SS})/#omega_{SS} vs. Reconstructed B_{s}#rightarrow X 
  Double Tagged events",20,0,2.501,20,0.01,0.5);
  hist->SetStats(kFALSE);
  hist->SetXTitle("Integrated Luminosity (fb^{-1})");
  hist->SetYTitle("#sigma(#omega_{SS})/#omega_{SS}");  
  hist->Draw();
  TGraph* grError = new TGraph(len,events,performance);
  grError->SetLineColor(kRed);
  grError->Draw();
  Projection-> Update();
  Projection.Print("Projection2.eps"); 
  gROOT->Reset();

}
