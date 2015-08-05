
# Generate Pile Up
from Gaudi.Configuration import *
from Configurables import Generation, Pythia8Production, Gauss

gen = Generation()

gen.Special.PileUpProductionTool = "Pythia8Production/Pythia8PileUp"
gen.PileUpTool = "FixedLuminosityForRareProcess"
gen.Special.addTool( Pythia8Production, name = "Pythia8PileUp" )
gen.Special.Pythia8PileUp.Tuning = "LHCbDefault.cmd"
gen.Special.ReinitializePileUpGenerator  = False

# AlpGen specific options
from Configurables import AlpGenProductionwqq, AlpGenProductionzbb, AlpGenProductiontop, AlpGenProduction4q

# AlpGen conf dict
try: AlpGenDict = Generation.AlpGenDict
except: raise NameError("AlpGenDict not defined")

# File type
try: alpgen_FileLabel = AlpGenDict["alpgen_FileLabel"]
except: raise NameError('value for alpgen_FileLabel property not set')
else:
    if alpgen_FileLabel=="wbb" or alpgen_FileLabel=="wcc":
        gen.Special.ProductionTool = "AlpGenProductionwqq"
        gen.Special.addTool( AlpGenProductionwqq )
        alpgen=gen.Special.AlpGenProductionwqq

    elif alpgen_FileLabel=="zbb" or alpgen_FileLabel=="zcc":
        gen.Special.ProductionTool = "AlpGenProductionzbb"
        gen.Special.addTool( AlpGenProductionzbb )
        alpgen=gen.Special.AlpGenProductionzbb

    elif "top" in alpgen_FileLabel:
        gen.Special.ProductionTool = "AlpGenProductiontop"
        gen.Special.addTool( AlpGenProductiontop )
        alpgen=gen.Special.AlpGenProductiontop

    elif "4" in alpgen_FileLabel:
        gen.Special.ProductionTool = "AlpGenProduction4q"
        gen.Special.addTool( AlpGenProduction4q )
        alpgen=gen.Special.AlpGenProduction4q
    
    alpgen.FileLabel = alpgen_FileLabel

## Global properties
try: alpgen_nevxiter = AlpGenDict['alpgen_nevxiter']
except: raise NameError('value for alpgen_nevxiter property not set')
else: alpgen.nevxiter = alpgen_nevxiter

try: alpgen_niter = AlpGenDict['alpgen_niter']
except: raise NameError('value for alpgen_niter property not set')
else: alpgen.niter = alpgen_niter

try: alpgen_nwgtev = AlpGenDict['alpgen_nwgtev']
except: raise NameError('value for alpgen_nwgtev property not set')
else: alpgen.nwgtev = alpgen_nwgtev

try: alpgen_njets = AlpGenDict['alpgen_njets']
except: raise NameError('value for alpgen_njets property not set')
else: alpgen.njets = alpgen_njets

try: alpgen_ndns = AlpGenDict['alpgen_ndns']
except: raise NameError('value for alpgen_ndns property not set')
else: alpgen.ndns = alpgen_ndns


## Specific type properties
if alpgen_FileLabel=='4b' or alpgen_FileLabel=='4q' or alpgen_FileLabel=='wbb' or alpgen_FileLabel=='wcc' or alpgen_FileLabel=='zbb' or alpgen_FileLabel=='zcc':
    try: alpgen_etabmin = AlpGenDict['alpgen_etabmin']
    except: raise NameError('value for alpgen_etabmin property not set')
    else: alpgen.etabmin = alpgen_etabmin
    
if alpgen_FileLabel=='top' or alpgen_FileLabel=='wbb' or alpgen_FileLabel=='wcc' or alpgen_FileLabel=='zbb' or alpgen_FileLabel=='zcc':
    try: alpgen_drlmin = AlpGenDict['alpgen_drlmin']
    except: raise NameError('value for alpgen_drlmin property not set')
    else: alpgen.drlmin = alpgen_drlmin
    
if alpgen_FileLabel=='4b' or alpgen_FileLabel=='4q' or alpgen_FileLabel=='top' or alpgen_FileLabel=='wbb' or alpgen_FileLabel=='zbb':
    try: alpgen_ptbmin = AlpGenDict['alpgen_ptbmin']
    except: raise NameError('value for alpgen_ptbmin property not set')
    else: alpgen.ptbmin = alpgen_ptbmin

    try: alpgen_drbmin = AlpGenDict['alpgen_drbmin']
    except: raise NameError('value for alpgen_drbmin property not set')
    else: alpgen.drbmin = alpgen_drbmin
    
    try: alpgen_etabmax = AlpGenDict['alpgen_etabmax']
    except: raise NameError('value for alpgen_etabmax property not set')
    else: alpgen.etabmax = alpgen_etabmax
    
if alpgen_FileLabel=='wbb' or alpgen_FileLabel=='wcc' or alpgen_FileLabel=='zbb' or alpgen_FileLabel=='zcc':
    try: alpgen_eta1lmin = AlpGenDict['alpgen_eta1lmin']
    except: raise NameError('value for alpgen_eta1lmin property not set')
    else: alpgen.eta1lmin = alpgen_eta1lmin
    
    try: alpgen_ptlmin = AlpGenDict['alpgen_ptlmin']
    except: raise NameError('value for alpgen_ptlmin property not set')
    else: alpgen.ptlmin = alpgen_ptlmin

    try: alpgen_etalmax = AlpGenDict['alpgen_etalmax']
    except: raise NameError('value for alpgen_etalmax property not set')
    else: alpgen.etalmax = alpgen_etalmax

if alpgen_FileLabel=='4b' or alpgen_FileLabel=='wbb' or alpgen_FileLabel=='wcc' or alpgen_FileLabel=='zcc':
    try: alpgen_ihvy = AlpGenDict['alpgen_ihvy']
    except: raise NameError('value for alpgen_ihvy property not set')
    else: alpgen.ihvy = alpgen_ihvy

if alpgen_FileLabel=='top' or alpgen_FileLabel=='wbb' or alpgen_FileLabel=='wcc':
    try: alpgen_iwdecmode = AlpGenDict['alpgen_iwdecmode']
    except: raise NameError('value for alpgen_iwdecmode property not set')
    else: alpgen.iwdecmode = alpgen_iwdecmode

if alpgen_FileLabel=='zbb' or alpgen_FileLabel=='zcc':
    try: alpgen_izdecmode = AlpGenDict['alpgen_izdecmode']
    except: raise NameError('value for alpgen_izdecmode property not set')
    else: alpgen.izdecmode = alpgen_izdecmode

if alpgen_FileLabel=='wcc' or alpgen_FileLabel=='zcc':
    try: alpgen_ptcmin = AlpGenDict['alpgen_ptcmin']
    except: raise NameError('value for alpgen_ptcmin property not set')
    else: alpgen.ptcmin = alpgen_ptcmin

    try: alpgen_drcmin = AlpGenDict['alpgen_drcmin']
    except: raise NameError('value for alpgen_drcmin property not set')
    else: alpgen.drcmin = alpgen_drcmin

    try: alpgen_etacmax = AlpGenDict['alpgen_etacmax']
    except: raise NameError('value for alpgen_etacmax property not set')
    else: alpgen.etacmax = alpgen_etacmax

if alpgen_FileLabel=='top':
    try: alpgen_itopprc = AlpGenDict['alpgen_itopprc']
    except: raise NameError('value for alpgen_itopprc property not set')
    else: alpgen.itopprc = alpgen_itopprc

    try: alpgen_itdecmode = AlpGenDict['alpgen_itdecmode']
    except: raise NameError('value for alpgen_itdecmode property not set')
    else: alpgen.itdecmode = alpgen_itdecmode
    
    try: alpgen_mt = AlpGenDict['alpgen_mt']
    except: raise NameError('value for alpgen_mt property not set')
    else: alpgen.mt = alpgen_mt

    try: alpgen_ptjmin = AlpGenDict['alpgen_ptjmin']
    except: raise NameError('value for alpgen_ptjmin property not set')
    else: alpgen.ptjmin = alpgen_ptjmin

    try: alpgen_drjmin = AlpGenDict['alpgen_drjmin']
    except: raise NameError('value for alpgen_drjmin property not set')
    else: alpgen.drjmin = alpgen_drjmin

    try: alpgen_etajmax = AlpGenDict['alpgen_etajmax']
    except: raise NameError('value for alpgen_etajmax property not set')
    else: alpgen.etajmax = alpgen_etajmax

if alpgen_FileLabel=='4b':
    try: alpgen_ihvy2 = AlpGenDict['alpgen_ihvy2']
    except: raise NameError('value for alpgen_ihvy2 property not set')
    else: alpgen.ihvy2 = alpgen_ihvy2

    try: alpgen_pt1bmin = AlpGenDict['alpgen_pt1bmin']
    except: raise NameError('value for alpgen_pt1bmin property not set')
    else: alpgen.pt1bmin = alpgen_pt1bmin
    
    try: alpgen_pt2bmin = AlpGenDict['alpgen_pt2bmin']
    except: raise NameError('value for alpgen_pt2bmin property not set')
    else: alpgen.pt2bmin = alpgen_pt2bmin

    try: alpgen_eta1bmin = AlpGenDict['alpgen_eta1bmin']
    except: pass
    else: alpgen.eta1bmin = alpgen_eta1bmin

    try: alpgen_eta2bmin = AlpGenDict['alpgen_eta2bmin']
    except: pass
    else: alpgen.eta2bmin = alpgen_eta2bmin
    

