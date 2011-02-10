//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
taginit(); //read option file for global variables

//NB: you can also book and fill histograms inside tools!

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
TString rootsubdir = ""; 
TFile fhisto("output/tag.root","RECREATE"); 
if(rootsubdir != "") { fhisto.mkdir(rootsubdir); fhisto.cd(rootsubdir); } 

//>>>>>>>>>>>>>>>>>>>>>>>>//create  histograms
float x1 = 0.5, x2 = 3;
TString direction="left2right";

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
TH1F* h1    = new TH1F("h1","BSmass ",100, 4, 6);
TH1F* h2    = new TH1F("h2","nr of primary vtx" ,10, 0., 10);
TH1F* h3    = new TH1F("h3","Pt B Opposite",50, 0, 100.);
TH1F* h4    = new TH1F("h4","track multiplicity",50, 0, 100.);

//Omega for taggers and combination
TH1F* hom_combOS = new TH1F("hom_combOS","hom_combOS", 50, 0, 0.6);
TH1F* hom_combOSSS = new TH1F("hom_combOSSS","hom_combOSSS", 50, 0, 0.6);
TH1F* hom_e = new TH1F("hom_e","hom_e", 50, 0, 0.6);
TH1F* hom_mu = new TH1F("hom_mu","hom_mu", 50, 0, 0.6);
TH1F* hom_k = new TH1F("hom_k","hom_k", 50, 0, 0.6);
TH1F* hom_ss = new TH1F("hom_ss","hom_ss", 50, 0, 0.6);
TH1F* hom_vtx = new TH1F("hom_vtx","hom_vtx", 50, 0, 0.6);

//B pt
TH1F* h6    = new TH1F("h6","B pT ",100, 0.0, 30.);
TH1F* h6rSS = new TH1F("h6rSS","B pT ",100, 0.0, 30.);
TH1F* h6wSS = new TH1F("h6wSS","B pT ",100, 0.0, 30.);
TH1F* h6r   = new TH1F("h6r","B pT ",100, 0.0, 30.);
TH1F* h6w   = new TH1F("h6w","B pT ",100, 0.0, 30.);
TH1F* h6rtis= new TH1F("h6rtis","B pT tis",100, 0.0, 30.);
TH1F* h6wtis= new TH1F("h6wtis","B pT tis",100, 0.0, 30.);
TH1F* h6rtos= new TH1F("h6rtos","B pT tos",100, 0.0, 30.);
TH1F* h6wtos= new TH1F("h6wtos","B pT tos",100, 0.0, 30.);
TH1F* h6rtistos = new TH1F("h6rtistos","B pT tistos",100, 0.0, 30.);
TH1F* h6wtistos = new TH1F("h6wtistos","B pT tistos",100, 0.0, 30.);

//NN
TH1F* h513  = new TH1F("h513","NNmu ", 50, 0.2, 1.);
TH1F* h613  = new TH1F("h613","NNmu ", 50, 0.2, 1.);
TH1F* h513p = new TH1F("h513p","NNmu ", 50, 0, .7);
TH1F* h613p = new TH1F("h613p","NNmu ", 50, 0, .7);
TH1F* h511  = new TH1F("h511","NNe  ", 50, 0.1, 1.);
TH1F* h611  = new TH1F("h611","NNe  ", 50, 0.1, 1.);
TH1F* h511p = new TH1F("h511p","NNe  ", 50, 0, .7);
TH1F* h611p = new TH1F("h611p","NNe  ", 50, 0, .7);
TH1F* h5321 = new TH1F("h5321","NNk ", 50, .1, 1.);
TH1F* h6321 = new TH1F("h6321","NNk ", 50, .1, 1.);
TH1F* h5321p= new TH1F("h5321p","NNk ", 50, 0, .7);
TH1F* h6321p= new TH1F("h6321p","NNk ", 50, 0, .7);
TH1F* h535  = new TH1F("h535","NNks/pi",50, .5, 1.);
TH1F* h635  = new TH1F("h635","NNks/pi",50, .5, 1.);
TH1F* h535p = new TH1F("h535p","NNks/pi",50, 0, 0.7);
TH1F* h635p = new TH1F("h635p","NNks/pi",50, 0, 0.7);
TH1F* h540  = new TH1F("h540","vert charge ", 90, .1, .9);
TH1F* h640  = new TH1F("h640","vert charge ", 90, .1, .9);
TH1F* h540p = new TH1F("h540p","vert charge ", 70, 0, .7);
TH1F* h640p = new TH1F("h640p","vert charge ", 70, 0, .7);
TH1F* h100  = new TH1F("h100","PNetSum ", 60, 0.4, 1.);
TH1F* h102  = new TH1F("h102","PNetSum ", 60, 0.4, 1.);
TH1F* h103  = new TH1F("h103","1-PNetSum",60, 0.0, 0.6);
TH1F* h104  = new TH1F("h104","1-PNetSum",60, 0.0, 0.6);

//DLL - PID
TH1F* h1000 = new TH1F("h1000","de  ", 50, -25.0, 25.);
TH1F* h1001 = new TH1F("h1001","dmu ", 50, -30.0, 30.);
TH1F* h1002 = new TH1F("h1002","dk  ", 50, -20.0, 80.);
TH1F* h1003 = new TH1F("h1003","dkS  ", 50,-20.0, 80.);
TH1F* h1004 = new TH1F("h1004","dkS-dp",50,-20.0, 80.);
TH1F* h1005 = new TH1F("h1005","dk-dp", 50,-20.0, 80.);
TH1F* h1010 = new TH1F("h1010","de  ", 50, -25.0, 25.);
TH1F* h1011 = new TH1F("h1011","dmu ", 50, -30.0, 30.);
TH1F* h1012 = new TH1F("h1012","dk  ", 50, -20.0, 80.);
TH1F* h1013 = new TH1F("h1013","dkS ", 50, -20.0, 80.);
TH1F* h1014 = new TH1F("h1014","dkS-dp",50,-20.0, 80.);
TH1F* h1015 = new TH1F("h1015","dk-dp", 50,-20.0, 80.);
TH1F* h1020 = new TH1F("h1020","de  ", 50, -25.0, 25.);
TH1F* h1021 = new TH1F("h1021","dmu ", 50, -30.0, 30.);
TH1F* h1022 = new TH1F("h1022","dk  ", 50, -20.0, 80.);
TH1F* h1023 = new TH1F("h1023","dkS ", 50, -20.0, 80.);
TH1F* h1024 = new TH1F("h1024","dkS-dp",50,-20.0, 80.);
TH1F* h1025 = new TH1F("h1025","dk-dp", 50,-20.0, 80.);

//Osc vs noOsc
TH1F* h2011   = new TH1F("h2011","Mu tag B time (ps) no osc.", 1000,0,10);
TH1F* h2012   = new TH1F("h2012","Mu tag B time (ps) osc.",    1000,0,10);
TH1F* h2021   = new TH1F("h2021","Ele tag B time (ps) no osc.",1000,0,10);
TH1F* h2022   = new TH1F("h2022","Ele tag B time (ps) osc.",   1000,0,10);
TH1F* h2031   = new TH1F("h2031","KO tag B time (ps) no osc.", 1000,0,10);
TH1F* h2032   = new TH1F("h2032","KO tag B time (ps) osc.",    1000,0,10);
TH1F* h2041   = new TH1F("h2041","SS tag B time (ps) no osc.", 1000,0,10);
TH1F* h2042   = new TH1F("h2042","SS tag B time (ps) osc.",    1000,0,10);
TH1F* h2051   = new TH1F("h2051","Vtx tag B time (ps) no osc.",1000,0,10);
TH1F* h2052   = new TH1F("h2052","Vtx tag B time (ps) osc.",   1000,0,10);
TH1F* h3011   = new TH1F("h3011","cat1 tag B time (ps) no osc.", 1000,0,10);
TH1F* h3012   = new TH1F("h3012","cat1 tag B time (ps) osc.",    1000,0,10);
TH1F* h3021   = new TH1F("h3021","cat2 tag B time (ps) no osc.", 1000,0,10);
TH1F* h3022   = new TH1F("h3022","cat2 tag B time (ps) osc.",    1000,0,10);
TH1F* h3031   = new TH1F("h3031","cat3 tag B time (ps) no osc.", 1000,0,10);
TH1F* h3032   = new TH1F("h3032","cat3 tag B time (ps) osc.",    1000,0,10);
TH1F* h3041   = new TH1F("h3041","cat4 tag B time (ps) no osc.", 1000,0,10);
TH1F* h3042   = new TH1F("h3042","cat4 tag B time (ps) osc.",    1000,0,10);
TH1F* h3051   = new TH1F("h3051","cat5 tag B time (ps) no osc.", 1000,0,10);
TH1F* h3052   = new TH1F("h3052","cat5 tag B time (ps) osc.",    1000,0,10);
TH1F* h3061   = new TH1F("h3061","ALL tag B time (ps) no osc.",  1000,0,10);
TH1F* h3062   = new TH1F("h3062","ALL tag B time (ps) osc.",     1000,0,10);
TH1F* h4061   = new TH1F("h4061","IDEAL tag B time (ps) no osc.",1000,0,10);
TH1F* h4062   = new TH1F("h4062","IDEAL tag B time (ps) osc.",   1000,0,10);

//Multiple PileUp vertices studies
TH1F* hpv_gippu_r = new TH1F("hpv_gippu_r","gippu tags", 50, 0.0, 50);
TH1F* hpv_gippu_w = new TH1F("hpv_gippu_w","gippu tags", 50, 0.0, 50);
TH1F* hpv_difip_r = new TH1F("hpv_difip_r","IP-min(IPPU) tags", 50, -15.0, 5);
TH1F* hpv_difip_w = new TH1F("hpv_difip_w","IP-min(IPPU) tags", 50, -15.0, 5);
TH1F* hpv_ipmeandif_r = new TH1F("hpv_ipmeandif_r","IP-<IPPU> tags", 50, -15.0, 5);
TH1F* hpv_ipmeandif_w = new TH1F("hpv_ipmeandif_w","IP-<IPPU> tags", 50, -15.0, 5);
TH1F* hpv_zposdif_r = new TH1F("hpv_zposdif_r","Z pos dif",50, -25., 25.);
TH1F* hpv_zposdif_w = new TH1F("hpv_zposdif_w","Z pos dif",50, -25., 25.);
TH1F* hpv_zposabsdif_r = new TH1F("hpv_zposabsdif_r","abs(Z(PU) - Z(tagtrack)) tags",50, 0., 25.);
TH1F* hpv_zposabsdif_w = new TH1F("hpv_zposabsdif_w","abs(Z(PU) - Z(tagtrack)) tags",50, 0., 25.);

//Secondary Vertices study (g/b refers to mc) (r/w refers to right/wrong tags)
TH1F* hsv100 = new TH1F("hsv100","hsv100", 5, 0, 5);//kaons
TH1F* hsv100_r = new TH1F("hsv100_r","hsv100_r", 5, 0, 5);
TH1F* hsv100_w = new TH1F("hsv100_w","hsv100_w", 5, 0, 5);
TH1F* hsv101 = new TH1F("hsv101","hsv101", 5, 0, 5);//leptons
TH1F* hsv101_r = new TH1F("hsv101_r","hsv101_r", 5, 0, 5);
TH1F* hsv101_w = new TH1F("hsv101_w","hsv101_w", 5, 0, 5);
TH1F* hsv102 = new TH1F("hsv102","hsv102", 5, 0, 5);//leptons or kaons
TH1F* hsv102_r = new TH1F("hsv102_r","hsv102_r", 5, 0, 5);
TH1F* hsv102_w = new TH1F("hsv102_w","hsv102_w", 5, 0, 5);
TH1F* hsv9 = new TH1F("hsv9","hsv9", 50, 0, 20);//distance SV-BO
TH1F* hsv200 = new TH1F("hsv200","hsv200", 50, 0, 3);//deltaPhi
TH1F* hsv200_g = new TH1F("hsv200_g","hsv200_g", 50, 0, 3);
TH1F* hsv200_b = new TH1F("hsv200_b","hsv200_b", 50, 0, 3);
TH1F* hsv200_r = new TH1F("hsv200_r","hsv200_r", 50, 0, 3);
TH1F* hsv200_w = new TH1F("hsv200_w","hsv200_w", 50, 0, 3);
TH1F* hsv201 = new TH1F("hsv201","hsv201", 50, -0.3, 0.3);//deltaTheta
TH1F* hsv201_g = new TH1F("hsv201_g","hsv201_g", 50, -0.3, .3);
TH1F* hsv201_b = new TH1F("hsv201_b","hsv201_b", 50, -0.3, .3);
TH1F* hsv201_r = new TH1F("hsv201_r","hsv201_r", 50, -0.3, 0.3);
TH1F* hsv201_w = new TH1F("hsv201_w","hsv201_w", 50, -0.3, 0.3);
TH1F* hsv202_g = new TH1F("hsv202_g","hsv202_g", 50, 0, 3);//delatPhiVect
TH1F* hsv202_b = new TH1F("hsv202_b","hsv202_b", 50, 0, 3);
TH1F* hsv300 = new TH1F("hsv300","hsv300", 50, 0, 10);//tau
TH1F* hsv300_r = new TH1F("hsv300_r","hsv300_r", 50, 0, 10);
TH1F* hsv300_w = new TH1F("hsv300_w","hsv300_w", 50, 0, 10);
TH1F* hsv301 = new TH1F("hsv301","hsv301", 50, -300, 300);//BMCP-SVGP
TH1F* hsv301_g = new TH1F("hsv301_g","hsv301_g", 50, -300, 300);
TH1F* hsv302 = new TH1F("hsv302","hsv302", 50, -10, 10);//BMCtau-SVtau
TH1F* hsv302_g = new TH1F("hsv302_g","hsv302_g", 50, -10, 10);
TH1F* hsv303 = new TH1F("hsv303","hsv303", 50, -50, 50);//BMCsz-sz
TH1F* hsv303_g = new TH1F("hsv303_g","hsv303_g", 50, -50, 50);
TH2F* hsv310_b = new TH2F("hsv310_b","hsv310_b", 50, 0., 100, 50, 0, 200);//(SVP,BMCP)
TH2F* hsv310_g = new TH2F("hsv310_g","hsv310_g", 50, 0., 100, 50, 0, 200);
TH1F* hsv311_b = new TH1F("hsv311_b","hsv311_b", 50, 0, 2);//SVP/BMCP
TH1F* hsv311_g = new TH1F("hsv311_g","hsv311_g", 50, 0, 2);
TH2F* hsv312_b = new TH2F("hsv312_b","hsv312_b", 50, 0., 5, 50, 0, 2);//(SVM, SVP/BMCP)
TH2F* hsv312_g = new TH2F("hsv312_g","hsv312_g", 50, 0., 5, 50, 0, 2);
TH1F* hsv313_b = new TH1F("hsv313_b","hsv313_b", 50, 0, 2);//SVM/BMCM
TH1F* hsv313_g = new TH1F("hsv313_g","hsv313_g", 50, 0, 2);
TH1F* hsv320_r = new TH1F("hsv320_r","hsv320_r", 50, 0, 7);
TH1F* hsv320_w = new TH1F("hsv320_w","hsv320_w", 50, 0, 7);
TH1F* hsv321_r = new TH1F("hsv321_r","hsv321_r", 50, 0, 7);//tau for Bs(531)
TH1F* hsv321_w = new TH1F("hsv321_w","hsv321_w", 50, 0, 7);
TH1F* hsv322_r = new TH1F("hsv322_r","hsv322_r", 50, 0, 7);//tau for B0(511)
TH1F* hsv322_w = new TH1F("hsv322_w","hsv322_w", 50, 0, 7);
TH1F* hsv323_r = new TH1F("hsv323_r","hsv323_r", 50, 0, 7);//tau for B+-(521)
TH1F* hsv323_w = new TH1F("hsv323_w","hsv323_w", 50, 0, 7);
//SV vs PU
TH1F* hsv401 = new TH1F("hsv401","hsv401", 50, -5, 75);//BoppDir.z
TH1F* hsv401_r = new TH1F("hsv401_r","hsv401_r", 50, -5, 75);
TH1F* hsv401_w = new TH1F("hsv401_w","hsv401_w", 50, -5, 75);
TH1F* hsv421 = new TH1F("hsv421","hsv421", 50, -5, 75);//BoppDir.Mag
TH1F* hsv402 = new TH1F("hsv402","hsv402", 50, -75, 75);//z dif SV-PU
TH1F* hsv402_r = new TH1F("hsv402_r","hsv402_r", 50, -75, 75);
TH1F* hsv402_w = new TH1F("hsv402_w","hsv402_w", 50, -75, 75);
TH1F* hsv422 = new TH1F("hsv422","hsv422", 50, -1, 150);//mag dif SV-PU
TH1F* hsv403 = new TH1F("hsv403","hsv403", 50, -75, 75);//SV zpos closer to PU or to PV (PU-PV)
TH1F* hsv403_r = new TH1F("hsv403_r","hsv403_r", 50, -75, 75);
TH1F* hsv403_w = new TH1F("hsv403_w","hsv403_w", 50, -75, 75);
TH1F* hsv423 = new TH1F("hsv423","hsv423", 50, -75, 75);//SV mag closer to PU or to PV (PU-PV)
TH2F* hsv404_r = new TH2F("hsv404_r","hsv404_r", 50, -2.5, 2.5, 50, -2.5, 2.5);//x,y pos of SV
TH2F* hsv404_w = new TH2F("hsv404_w","hsv404_w", 50, -2.5, 2.5, 50, -2.5, 2.5);
TH1F* hsv405 = new TH1F("hsv405","hsv405", 100, 0, 5);
TH1F* hsv405_r = new TH1F("hsv405_r","hsv405_r", 100, 0, 5);
TH1F* hsv405_w = new TH1F("hsv405_w","hsv405_w", 100, 0, 5);
TH1F* hsv500 = new TH1F("hsv500","hsv500", 4, 0, 4);//PU counter
TH1F* hsv500_r = new TH1F("hsv500_r","hsv500_r", 4, 0, 4);
TH1F* hsv500_w = new TH1F("hsv500_w","hsv500_w", 4, 0, 4);
TH1F* hsv501 = new TH1F("hsv501","hsv501", 49, 1, 50);//tracsk in PU
TH1F* hsv501_r = new TH1F("hsv501_r","hsv501_r", 49, 1, 50);
TH1F* hsv501_w = new TH1F("hsv501_w","hsv501_w", 49, 1, 50);
TH1F* hsv502 = new TH1F("hsv502","hsv502", 50, -100, 100);//PU z position
TH1F* hsv502_r = new TH1F("hsv502_r","hsv502_r", 50, -100, 100);
TH1F* hsv502_w = new TH1F("hsv502_w","hsv502_w", 50, -100, 100);
TH1F* hsv522 = new TH1F("hsv522","hsv522", 50, -1, 25);//PU mag
TH1F* hsv503 = new TH1F("hsv503","hsv503", 50, 0, 1);//activity PU (%tracks in PU)
TH1F* hsv503_r = new TH1F("hsv503_r","hsv503_r", 50, 0, 1);
TH1F* hsv503_w = new TH1F("hsv503_w","hsv503_w", 50, 0, 1);


//Taggers

TH1F* h70 = new TH1F("h70","ko pt",50, 0., 10.); //ALL
TH1F* h71 = new TH1F("h71","ko p" ,50, 0., 100.);
TH1F* h72 = new TH1F("h72","ko ip/s" ,50, -50., 50.);
TH1F* h73 = new TH1F("h73","ko LCS" ,50, 0., 5.);
TH1F* h74 = new TH1F("h74","ko ghost prob" ,50, -50., 0.);
TH1F* h75 = new TH1F("h75","ko PIDk" ,50, -40., 100.);
TH1F* h76 = new TH1F("h76","ko PIDkp" ,50, -10., 50.);
TH1F* h77 = new TH1F("h77","ko distphi" ,50, 0., .2);
 
TH1F* h80 = new TH1F("h80","ko pt",50, 0., 10.); //right
TH1F* h81 = new TH1F("h81","ko p" ,50, 0., 100.);
TH1F* h82 = new TH1F("h82","ko ip/s" ,50, -30., 30.);
TH1F* h83 = new TH1F("h83","ko LCS" ,50, 0., 5.);
TH1F* h84 = new TH1F("h84","ko ghost prob" ,50, -50., 0.);
TH1F* h85 = new TH1F("h85","ko PIDk" ,50, -40., 100.);
TH1F* h86 = new TH1F("h86","ko PIDkp" ,50, -10., 50.);
TH1F* h87 = new TH1F("h87","ko distphi" ,50, 0., .2);

TH1F* h80_3 = new TH1F("h80_3","ko pt",50, 0., 10.); //right nu=3
TH1F* h81_3 = new TH1F("h81_3","ko p" ,50, 0., 100.);
TH1F* h82_3 = new TH1F("h82_3","ko ip/s" ,50, -30., 30.);
TH1F* h83_3 = new TH1F("h83_3","ko LCS" ,50, 0., 5.);
TH1F* h84_3 = new TH1F("h84_3","ko ghost prob" ,50, -50., 0.);
TH1F* h85_3 = new TH1F("h85_3","ko PIDk" ,50, -40., 100.);
TH1F* h86_3 = new TH1F("h86_3","ko PIDkp" ,50, -10., 50.);
TH1F* h87_3 = new TH1F("h87_3","ko distphi" ,50, 0., .2);

//MUON
x1=0; x2=5  ; //pt
TH1F* hr_mu_pt = new TH1F("hr_mu_pt","pt right tags", 50, x1, x2);
TH1F* hw_mu_pt = new TH1F("hw_mu_pt","pt wrong tags", 50, x1, x2);
x1=0; x2=70 ; //p
TH1F* hr_mu_p = new TH1F("hr_mu_p","p right tags", 50, x1, x2);
TH1F* hw_mu_p = new TH1F("hw_mu_p","p wrong tags", 50, x1, x2);
x1=0; x2=50  ; //ip/s
TH1F* hr_mu_ips = new TH1F("hr_mu_ips","ips right tags", 50, x1, x2);
TH1F* hw_mu_ips = new TH1F("hw_mu_ips","ips wrong tags", 50, x1, x2);
x1=0; x2=100  ; //ippu
TH1F* hr_mu_ippu = new TH1F("hr_mu_ippu","ippu right tags", 50, x1, x2);
TH1F* hw_mu_ippu = new TH1F("hw_mu_ippu","ippu wrong tags", 50, x1, x2);
x1=0; x2=5   ; //LCS
TH1F* hr_mu_lcs = new TH1F("hr_mu_lcs","lcs right tags", 50, x1, x2);
TH1F* hw_mu_lcs = new TH1F("hw_mu_lcs","lcs wrong tags", 50, x1, x2);
x1=-5, x2=20; // PID
TH1F* hr_mu_pid = new TH1F("hr_mu_pid","pid right tags", 50, x1, x2);
TH1F* hw_mu_pid = new TH1F("hw_mu_pid","pid wrong tags", 50, x1, x2);
x1=-35; x2=0 ;// tsal ghost likel
TH1F* hr_mu_tsal = new TH1F("hr_mu_tsal","tsal right tags", 50, x1, x2);
TH1F* hw_mu_tsal = new TH1F("hw_mu_tsal","tsal wrong tags", 50, x1, x2);
x1=0; x2=100;//N mult
TH1F* hr_mu_mult = new TH1F("hr_mu_mult","mult right tags", 50, x1, x2);
TH1F* hw_mu_mult = new TH1F("hw_mu_mult","mult wrong tags", 50, x1, x2);

//ELECTRON
x1=0; x2=5  ; //pt
TH1F* hr_ele_pt = new TH1F("hr_ele_pt","pt right tags", 50, x1, x2);
TH1F* hw_ele_pt = new TH1F("hw_ele_pt","pt wrong tags", 50, x1, x2);
x1=0; x2=70 ; //p
TH1F* hr_ele_p = new TH1F("hr_ele_p","p right tags", 50, x1, x2);
TH1F* hw_ele_p = new TH1F("hw_ele_p","p wrong tags", 50, x1, x2);
x1=0; x2=50  ; //ip/s
TH1F* hr_ele_ips = new TH1F("hr_ele_ips","ips right tags", 50, x1, x2);
TH1F* hw_ele_ips = new TH1F("hw_ele_ips","ips wrong tags", 50, x1, x2);
x1=0; x2=100  ; //ippu
TH1F* hr_ele_ippu = new TH1F("hr_ele_ippu","ippu right tags", 50, x1, x2);
TH1F* hw_ele_ippu = new TH1F("hw_ele_ippu","ippu wrong tags", 50, x1, x2);
x1=0; x2=5   ; //LCS
TH1F* hr_ele_lcs = new TH1F("hr_ele_lcs","lcs right tags", 50, x1, x2);
TH1F* hw_ele_lcs = new TH1F("hw_ele_lcs","lcs wrong tags", 50, x1, x2);
x1=-5, x2=20; // PID
TH1F* hr_ele_pid = new TH1F("hr_ele_pid","pid right tags", 50, x1, x2);
TH1F* hw_ele_pid = new TH1F("hw_ele_pid","pid wrong tags", 50, x1, x2);
x1=-35; x2=0 ;// tsal ghost likel
TH1F* hr_ele_tsal = new TH1F("hr_ele_tsal","tsal right tags", 50, x1, x2);
TH1F* hw_ele_tsal = new TH1F("hw_ele_tsal","tsal wrong tags", 50, x1, x2);
x1=0; x2=100;//N mult
TH1F* hr_ele_mult = new TH1F("hr_ele_mult","mult right tags", 50, x1, x2);
TH1F* hw_ele_mult = new TH1F("hw_ele_mult","mult wrong tags", 50, x1, x2);
x1=0; x2=3;//VeloCharge
TH1F* hr_ele_veloch = new TH1F("hr_ele_veloch","veloch right tags", 50, x1, x2);
TH1F* hw_ele_veloch = new TH1F("hw_ele_veloch","veloch wrong tags", 50, x1, x2);

//KAON OPP
x1=0; x2=5  ; //pt
TH1F* hr_k_pt = new TH1F("hr_k_pt","pt right tags", 50, x1, x2);
TH1F* hw_k_pt = new TH1F("hw_k_pt","pt wrong tags", 50, x1, x2);
x1=0; x2=70 ; //p
TH1F* hr_k_p = new TH1F("hr_k_p","p right tags", 50, x1, x2);
TH1F* hw_k_p = new TH1F("hw_k_p","p wrong tags", 50, x1, x2);
x1=2; x2=10  ; //ip/s
TH1F* hr_k_ips = new TH1F("hr_k_ips","ips right tags", 50, x1, x2);
TH1F* hw_k_ips = new TH1F("hw_k_ips","ips wrong tags", 50, x1, x2);
x1=0; x2=100  ; //ippu
TH1F* hr_k_ippu = new TH1F("hr_k_ippu","ippu right tags", 50, x1, x2);
TH1F* hw_k_ippu = new TH1F("hw_k_ippu","ippu wrong tags", 50, x1, x2);
x1=0; x2=5   ; //LCS
TH1F* hr_k_lcs = new TH1F("hr_k_lcs","lcs right tags", 50, x1, x2);
TH1F* hw_k_lcs = new TH1F("hw_k_lcs","lcs wrong tags", 50, x1, x2);
x1=-5, x2=95; // PID
TH1F* hr_k_pid = new TH1F("hr_k_pid","pid right tags", 50, x1, x2);
TH1F* hw_k_pid = new TH1F("hw_k_pid","pid wrong tags", 50, x1, x2);
x1=-35; x2=0 ;// tsal ghost likel
TH1F* hr_k_tsal = new TH1F("hr_k_tsal","tsal right tags", 50, x1, x2);
TH1F* hw_k_tsal = new TH1F("hw_k_tsal","tsal wrong tags", 50, x1, x2);
x1=0; x2=100;//N mult
TH1F* hr_k_mult = new TH1F("hr_k_mult","mult right tags", 50, x1, x2);
TH1F* hw_k_mult = new TH1F("hw_k_mult","mult wrong tags", 50, x1, x2);

//KAON/PION SAME
x1=0; x2=5  ; //pt
TH1F* hr_kS_pt = new TH1F("hr_kS_pt","pt right tags", 50, x1, x2);
TH1F* hw_kS_pt = new TH1F("hw_kS_pt","pt wrong tags", 50, x1, x2);
x1=0; x2=70 ; //p
TH1F* hr_kS_p = new TH1F("hr_kS_p","p right tags", 50, x1, x2);
TH1F* hw_kS_p = new TH1F("hw_kS_p","p wrong tags", 50, x1, x2);
x1=0; x2=15  ; //ip/s
TH1F* hr_kS_ips = new TH1F("hr_kS_ips","ips right tags", 50, x1, x2);
TH1F* hw_kS_ips = new TH1F("hw_kS_ips","ips wrong tags", 50, x1, x2);
x1=0; x2=100  ; //ippu
TH1F* hr_kS_ippu = new TH1F("hr_kS_ippu","ippu right tags", 50, x1, x2);
TH1F* hw_kS_ippu = new TH1F("hw_kS_ippu","ippu wrong tags", 50, x1, x2);
x1=0; x2=5   ; //LCS
TH1F* hr_kS_lcs = new TH1F("hr_kS_lcs","lcs right tags", 50, x1, x2);
TH1F* hw_kS_lcs = new TH1F("hw_kS_lcs","lcs wrong tags", 50, x1, x2);
x1=-5, x2=95; // PID
TH1F* hr_kS_pid = new TH1F("hr_kS_pid","pid right tags", 50, x1, x2);
TH1F* hw_kS_pid = new TH1F("hw_kS_pid","pid wrong tags", 50, x1, x2);
x1=-35; x2=0 ;// tsal ghost likel
TH1F* hr_kS_tsal = new TH1F("hr_kS_tsal","tsal right tags", 50, x1, x2);
TH1F* hw_kS_tsal = new TH1F("hw_kS_tsal","tsal wrong tags", 50, x1, x2);
x1=0; x2=100;//N mult
TH1F* hr_kS_mult = new TH1F("hr_kS_mult","mult right tags", 50, x1, x2);
TH1F* hw_kS_mult = new TH1F("hw_kS_mult","mult wrong tags", 50, x1, x2);

//deltaphi with signal part
x1=0; x2=1;
TH1F* homphi_mu_r = new TH1F("homphi_mu_r","w(dphi)", 50, x1, x2);
TH1F* homphi_mu_w = new TH1F("homphi_mu_w","w(dphi)", 50, x1, x2);

TH1F* homphi_ele_r = new TH1F("homphi_ele_r","w(dphi)", 50, x1, x2);
TH1F* homphi_ele_w = new TH1F("homphi_ele_w","w(dphi)", 50, x1, x2);

TH1F* homphi_kO_r = new TH1F("homphi_kO_r","w(dphi)", 50, x1, x2);
TH1F* homphi_kO_w = new TH1F("homphi_kO_w","w(dphi)", 50, x1, x2);

TH1F* homphi_kS_r = new TH1F("homphi_kS_r","w(dphi)", 50, x1, x2);
TH1F* homphi_kS_w = new TH1F("homphi_kS_w","w(dphi)", 50, x1, x2);

//vtx charge
//x1=-1.01; x2=1.01;
x1=0.01; x2=1.1; //abs value
TH1F* hvtxch_r = new TH1F("hvtxch_r","vtxch", 50, x1, x2);
TH1F* hvtxch_w = new TH1F("hvtxch_w","vtxch", 50, x1, x2);
TH1F* hvtxch_u = new TH1F("hvtxch_u","vtxch", 50, x1, x2);

