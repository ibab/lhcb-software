//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
taginit(); //read option file for global variables

//NB: you can also book and fill histograms inside tools!

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
TString rootsubdir = ""; 
TFile fhisto("output/tag.root","RECREATE"); 
if(rootsubdir != "") { fhisto.mkdir(rootsubdir); fhisto.cd(rootsubdir); } 

//>>>>>>>>>>>>>>>>>>>>>>>>//create  histograms
float x1 = 0.5, x2 = 3;
TH1F* hright = new TH1F("hright","right tags", 50, x1, x2);
TH1F* hwrong = new TH1F("hwrong","wrong tags", 50, x1, x2);
TString direction="left2right";

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
TH1F* h1    = new TH1F("h1","BSmass ",100, 4, 6);
TH1F* h2    = new TH1F("h2","nr of primary vtx" ,10, 0., 10);
TH1F* h3    = new TH1F("h3","Pt B Opposite",50, 0, 100.);
TH1F* h4    = new TH1F("h4","track multiplicity",50, 0, 100.);

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

//for pv studies
TH1F* hpv_ipmean_r = new TH1F("hpv_ipmean_r","IP mean",50, 0.0, 25.);
TH1F* hpv_ipmean_w = new TH1F("hpv_ipmean_w","IP mean",50, 0.0, 25.);
TH1F* hpv_ippu_r = new TH1F("hpv_ippu_r","ippu tags", 50, 0.0, 50);
TH1F* hpv_ippu_w = new TH1F("hpv_ippu_w","ippu tags", 50, 0.0, 50);
TH1F* hpv_gippu_r = new TH1F("hpv_gippu_r","gippu tags", 50, 0.0, 50);
TH1F* hpv_gippu_w = new TH1F("hpv_gippu_w","gippu tags", 50, 0.0, 50);
TH1F* hpv_nippu_r = new TH1F("hpv_nippu_r","min IPPU",50, 0.0, 25.);
TH1F* hpv_nippu_w = new TH1F("hpv_nippu_w","min IPPU",50, 0.0, 25.);
TH1F* hpv_nippuerr_r = new TH1F("hpv_nippuerr_r","iperr of min IPPU",50, 0.0, 1000.);
TH1F* hpv_nippuerr_w = new TH1F("hpv_nippuerr_w","iperr of min IPPU",50, 0.0, 1000.);
TH1F* hpv_tracks_r = new TH1F("hpv_tracks_r","tracks",50, 0.0, 100.);
TH1F* hpv_tracks_w = new TH1F("hpv_tracks_w","tracks",50, 0.0, 100.);
TH1F* hpv_trackzfirst_r = new TH1F("hpv_trackzfirst_r","trackzfirst",50, -150., 200.);
TH1F* hpv_trackzfirst_w = new TH1F("hpv_trackzfirst_w","trackzfirst",50, -150., 200.);
TH1F* hpv_trackp_r = new TH1F("hpv_trackp_r","trackp",50, 0.0, 10000.);
TH1F* hpv_trackp_w = new TH1F("hpv_trackp_w","trackp",50, 0.0, 10000.);
TH1F* hpv_zposdif_r = new TH1F("hpv_zposdif_r","Z pos dif",50, -25., 25.);
TH1F* hpv_zposdif_w = new TH1F("hpv_zposdif_w","Z pos dif",50, -25., 25.);
//TH1F* hpv_zposdif1_r = new TH1F("hpv_zposdif1_r","abs(Z(PU) - Z(tagtrack)) from B",50, 0., 25.);
//TH1F* hpv_zposdif1_w = new TH1F("hpv_zposdif1_w","abs(Z(PU) - Z(tagtrack)) from B",50, 0., 25.);
TH1F* hpv_zposdif1_r = new TH1F("hpv_zposdif1_r","abs(Z(PU) - Z(tagtrack)) tags",50, 0., 25.);
TH1F* hpv_zposdif1_w = new TH1F("hpv_zposdif1_w","abs(Z(PU) - Z(tagtrack)) tags",50, 0., 25.);
TH1F* hpv_ippubs_r = new TH1F("hpv_ippubs_r","ip wrt signal B",50, 0.0, 10.);
TH1F* hpv_ippubs_w = new TH1F("hpv_ippubs_w","ip wrt signal B",50, 0.0, 10.);
TH1F* hpv_ippuchi2bs_r = new TH1F("hpv_ippuchi2bs_r","ipChi2 wrt signal B",50, 0.0, 1000.);
TH1F* hpv_ippuchi2bs_w = new TH1F("hpv_ippuchi2bs_w","ipChi2 wrt signal B",50, 0.0, 1000.);
TH1F* hpv_zpos_r = new TH1F("hpv_zpos_r","Z pos of min ippu part",50, -120., 120.);
TH1F* hpv_zpos_w = new TH1F("hpv_zpos_w","Z pos of min ippu part",50, -120., 120.);
TH1F* hpv_zerrpos_r = new TH1F("hpv_zerrpos_r","Zerr pos of min ippu part",50, 0.0, 0.1);
TH1F* hpv_zerrpos_w = new TH1F("hpv_zerrpos_w","Zerr pos of min ippu part",50, 0.0, 0.1);
TH1F* hpv_difip_r = new TH1F("hpv_difip_r","dif ip-ippu tags", 50, 0., 15.);
TH1F* hpv_difip_w = new TH1F("hpv_difip_w","dif ip-ippu tags", 50, 0., 15.);
//TH1F* hpv_difip1_r = new TH1F("hpv_difip1_r","IP-min(IPPU) from B", 50, -15.0, 5);
//TH1F* hpv_difip1_w = new TH1F("hpv_difip1_w","IP-min(IPPU) from B", 50, -15.0, 5);
TH1F* hpv_difip1_r = new TH1F("hpv_difip1_r","IP-min(IPPU) tags", 50, -15.0, 5);
TH1F* hpv_difip1_w = new TH1F("hpv_difip1_w","IP-min(IPPU) tags", 50, -15.0, 5);
TH1F* hpv_ipmeandif_r = new TH1F("hpv_ipmeandif_r","dif ip ipmean tags", 50, 0.0, 7);
TH1F* hpv_ipmeandif_w = new TH1F("hpv_ipmeandif_w","dif ip ipmean tags", 50, 0.0, 7);
//TH1F* hpv_ipmeandif1_r = new TH1F("hpv_ipmeandif1_r","IP-<IPPU> from B", 50, -15.0, 5);
//TH1F* hpv_ipmeandif1_w = new TH1F("hpv_ipmeandif1_w","IP-<IPPU> from B", 50, -15.0, 5);
TH1F* hpv_ipmeandif1_r = new TH1F("hpv_ipmeandif1_r","IP-<IPPU> tags", 50, -15.0, 5);
TH1F* hpv_ipmeandif1_w = new TH1F("hpv_ipmeandif1_w","IP-<IPPU> tags", 50, -15.0, 5);


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

x1=0; x2=1;
TH1F* homphi_mu_r = new TH1F("homphi_mu_r","w(dphi)", 50, x1, x2);
TH1F* homphi_mu_w = new TH1F("homphi_mu_w","w(dphi)", 50, x1, x2);

TH1F* homphi_ele_r = new TH1F("homphi_ele_r","w(dphi)", 50, x1, x2);
TH1F* homphi_ele_w = new TH1F("homphi_ele_w","w(dphi)", 50, x1, x2);

TH1F* homphi_kO_r = new TH1F("homphi_kO_r","w(dphi)", 50, x1, x2);
TH1F* homphi_kO_w = new TH1F("homphi_kO_w","w(dphi)", 50, x1, x2);

TH1F* homphi_kS_r = new TH1F("homphi_kS_r","w(dphi)", 50, x1, x2);
TH1F* homphi_kS_w = new TH1F("homphi_kS_w","w(dphi)", 50, x1, x2);

TH1F* hv170 = new TH1F("hv170","hv170", 50, 0, 20);
TH1F* hv174_true = new TH1F("hv174_true","hv174_true", 50, 0, 3);
TH1F* hv175_true = new TH1F("hv175_true","hv175_true", 50, 0, .1);
TH1F* hv176_true = new TH1F("hv176_true","hv176_true", 50, 0, 3);
TH1F* hv174 = new TH1F("hv174","hv174", 50, 0, 3);
TH1F* hv175 = new TH1F("hv175","hv175", 50, 0, .1);
TH1F* hv176 = new TH1F("hv176","hv176", 50, 0, 3);


