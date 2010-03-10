  //>>>>>>>>>>>>>>>>>>>>>>>>
  //DEFINE HERE YOUR HISTOS
  //>>>>>>>>>>>>>>>>>>>>>>>

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

  x1=1; x2=21  ; //ncands
  TH1F* hr_mu_ncand = new TH1F("hr_mu_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_mu_ncand = new TH1F("hw_mu_ncand","ncand wrong tags", 50, x1, x2);


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
  x1=1; x2=21  ; //ncands
  TH1F* hr_ele_ncand = new TH1F("hr_ele_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_ele_ncand = new TH1F("hw_ele_ncand","ncand wrong tags", 50, x1, x2);
  x1=0; x2=3; //ele veloch
  x1=0; x2=2; //ele EoverP

  //KAON OPPO
  x1=0; x2=5  ; //pt
  TH1F* hr_k_pt = new TH1F("hr_k_pt","pt right tags", 50, x1, x2);
  TH1F* hw_k_pt = new TH1F("hw_k_pt","pt wrong tags", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_k_p = new TH1F("hr_k_p","p right tags", 50, x1, x2);
  TH1F* hw_k_p = new TH1F("hw_k_p","p wrong tags", 50, x1, x2);
  x1=0; x2=50  ; //ip/s
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
  x1=1; x2=21  ; //ncands
  TH1F* hr_k_ncand = new TH1F("hr_k_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_k_ncand = new TH1F("hw_k_ncand","ncand wrong tags", 50, x1, x2);

  //KAON SAME
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

  x1=1; x2=21  ; //ncands
  TH1F* hr_kS_ncand = new TH1F("hr_kS_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_kS_ncand = new TH1F("hw_kS_ncand","ncand wrong tags", 50, x1, x2);
  x1=0, x2=5  ; //dphi

  //PION SAME
  x1=0; x2=5  ; //pt
  TH1F* hr_pS_pt = new TH1F("hr_pS_pt","pt right tags", 50, x1, x2);
  TH1F* hw_pS_pt = new TH1F("hw_pS_pt","pt wrong tags", 50, x1, x2);
  x1=0; x2=70 ; //p
  TH1F* hr_pS_p = new TH1F("hr_pS_p","p right tags", 50, x1, x2);
  TH1F* hw_pS_p = new TH1F("hw_pS_p","p wrong tags", 50, x1, x2);
  x1=0; x2=15  ; //ip/s
  TH1F* hr_pS_ips = new TH1F("hr_pS_ips","ips right tags", 50, x1, x2);
  TH1F* hw_pS_ips = new TH1F("hw_pS_ips","ips wrong tags", 50, x1, x2);
  x1=0; x2=100  ; //ippu
  TH1F* hr_pS_ippu = new TH1F("hr_pS_ippu","ippu right tags", 50, x1, x2);
  TH1F* hw_pS_ippu = new TH1F("hw_pS_ippu","ippu wrong tags", 50, x1, x2);

  x1=0; x2=5   ; //LCS
  TH1F* hr_pS_lcs = new TH1F("hr_pS_lcs","lcs right tags", 50, x1, x2);
  TH1F* hw_pS_lcs = new TH1F("hw_pS_lcs","lcs wrong tags", 50, x1, x2);
  x1=-5, x2=95; // PID
  TH1F* hr_pS_pid = new TH1F("hr_pS_pid","pid right tags", 50, x1, x2);
  TH1F* hw_pS_pid = new TH1F("hw_pS_pid","pid wrong tags", 50, x1, x2);
  x1=-35; x2=0 ;// tsal ghost likel
  TH1F* hr_pS_tsal = new TH1F("hr_pS_tsal","tsal right tags", 50, x1, x2);
  TH1F* hw_pS_tsal = new TH1F("hw_pS_tsal","tsal wrong tags", 50, x1, x2);

  x1=0; x2=100;//N mult
  TH1F* hr_pS_mult = new TH1F("hr_pS_mult","mult right tags", 50, x1, x2);
  TH1F* hw_pS_mult = new TH1F("hw_pS_mult","mult wrong tags", 50, x1, x2);

  x1=1; x2=21  ; //ncands
  TH1F* hr_pS_ncand = new TH1F("hr_pS_ncand","ncand right tags", 50, x1, x2);
  TH1F* hw_pS_ncand = new TH1F("hw_pS_ncand","ncand wrong tags", 50, x1, x2);

  x1=0; x2=1;
  TH1F* homphi_mu_r = new TH1F("homphi_mu_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_mu_w = new TH1F("homphi_mu_w","w(dphi)", 50, x1, x2);

  TH1F* homphi_kO_r = new TH1F("homphi_kO_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_kO_w = new TH1F("homphi_kO_w","w(dphi)", 50, x1, x2);

  TH1F* homphi_kS_r = new TH1F("homphi_kS_r","w(dphi)", 50, x1, x2);
  TH1F* homphi_kS_w = new TH1F("homphi_kS_w","w(dphi)", 50, x1, x2);

  TH1F* hrefit1 = new TH1F("hrefit1","hrefit1", 50, -1, 2);
  TH1F* hrefit2 = new TH1F("hrefit2","hrefit2", 50, -1, 2);


  x1=3; x2=8;
  TH1F* hright = new TH1F("hright","right tags", 50, x1, x2);
  TH1F* hwrong = new TH1F("hwrong","wrong tags", 50, x1, x2);
  TH1F* heffec ;//= new TH1F("heffec","effective efficiency(cut)", 50, x1, x2);
  TString direction="left2right";





  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //>>>>>>>>>>>>>>>>>>>>>>>>//create tree of nnet variables:
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


  //input data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  TString froot = readString("datafile");
  std::vector<TString> filelist(0);
  const char* astr1 = froot.Data();
  const string astr2(astr1);
  filelist = getFiles(astr2);
  if(filelist.size()==0) filelist.push_back(froot);

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  gROOT->Reset();
  TString rootsubdir = "";
  TFile fhisto("tag.root","RECREATE");
  if(rootsubdir != "") { fhisto.mkdir(rootsubdir); fhisto.cd(rootsubdir); }

  //>>>>>>>>>>>>>>>>>>>>>>>>//>>>>>>>>>>>>>>>>>>>>>>>> inits
  int nB531=0, nB521=0, nB511=0, nBother=0, nBanti=0, ntrig=0;
  nsele=nlm=nllm=nle=nlle=nlk=nlkS=nllk=nllkS=nidm=nide=nidk=nidkS=
    nghost_m=nghost_e=nghost_k=nghost_kS=0; 
  for(int i=0; i<20; ++i) {nrt[i]=0; nwt[i]=0; nrtag[i]=0; nwtag[i]=0;}
  std::vector<TString>::iterator ifile; int nfile = 0;

  //read cuts and properties from file:
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  TString NNetTrain = readString("NNetTrain");
  TString mlptype   = readString("mlptype");

  int trigHlt2, trigHlt1, trigL0;
  askL0  =(int)read("askL0");
  askHlt1=(int)read("askHlt1");
  askHlt2=(int)read("askHlt2");

  float Nmax=read("Nmax"); if(Nmax==-1) Nmax=10000000;

  float param  = read("param");
  if(param != -1) 
    cout<<BOLD<<ROJO<<"Using internal param = "<<param<<ENDC<<endl;
  initmva_mu=initmva_e=initmva_k=initmva_kS=initmva_pS=true;

  bool IsControlChannel = read("IsControlChannel"); 
  bool cheatID = read("cheatID");
  bool cheatP  = read("cheatP");     
  bool useDV   = read("useDV");     
  DBG          = (bool)read("DBG");     

  if(useDV)cout<<ROJO<<"Using results of DaVinci src/FlavourTagging!\n"<<ENDC;
  cout<<VERDE<<"Using "<<mlptype
  <<", Triggers required: "<<ENDC<<SUBVERDE
  <<(askL0?" L0 ":"")
  <<(askHlt1?" Hlt1 ":"")
  <<(askHlt2?" Hlt2 ":"")
  <<(( !askL0 && !askHlt1 && !askHlt2)?" none ":"")
  <<ENDC<<endl;

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //Taggers common selection cuts 
  float IPPU_cut   = read("IPPU_cut");
  float distPhi_cut= read("distPhi_cut");
  //dilution binning range for omega combination
  float  ProbMin     = read("ProbMin");
  double omegamaxbin = read("omegamaxbin");
  double omegascale  = read("omegascale");

  //muons -------------
  float Muon_Pt_cut= read("Muon_Pt_cut"), Muon_P_cut = read("Muon_P_cut"); 
  float lcs_m_cut  = read("lcs_m_cut");
  float PID_m_cut  = read("PID_m_cut");

  //elect -------------
  float Ele_Pt_cut     = read("Ele_Pt_cut"),    Ele_P_cut     = read("Ele_P_cut");
  float VeloChargeMin  = read("VeloChargeMin"), VeloChargeMax = read("VeloChargeMax");
  float Ele_EoverP_cut = read("Ele_EoverP_cut");
  float lcs_e_cut      = read("lcs_e_cut"); 
  float Ele_ghost_cut  = read("Ele_ghost_cut");
  float PID_e_cut     =  read("PID_e_cut");

  //OS kaons -------------
  float Kaon_Pt_cut= read("Kaon_Pt_cut"), Kaon_P_cut= read("Kaon_P_cut"), 
        Kaon_IP_cut= read("Kaon_IP_cut"), Kaon_IPRAW_cut= read("Kaon_IPRAW_cut");
  float lcs_k_ups_cut = read("lcs_k_ups_cut"), lcs_k_long_cut= read("lcs_k_long_cut"), 
        Kaon_ghost_prob_cut= read("Kaon_ghost_prob_cut");
  float PID_k_cut    = read("PID_k_cut"), 
        PID_kp_cut   = read("PID_kp_cut");

  //cuts x Same Side tagging Kaons -------------
  float KaonSame_Pt_cut= read("KaonSame_Pt_cut"), KaonSame_P_cut = read("KaonSame_P_cut"), 
        KaonSame_IP_cut = read("KaonSame_IP_cut");
  float KaonSame_Phi_cut= read("KaonSame_Phi_cut"), KaonSame_Eta_cut= read("KaonSame_Eta_cut"), 
        KaonSame_dQ_cut = read("KaonSame_dQ_cut");
  float lcs_kS_cut = read("lcs_kS_cut");
  float PID_kS_cut = read("PID_kS_cut"), PID_kpS_cut = read("PID_kpS_cut");

  //cuts x Same Side tagging Pions -------------
  float PionSame_Pt_cut = read("PionSame_Pt_cut"), 
        PionSame_P_cut  = read("PionSame_P_cut"), PionSame_IP_cut= read("PionSame_IP_cut");
  float PionSame_dQ_cut = read("PionSame_dQ_cut"), PionSame_dQ2_cut= read("PionSame_dQ2_cut");
  float PionSame_PIDNoK_cut = read("PionSame_PIDNoK_cut"), 
        PionSame_PIDNoP_cut = read("PionSame_PIDNoP_cut");
  float Pion_ghost_prob_cut = read("Pion_ghost_prob_cut");
  float lcs_pS_cut  = read("lcs_pS_cut");
  float ProbMin_pion= read("ProbMin_pion");

  //weight of Vch in tagging decision -------------
  float VchPowerK= read("VchPowerK"), MinimumVCharge= read("MinimumVCharge");
  float m_P0 = read("m_P0"), m_P1 = read("m_P1"), m_Gt075 = read("m_Gt075");
  float wSameSign2 = read("wSameSign2"), wSameSignMoreThan2 = read("wSameSignMoreThan2");
  float ProbMinVtx = read("ProbMinVtx");
  //>>>>>>>>>>>>>>>>>>>>>>>>
