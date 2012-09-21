from Gaudi.Configuration import *
importOptions( "$DECFILESROOT/options/SwitchOffAllPythiaProcesses.opts" )
from Configurables import Generation, Special, FixedNInteractions, ToolSvc, EvtGenDecay, AlpGenProduction
gen = Generation()

gen.SampleGenerationTool = "Special"
gen.addTool( Special )
gen.Special.ProductionTool = "AlpGenProduction"
gen.Special.addTool( AlpGenProduction )
gen.Special.AlpGenProduction.nevxiter = 10000
gen.Special.AlpGenProduction.niter = 2
gen.Special.AlpGenProduction.nwgtev = 100000
gen.Special.AlpGenProduction.FileLabel = "monfichier"
gen.Special.AlpGenProduction.njets = 0  ##number of light jets
gen.Special.AlpGenProduction.etalmin = 1.5
gen.Special.AlpGenProduction.etalmax = 6
gen.Special.AlpGenProduction.etabmin = 1.5
gen.Special.AlpGenProduction.etabmax = 6
gen.Special.AlpGenProduction.mllmin = 50
gen.Special.AlpGenProduction.mllmax = 200
gen.Special.AlpGenProduction.ndns=9
gen.Special.AlpGenProduction.ptlmin=5
gen.Special.AlpGenProduction.ptbmin=5
gen.Special.AlpGenProduction.drbmin=0.5
gen.Special.AlpGenProduction.drlmin=0.5


Generation().Special.CutTool = ""
Generation().PileUpTool = "FixedNInteractions"
Generation().addTool( FixedNInteractions )
Generation().FixedNInteractions.NInteractions = 1

######As a help, here is a copy of the available options (from AlpGenProduction.cpp)##############
##  declareProperty( "FileLabel" , m_fileLabel = "Zbb" );
##   declareProperty( "nevxiter", m_nevxiter = 10000, 
##                    "number of events per iteration to generate alpgen's phase space grid in alpgen mode 1. Do tests!");
##   declareProperty( "niter", m_niter = 2, "number of iterations to generate alpgen's phase space grid in alpgen mode 1.");
##   declareProperty( "nwgtev", m_nwgtev = 100000, "number of weighted events to generate by alpgen in mode 1.  Do tests!");
##   declareProperty( "ihrd" , m_ihrd = 2 , "Hard process code : 1:WQQ - 2:ZQQ - 7:QQQQ 13:top" ); 
##   declareProperty( "ndns" , m_ndns = 9 , "parton density set (cf alpgen for codes) 9: CTEQ6L1 5: CTEQ5L");
##   declareProperty( "iqopt" , m_iqopt = 1 , "scale option (process dependent)");
##   declareProperty( "qfac" , m_qfac =  1.00, " Q scale rescaling factor");
##   declareProperty( "ickkw" , m_ickkw = 0 , "0 : no matching of Matrix Elements and Parton Shower - 1: matching (mlm scheme)");
##   declareProperty( "ktfac" , m_ktfac = 1.00 , " scale factor for ckkw alphas scale");
##   declareProperty( "njets" , m_njets = 0, "number of light jets");
##   declareProperty( "ihvy" , m_ihvy = 5 , "heavy flavour type for procs like WQQ, ZQQ, 2Q, etc(4=c, 5=b, 6=t)");
##   declareProperty( "ihvy2" , m_ihvy2 = 5 , "2nd heavy flavour type for procs like 4Q");
##   declareProperty( "mc" , m_mc =  1.50, "charm mass");
##   declareProperty( "mb" , m_mb = 4.70 , "bottom mass");
##   //top mass from Frederic Deliot's LHC-Tevatron combination at ICHEP 2012 (173.1+-0.9 GeV/c2):");
##   declareProperty( "mt" , m_mt = 173.1, "top mass" ); 
##   declareProperty( "ptjmin" , m_ptjmin = 1.0 , "minimum pt for light jets");
##   declareProperty( "ptbmin" , m_ptbmin = 1.0 , " ptmin for bottom quarks (in procs with explicit b)");
##   declareProperty( "ptcmin" , m_ptcmin = 1.0 , " ptmin for charm quarks (in procs with explicit c)");
##   declareProperty( "ptlmin" , m_ptlmin = 1.0 , "minimum pt for charged leptons");
##   declareProperty( "etajmax" , m_etajmax = 100.0 , "max|eta| for light jets");
##   declareProperty( "etabmax" , m_etabmax = 100.0 , " max|eta| for b quarks (in procs with explicit b)");
##   declareProperty( "etacmax" , m_etacmax = 100.0, "max|eta| for c quarks (in procs with explicit c)");
##   declareProperty( "etalmax" , m_etalmax = 100.0 , "max abs(eta) for charged leptons");
##   declareProperty( "etalmin" , m_etalmin = -100.0 , "min eta for charged leptons");
##   declareProperty( "etabmin" , m_etabmin = -100.0 , "min eta for b quarks  ");
##   declareProperty( "eta1lmin" , m_eta1lmin = -100.0 , "min eta for at least 1 charged lepton");
##   declareProperty( "eta1bmin" , m_eta1bmin = -100.0 , "min eta for at least 1 b quark  ");
##   declareProperty( "drjmin" , m_drjmin = 0.10 , " min deltaR(j-j), deltaR(Q-j) [j=light jet, Q=c/b]");
##   declareProperty( "drbmin" , m_drbmin = 0.10 , "min deltaR(b-b) (procs with explicit b)");
##   declareProperty( "drcmin" , m_drcmin = 0.10 , "min deltaR(c-c) (procs with explicit c)");
##   declareProperty( "drlmin" , m_drlmin = 0.0 , "min deltaR between charged lepton and light jets");
##   //declareProperty( "ilep" , m_ilep =  , "Z*/gamma fin state: 0=lept (1 family) 1=nu (3 fam)");
##   declareProperty( "mllmin" , m_mllmin = 5.0 , "min dilepton inv mass");
##   declareProperty( "mllmax" , m_mllmax = 1000.0 , "max dilepton inv mass");
##   declareProperty( "iwdecmode" , m_iwdecmode = 2 , "W decay modes, in imode=2 - 1:electron 2: muon");
##   declareProperty( "itdecmode" , m_itdecmode = 1 , "top (or t-tbar) decay modes, in imode=2");
##   declareProperty( "izdecmode" , m_izdecmode = 2 , "Z decay modes, in imode=2 - 1:electron 2: muon");
##   declareProperty( "xlclu" , m_xlclu = -1.0 , "lambda value for ckkw alpha (match shower alpha) - default -1");
##   declareProperty( "lpclu" , m_lpclu =  -1.0, "loop order for ckkw alpha (match shower alpha) - default -1");
##  declareProperty( "cluopt" , m_cluopt =  1, "kt scale option. 1:kt propto pt, 2:kt propto mt");


