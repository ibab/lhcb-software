
  gROOT->Reset();
  TFile fhisto("tag.root","RECREATE");
  if(rootsubdir != "") { fhisto.mkdir(rootsubdir); fhisto.cd(rootsubdir); } 
  cout<<"Using "<<mlptype<<endl;

  //create tree of nnet variables:
  TTree *nntrain = new TTree("nntrain", "tree neural net");
  Int_t iscorrect,tagger;
  nntrain->Branch("iscorrect",&iscorrect, "iscorrect/I");
  nntrain->Branch("tagger", &tagger,  "tagger/I");
  nntrain->Branch("mult",   &mult,    "mult/F");
  nntrain->Branch("ptB",    &ptB,     "ptB/F");
  nntrain->Branch("partP",  &partP,   "partP/F");
  nntrain->Branch("partPt", &partPt,  "partPt/F");
  nntrain->Branch("IPPV",   &IPPV,    "IPPV/F");
  nntrain->Branch("nndeta", &nndeta,  "nndeta/F");
  nntrain->Branch("nndphi", &nndphi,  "nndphi/F");
  nntrain->Branch("nndq",   &nndq,    "nndq/F");
  nntrain->Branch("nkrec",  &nkrec,   "nkrec/F");
  nntrain->Branch("ncand",  &ncand,   "ncand/F");

  //create  histograms
  TH1F* h5    = new TH1F("h5","BSmass ",100, 5.2, 5.5);

  TH1F* h6    = new TH1F("h6","B pT ",100, 0.0, 30.);
  TH1F* h6rSS = new TH1F("h6rSS","B pT ",100, 0.0, 30.);
  TH1F* h6wSS = new TH1F("h6wSS","B pT ",100, 0.0, 30.);
  TH1F* h6rOS = new TH1F("h6rOS","B pT ",100, 0.0, 30.);
  TH1F* h6wOS = new TH1F("h6wOS","B pT ",100, 0.0, 30.);

  TH1F* h513  = new TH1F("h513","NNmu ", 50, 0, 1.);
  TH1F* h613  = new TH1F("h613","NNmu ", 50, 0, 1.);
  TH1F* h513p = new TH1F("h513p","NNmu ", 50, 0, .7);
  TH1F* h613p = new TH1F("h613p","NNmu ", 50, 0, .7);
  TH1F* h511  = new TH1F("h511","NNe  ", 50, 0, 1.);
  TH1F* h611  = new TH1F("h611","NNe  ", 50, 0, 1.);
  TH1F* h511p = new TH1F("h511p","NNe  ", 50, 0, .7);
  TH1F* h611p = new TH1F("h611p","NNe  ", 50, 0, .7);
  TH1F* h5321 = new TH1F("h5321","NNk ", 50, .0, 1.);
  TH1F* h6321 = new TH1F("h6321","NNk ", 50, .0, 1.);
  TH1F* h5321p= new TH1F("h5321p","NNk ", 50, 0, .7);
  TH1F* h6321p= new TH1F("h6321p","NNk ", 50, 0, .7);
  TH1F* h53210= new TH1F("h53210","NNks/pi",50, .0, 1.);
  TH1F* h63210= new TH1F("h63210","NNks/pi",50, .0, 1.);
  TH1F* h53210p= new TH1F("h53210p","NNks/pi",50, 0, 0.7);
  TH1F* h63210p= new TH1F("h63210p","NNks/pi",50, 0, 0.7);
  TH1F* h540  = new TH1F("h540","vert charge ", 70, .1, .8);
  TH1F* h640  = new TH1F("h640","vert charge ", 70, .1, .8);
  TH1F* h540p = new TH1F("h540p","vert charge ", 70, 0, .7);
  TH1F* h640p = new TH1F("h640p","vert charge ", 70, 0, .7);
  TH1F* h100  = new TH1F("h100","PNetSum ", 60, 0.4, 1.);
  TH1F* h102  = new TH1F("h102","PNetSum ", 60, 0.4, 1.);
  TH1F* h103  = new TH1F("h103","1-PNetSum",60, 0.0, 0.6);
  TH1F* h104  = new TH1F("h104","1-PNetSum",60, 0.0, 0.6);
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
  TH1F* h301  = new TH1F("h301","P B Opposite",50, 0, 500.);
  TH1F* h401  = new TH1F("h401","P B",50, 0, 500.);
  TH1F* h302  = new TH1F("h302","theta B",50, 0, 300.);
  TH1F* h311  = new TH1F("h311","mult",50, 0, 100.);
 
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
  TH1F* h88 = new TH1F("h88","ko ncand" ,10, 0., 10);
  TH1F* h89 = new TH1F("h89","ko mult" ,50, 0., 100);

  TH1F* h80_3 = new TH1F("h80_3","ko pt",50, 0., 10.); //right nu=3
  TH1F* h81_3 = new TH1F("h81_3","ko p" ,50, 0., 100.);
  TH1F* h82_3 = new TH1F("h82_3","ko ip/s" ,50, -30., 30.);
  TH1F* h83_3 = new TH1F("h83_3","ko LCS" ,50, 0., 5.);
  TH1F* h84_3 = new TH1F("h84_3","ko ghost prob" ,50, -50., 0.);
  TH1F* h85_3 = new TH1F("h85_3","ko PIDk" ,50, -40., 100.);
  TH1F* h86_3 = new TH1F("h86_3","ko PIDkp" ,50, -10., 50.);
  TH1F* h87_3 = new TH1F("h87_3","ko distphi" ,50, 0., .2);
  TH1F* h88_3 = new TH1F("h88_3","ko ncand" ,10, 0., 10);
  TH1F* h89_3 = new TH1F("h89_3","ko mult" ,50, 0., 100);

  //MUON
  float x1=0, x2=5  ; //pt
  TH1F* hr_mu_pt = new TH1F("hr_mu_pt","pt right tags", 50, x1, x2);
  TH1F* hw_mu_pt = new TH1F("hw_mu_pt","pt wrong tags", 50, x1, x2);
//  TH1F* ho_mu_pt = new TH1F("ho_mu_pt","pt omega",      50, x1, x2);
  TH1F* he_mu_pt = new TH1F("he_mu_pt","pt effective efficiency", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_mu_p = new TH1F("hr_mu_p","p right tags", 50, x1, x2);
  TH1F* hw_mu_p = new TH1F("hw_mu_p","p wrong tags", 50, x1, x2);
//  TH1F* ho_mu_p = new TH1F("ho_mu_p","p omega",      50, x1, x2);
  TH1F* he_mu_p = new TH1F("he_mu_p","p effective efficiency", 50, x1, x2);
  x1=0; x2=50  ; //ip/s
  TH1F* hr_mu_ips = new TH1F("hr_mu_ips","ips right tags", 50, x1, x2);
  TH1F* hw_mu_ips = new TH1F("hw_mu_ips","ips wrong tags", 50, x1, x2);
//  TH1F* ho_mu_ips = new TH1F("ho_mu_ips","ips omega",      50, x1, x2);
  TH1F* he_mu_ips = new TH1F("he_mu_ips","ips effective efficiency", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_mu_ippu = new TH1F("hr_mu_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_mu_ippu = new TH1F("hw_mu_ippu","ippu wrong tags", 50, x1, x2);
//  TH1F* ho_mu_ippu = new TH1F("ho_mu_ippu","ippu omega",      50, x1, x2);
  TH1F* he_mu_ippu = new TH1F("he_mu_ippu","ippu effective efficiency", 50, x1, x2);
  x1=0; x2=5   ; //LCS
  TH1F* hr_mu_lcs = new TH1F("hr_mu_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_mu_lcs = new TH1F("hw_mu_lcs","lcs wrong tags", 50, x1, x2);
//  TH1F* ho_mu_lcs = new TH1F("ho_mu_lcs","lcs omega",      50, x1, x2);
  TH1F* he_mu_lcs = new TH1F("he_mu_lcs","lcs effective efficiency", 50, x1, x2);
  x1=-5, x2=20; // PID
  TH1F* hr_mu_pid = new TH1F("hr_mu_pid","pid right tags", 50, x1, x2);
  TH1F* hw_mu_pid = new TH1F("hw_mu_pid","pid wrong tags", 50, x1, x2);
//  TH1F* ho_mu_pid = new TH1F("ho_mu_pid","pid omega",      50, x1, x2);
  TH1F* he_mu_pid = new TH1F("he_mu_pid","pid effective efficiency", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_mu_tsal = new TH1F("hr_mu_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_mu_tsal = new TH1F("hw_mu_tsal","tsal wrong tags", 50, x1, x2);
//  TH1F* ho_mu_tsal = new TH1F("ho_mu_tsal","tsal omega",      50, x1, x2);
  TH1F* he_mu_tsal = new TH1F("he_mu_tsal","tsal effective efficiency", 50, x1, x2);
  x1=0; x2=100;//N mult
  TH1F* hr_mu_mult = new TH1F("hr_mu_mult","mult right tags", 50, x1, x2);
  TH1F* hw_mu_mult = new TH1F("hw_mu_mult","mult wrong tags", 50, x1, x2);
//  TH1F* ho_mu_mult = new TH1F("ho_mu_mult","mult omega",      50, x1, x2);
  TH1F* he_mu_mult = new TH1F("he_mu_mult","mult effective efficiency", 50, x1, x2);
  x1=1; x2=21  ; //ncands
  TH1F* hr_mu_ncand = new TH1F("hr_mu_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_mu_ncand = new TH1F("hw_mu_ncand","ncand wrong tags", 50, x1, x2);
//  TH1F* ho_mu_ncand = new TH1F("ho_mu_ncand","ncand omega",      50, x1, x2);
  TH1F* he_mu_ncand = new TH1F("he_mu_ncand","ncand effective efficiency", 50, x1, x2);

  //ELECTRON
  x1=0; x2=5  ; //pt
  TH1F* hr_ele_pt = new TH1F("hr_ele_pt","pt right tags", 50, x1, x2);
  TH1F* hw_ele_pt = new TH1F("hw_ele_pt","pt wrong tags", 50, x1, x2);
//  TH1F* ho_ele_pt = new TH1F("ho_ele_pt","pt omega",      50, x1, x2);
  TH1F* he_ele_pt = new TH1F("he_ele_pt","pt effective efficiency", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_ele_p = new TH1F("hr_ele_p","p right tags", 50, x1, x2);
  TH1F* hw_ele_p = new TH1F("hw_ele_p","p wrong tags", 50, x1, x2);
//  TH1F* ho_ele_p = new TH1F("ho_ele_p","p omega",      50, x1, x2);
  TH1F* he_ele_p = new TH1F("he_ele_p","p effective efficiency", 50, x1, x2);
  x1=0; x2=50  ; //ip/s
  TH1F* hr_ele_ips = new TH1F("hr_ele_ips","ips right tags", 50, x1, x2);
  TH1F* hw_ele_ips = new TH1F("hw_ele_ips","ips wrong tags", 50, x1, x2);
//  TH1F* ho_ele_ips = new TH1F("ho_ele_ips","ips omega",      50, x1, x2);
  TH1F* he_ele_ips = new TH1F("he_ele_ips","ips effective efficiency", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_ele_ippu = new TH1F("hr_ele_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_ele_ippu = new TH1F("hw_ele_ippu","ippu wrong tags", 50, x1, x2);
//  TH1F* ho_ele_ippu = new TH1F("ho_ele_ippu","ippu omega",      50, x1, x2);
  TH1F* he_ele_ippu = new TH1F("he_ele_ippu","ippu effective efficiency", 50, x1, x2);
  x1=0; x2=5   ; //LCS
  TH1F* hr_ele_lcs = new TH1F("hr_ele_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_ele_lcs = new TH1F("hw_ele_lcs","lcs wrong tags", 50, x1, x2);
//  TH1F* ho_ele_lcs = new TH1F("ho_ele_lcs","lcs omega",      50, x1, x2);
  TH1F* he_ele_lcs = new TH1F("he_ele_lcs","lcs effective efficiency", 50, x1, x2);
  x1=-5, x2=20; // PID
  TH1F* hr_ele_pid = new TH1F("hr_ele_pid","pid right tags", 50, x1, x2);
  TH1F* hw_ele_pid = new TH1F("hw_ele_pid","pid wrong tags", 50, x1, x2);
//  TH1F* ho_ele_pid = new TH1F("ho_ele_pid","pid omega",      50, x1, x2);
  TH1F* he_ele_pid = new TH1F("he_ele_pid","pid effective efficiency", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_ele_tsal = new TH1F("hr_ele_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_ele_tsal = new TH1F("hw_ele_tsal","tsal wrong tags", 50, x1, x2);
//  TH1F* ho_ele_tsal = new TH1F("ho_ele_tsal","tsal omega",      50, x1, x2);
  TH1F* he_ele_tsal = new TH1F("he_ele_tsal","tsal effective efficiency", 50, x1, x2);
  x1=0; x2=100;//N mult
  TH1F* hr_ele_mult = new TH1F("hr_ele_mult","mult right tags", 50, x1, x2);
  TH1F* hw_ele_mult = new TH1F("hw_ele_mult","mult wrong tags", 50, x1, x2);
//  TH1F* ho_ele_mult = new TH1F("ho_ele_mult","mult omega",      50, x1, x2);
  TH1F* he_ele_mult = new TH1F("he_ele_mult","mult effective efficiency", 50, x1, x2);
  x1=1; x2=21  ; //ncands
  TH1F* hr_ele_ncand = new TH1F("hr_ele_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_ele_ncand = new TH1F("hw_ele_ncand","ncand wrong tags", 50, x1, x2);
//  TH1F* ho_ele_ncand = new TH1F("ho_ele_ncand","ncand omega",      50, x1, x2);
  TH1F* he_ele_ncand = new TH1F("he_ele_ncand","ncand effective efficiency", 50, x1, x2);
  x1=0; x2=3; //ele veloch
//  TH1F* hr_ele_veloch = new TH1F("hr_ele_veloch","veloch right tags", 50, x1, x2);
//  TH1F* hw_ele_veloch = new TH1F("hw_ele_veloch","veloch wrong tags", 50, x1, x2);
//  TH1F* ho_ele_veloch = new TH1F("ho_ele_veloch","veloch omega",      50, x1, x2);
//  TH1F* he_ele_veloch = new TH1F("he_ele_veloch","veloch effective efficiency", 50, x1, x2);
  x1=0; x2=2; //ele EoverP
//  TH1F* hr_ele_EoverP = new TH1F("hr_ele_EoverP","EoverP right tags", 50, x1, x2);
//  TH1F* hw_ele_EoverP = new TH1F("hw_ele_EoverP","EoverP wrong tags", 50, x1, x2);
//  TH1F* ho_ele_EoverP = new TH1F("ho_ele_EoverP","EoverP omega",      50, x1, x2);
//  TH1F* he_ele_EoverP = new TH1F("he_ele_EoverP","EoverP effective efficiency", 50, x1, x2);

  //KAON OPPO
  x1=0; x2=5  ; //pt
  TH1F* hr_k_pt = new TH1F("hr_k_pt","pt right tags", 50, x1, x2);
  TH1F* hw_k_pt = new TH1F("hw_k_pt","pt wrong tags", 50, x1, x2);
//  TH1F* ho_k_pt = new TH1F("ho_k_pt","pt omega",      50, x1, x2);
  TH1F* he_k_pt = new TH1F("he_k_pt","pt effective efficiency", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_k_p = new TH1F("hr_k_p","p right tags", 50, x1, x2);
  TH1F* hw_k_p = new TH1F("hw_k_p","p wrong tags", 50, x1, x2);
//  TH1F* ho_k_p = new TH1F("ho_k_p","p omega",      50, x1, x2);
  TH1F* he_k_p = new TH1F("he_k_p","p effective efficiency", 50, x1, x2);
  x1=0; x2=50  ; //ip/s
  TH1F* hr_k_ips = new TH1F("hr_k_ips","ips right tags", 50, x1, x2);
  TH1F* hw_k_ips = new TH1F("hw_k_ips","ips wrong tags", 50, x1, x2);
//  TH1F* ho_k_ips = new TH1F("ho_k_ips","ips omega",      50, x1, x2);
  TH1F* he_k_ips = new TH1F("he_k_ips","ips effective efficiency", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_k_ippu = new TH1F("hr_k_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_k_ippu = new TH1F("hw_k_ippu","ippu wrong tags", 50, x1, x2);
//  TH1F* ho_k_ippu = new TH1F("ho_k_ippu","ippu omega",      50, x1, x2);
  TH1F* he_k_ippu = new TH1F("he_k_ippu","ippu effective efficiency", 50, x1, x2);
  x1=0; x2=5   ; //LCS
  TH1F* hr_k_lcs = new TH1F("hr_k_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_k_lcs = new TH1F("hw_k_lcs","lcs wrong tags", 50, x1, x2);
//  TH1F* ho_k_lcs = new TH1F("ho_k_lcs","lcs omega",      50, x1, x2);
  TH1F* he_k_lcs = new TH1F("he_k_lcs","lcs effective efficiency", 50, x1, x2);
  x1=-5, x2=95; // PID
  TH1F* hr_k_pid = new TH1F("hr_k_pid","pid right tags", 50, x1, x2);
  TH1F* hw_k_pid = new TH1F("hw_k_pid","pid wrong tags", 50, x1, x2);
//  TH1F* ho_k_pid = new TH1F("ho_k_pid","pid omega",      50, x1, x2);
  TH1F* he_k_pid = new TH1F("he_k_pid","pid effective efficiency", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_k_tsal = new TH1F("hr_k_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_k_tsal = new TH1F("hw_k_tsal","tsal wrong tags", 50, x1, x2);
//  TH1F* ho_k_tsal = new TH1F("ho_k_tsal","tsal omega",      50, x1, x2);
  TH1F* he_k_tsal = new TH1F("he_k_tsal","tsal effective efficiency", 50, x1, x2);
  x1=0; x2=100;//N mult
  TH1F* hr_k_mult = new TH1F("hr_k_mult","mult right tags", 50, x1, x2);
  TH1F* hw_k_mult = new TH1F("hw_k_mult","mult wrong tags", 50, x1, x2);
//  TH1F* ho_k_mult = new TH1F("ho_k_mult","mult omega",      50, x1, x2);
  TH1F* he_k_mult = new TH1F("he_k_mult","mult effective efficiency", 50, x1, x2);
  x1=1; x2=21  ; //ncands
  TH1F* hr_k_ncand = new TH1F("hr_k_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_k_ncand = new TH1F("hw_k_ncand","ncand wrong tags", 50, x1, x2);
//  TH1F* ho_k_ncand = new TH1F("ho_k_ncand","ncand omega",      50, x1, x2);
  TH1F* he_k_ncand = new TH1F("he_k_ncand","ncand effective efficiency", 50, x1, x2);

  //KAON SAME
  x1=0; x2=5  ; //pt
  TH1F* hr_kS_pt = new TH1F("hr_kS_pt","pt right tags", 50, x1, x2);
  TH1F* hw_kS_pt = new TH1F("hw_kS_pt","pt wrong tags", 50, x1, x2);
//  TH1F* ho_kS_pt = new TH1F("ho_kS_pt","pt omega",      50, x1, x2);
  TH1F* he_kS_pt = new TH1F("he_kS_pt","pt effective efficiency", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_kS_p = new TH1F("hr_kS_p","p right tags", 50, x1, x2);
  TH1F* hw_kS_p = new TH1F("hw_kS_p","p wrong tags", 50, x1, x2);
//  TH1F* ho_kS_p = new TH1F("ho_kS_p","p omega",      50, x1, x2);
  TH1F* he_kS_p = new TH1F("he_kS_p","p effective efficiency", 50, x1, x2);
  x1=0; x2=15  ; //ip/s
  TH1F* hr_kS_ips = new TH1F("hr_kS_ips","ips right tags", 50, x1, x2);
  TH1F* hw_kS_ips = new TH1F("hw_kS_ips","ips wrong tags", 50, x1, x2);
//  TH1F* ho_kS_ips = new TH1F("ho_kS_ips","ips omega",      50, x1, x2);
  TH1F* he_kS_ips = new TH1F("he_kS_ips","ips effective efficiency", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_kS_ippu = new TH1F("hr_kS_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_kS_ippu = new TH1F("hw_kS_ippu","ippu wrong tags", 50, x1, x2);
//  TH1F* ho_kS_ippu = new TH1F("ho_kS_ippu","ippu omega",      50, x1, x2);
  TH1F* he_kS_ippu = new TH1F("he_kS_ippu","ippu effective efficiency", 50, x1, x2);
  x1=0; x2=5   ; //LCS
  TH1F* hr_kS_lcs = new TH1F("hr_kS_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_kS_lcs = new TH1F("hw_kS_lcs","lcs wrong tags", 50, x1, x2);
//  TH1F* ho_kS_lcs = new TH1F("ho_kS_lcs","lcs omega",      50, x1, x2);
  TH1F* he_kS_lcs = new TH1F("he_kS_lcs","lcs effective efficiency", 50, x1, x2);
  x1=-5, x2=95; // PID
  TH1F* hr_kS_pid = new TH1F("hr_kS_pid","pid right tags", 50, x1, x2);
  TH1F* hw_kS_pid = new TH1F("hw_kS_pid","pid wrong tags", 50, x1, x2);
//  TH1F* ho_kS_pid = new TH1F("ho_kS_pid","pid omega",      50, x1, x2);
  TH1F* he_kS_pid = new TH1F("he_kS_pid","pid effective efficiency", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_kS_tsal = new TH1F("hr_kS_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_kS_tsal = new TH1F("hw_kS_tsal","tsal wrong tags", 50, x1, x2);
//  TH1F* ho_kS_tsal = new TH1F("ho_kS_tsal","tsal omega",      50, x1, x2);
  TH1F* he_kS_tsal = new TH1F("he_kS_tsal","tsal effective efficiency", 50, x1, x2);
  x1=0; x2=100;//N mult
  TH1F* hr_kS_mult = new TH1F("hr_kS_mult","mult right tags", 50, x1, x2);
  TH1F* hw_kS_mult = new TH1F("hw_kS_mult","mult wrong tags", 50, x1, x2);
//  TH1F* ho_kS_mult = new TH1F("ho_kS_mult","mult omega",      50, x1, x2);
  TH1F* he_kS_mult = new TH1F("he_kS_mult","mult effective efficiency", 50, x1, x2);
  x1=1; x2=21  ; //ncands
  TH1F* hr_kS_ncand = new TH1F("hr_kS_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_kS_ncand = new TH1F("hw_kS_ncand","ncand wrong tags", 50, x1, x2);
//  TH1F* ho_kS_ncand = new TH1F("ho_kS_ncand","ncand omega",      50, x1, x2);
  TH1F* he_kS_ncand = new TH1F("he_kS_ncand","ncand effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //dphi
//  TH1F* hr_kS_dphi = new TH1F("hr_kS_dphi","dphi right tags", 50, x1, x2);
//  TH1F* hw_kS_dphi = new TH1F("hw_kS_dphi","dphi wrong tags", 50, x1, x2);
//  TH1F* ho_kS_dphi = new TH1F("ho_kS_dphi","dphi omega",      50, x1, x2);
//  TH1F* he_kS_dphi = new TH1F("he_kS_dphi","dphi effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //deta
//  TH1F* hr_kS_deta = new TH1F("hr_kS_deta","deta right tags", 50, x1, x2);
//  TH1F* hw_kS_deta = new TH1F("hw_kS_deta","deta wrong tags", 50, x1, x2);
//  TH1F* ho_kS_deta = new TH1F("ho_kS_deta","deta omega",      50, x1, x2);
//  TH1F* he_kS_deta = new TH1F("he_kS_deta","deta effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //dq
//  TH1F* hr_kS_dq = new TH1F("hr_kS_dq","dq right tags", 50, x1, x2);
//  TH1F* hw_kS_dq = new TH1F("hw_kS_dq","dq wrong tags", 50, x1, x2);
//  TH1F* ho_kS_dq = new TH1F("ho_kS_dq","dq omega",      50, x1, x2);
//  TH1F* he_kS_dq = new TH1F("he_kS_dq","dq effective efficiency", 50, x1, x2);

  //PION SAME
  x1=0; x2=5  ; //pt
  TH1F* hr_pS_pt = new TH1F("hr_pS_pt","pt right tags", 50, x1, x2);
  TH1F* hw_pS_pt = new TH1F("hw_pS_pt","pt wrong tags", 50, x1, x2);
//  TH1F* ho_pS_pt = new TH1F("ho_pS_pt","pt omega",      50, x1, x2);
  TH1F* he_pS_pt = new TH1F("he_pS_pt","pt effective efficiency", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_pS_p = new TH1F("hr_pS_p","p right tags", 50, x1, x2);
  TH1F* hw_pS_p = new TH1F("hw_pS_p","p wrong tags", 50, x1, x2);
//  TH1F* ho_pS_p = new TH1F("ho_pS_p","p omega",      50, x1, x2);
  TH1F* he_pS_p = new TH1F("he_pS_p","p effective efficiency", 50, x1, x2);
  x1=0; x2=15  ; //ip/s
  TH1F* hr_pS_ips = new TH1F("hr_pS_ips","ips right tags", 50, x1, x2);
  TH1F* hw_pS_ips = new TH1F("hw_pS_ips","ips wrong tags", 50, x1, x2);
//  TH1F* ho_pS_ips = new TH1F("ho_pS_ips","ips omega",      50, x1, x2);
  TH1F* he_pS_ips = new TH1F("he_pS_ips","ips effective efficiency", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_pS_ippu = new TH1F("hr_pS_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_pS_ippu = new TH1F("hw_pS_ippu","ippu wrong tags", 50, x1, x2);
//  TH1F* ho_pS_ippu = new TH1F("ho_pS_ippu","ippu omega",      50, x1, x2);
  TH1F* he_pS_ippu = new TH1F("he_pS_ippu","ippu effective efficiency", 50, x1, x2);
  x1=0; x2=5   ; //LCS
  TH1F* hr_pS_lcs = new TH1F("hr_pS_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_pS_lcs = new TH1F("hw_pS_lcs","lcs wrong tags", 50, x1, x2);
//  TH1F* ho_pS_lcs = new TH1F("ho_pS_lcs","lcs omega",      50, x1, x2);
  TH1F* he_pS_lcs = new TH1F("he_pS_lcs","lcs effective efficiency", 50, x1, x2);
  x1=-5, x2=95; // PID
  TH1F* hr_pS_pid = new TH1F("hr_pS_pid","pid right tags", 50, x1, x2);
  TH1F* hw_pS_pid = new TH1F("hw_pS_pid","pid wrong tags", 50, x1, x2);
//  TH1F* ho_pS_pid = new TH1F("ho_pS_pid","pid omega",      50, x1, x2);
  TH1F* he_pS_pid = new TH1F("he_pS_pid","pid effective efficiency", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_pS_tsal = new TH1F("hr_pS_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_pS_tsal = new TH1F("hw_pS_tsal","tsal wrong tags", 50, x1, x2);
//  TH1F* ho_pS_tsal = new TH1F("ho_pS_tsal","tsal omega",      50, x1, x2);
  TH1F* he_pS_tsal = new TH1F("he_pS_tsal","tsal effective efficiency", 50, x1, x2);
  x1=0; x2=100;//N mult
  TH1F* hr_pS_mult = new TH1F("hr_pS_mult","mult right tags", 50, x1, x2);
  TH1F* hw_pS_mult = new TH1F("hw_pS_mult","mult wrong tags", 50, x1, x2);
//  TH1F* ho_pS_mult = new TH1F("ho_pS_mult","mult omega",      50, x1, x2);
  TH1F* he_pS_mult = new TH1F("he_pS_mult","mult effective efficiency", 50, x1, x2);
  x1=1; x2=21  ; //ncands
  TH1F* hr_pS_ncand = new TH1F("hr_pS_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_pS_ncand = new TH1F("hw_pS_ncand","ncand wrong tags", 50, x1, x2);
//  TH1F* ho_pS_ncand = new TH1F("ho_pS_ncand","ncand omega",      50, x1, x2);
  TH1F* he_pS_ncand = new TH1F("he_pS_ncand","ncand effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //dphi
//  TH1F* hr_pS_dphi = new TH1F("hr_pS_dphi","dphi right tags", 50, x1, x2);
//  TH1F* hw_pS_dphi = new TH1F("hw_pS_dphi","dphi wrong tags", 50, x1, x2);
//  TH1F* ho_pS_dphi = new TH1F("ho_pS_dphi","dphi omega",      50, x1, x2);
//  TH1F* he_pS_dphi = new TH1F("he_pS_dphi","dphi effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //deta
//  TH1F* hr_pS_deta = new TH1F("hr_pS_deta","deta right tags", 50, x1, x2);
//  TH1F* hw_pS_deta = new TH1F("hw_pS_deta","deta wrong tags", 50, x1, x2);
//  TH1F* ho_pS_deta = new TH1F("ho_pS_deta","deta omega",      50, x1, x2);
//  TH1F* he_pS_deta = new TH1F("he_pS_deta","deta effective efficiency", 50, x1, x2);
  x1=0, x2=5  ; //dq
//  TH1F* hr_pS_dq = new TH1F("hr_pS_dq","dq right tags", 50, x1, x2);
//  TH1F* hw_pS_dq = new TH1F("hw_pS_dq","dq wrong tags", 50, x1, x2);
//  TH1F* ho_pS_dq = new TH1F("ho_pS_dq","dq omega",      50, x1, x2);
//  TH1F* he_pS_dq = new TH1F("he_pS_dq","dq effective efficiency", 50, x1, x2);

  x1=0; x2=1;
  TH1F* homphi_mu_r = new TH1F("homphi_mu_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_mu_w = new TH1F("homphi_mu_w","w(dphi)", 50, x1, x2);

  TH1F* homphi_kO_r = new TH1F("homphi_kO_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_kO_w = new TH1F("homphi_kO_w","w(dphi)", 50, x1, x2);

  TH1F* homphi_kS_r = new TH1F("homphi_kS_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_kS_w = new TH1F("homphi_kS_w","w(dphi)", 50, x1, x2);


  TH1F* hright = new TH1F("hright","right tags", 50, x1, x2);
  TH1F* hwrong = new TH1F("hwrong","wrong tags", 50, x1, x2);
  TH1F* homega = new TH1F("homega","omega",      50, x1, x2);
  TH1F* heffec = new TH1F("heffec","effective efficiency", 50, x1, x2);
  TString direction="left2right";
