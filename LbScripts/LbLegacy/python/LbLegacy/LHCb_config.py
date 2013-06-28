"""
 061213 - add HidValley and Hijing in Gauss_other_executables
 070111 - HLT application name is Hlt/HltApp
 070306 - remove AcerData
          add ANALYSIS, ORWELL and SQLDDB
 070329 - increase volume size of some projects
 070522 - add ALIGNMENT
 070703 - increase the AFS volume sizes for AMD64 addition
 070802 - added size for Online and modified a few quota
"""
#-----------------------------------------------------------------------------


data_files = {'XmlDDDB':'DBASE_Det_XmlDDDB',
                'DecFiles':'DBASE_Gen_DecFiles',
                'FieldMap':'DBASE_FieldMap',
                'BooleDQ':'DBASE_DQ_BooleDQ',
                'BrunelDQ':'DBASE_DQ_BrunelDQ',
                'ParamFiles':'PARAM_ParamFiles',
                'XmlConditions':'DBASE_Det_XmlConditions',
                'SQLDDDB':'DBASE_Det_SQLDDDB',
                'BcVegPyData':'PARAM_BcVegPyData',
                'MCatNLOData':'PARAM_MCatNLOData',
                'LHCbBkg':'PARAM_LHCbBkg',
                'Geant4Files':'PARAM_Geant4Files',
                'MIBData':'PARAM_MIBData',
                'L0TCK': 'DBASE_TCK_L0TCK',
                'HltTCK': 'DBASE_TCK_HltTCK',
                'AppConfig': 'DBASE_AppConfig',
                'Emacs': 'TOOLS_Tools_Emacs'}

ext_lhcb = ['LCGCMT', 'GENSER', 'OpenScientist']

lcg_projects = ['LCGCMT', 'SEAL', 'POOL', 'GENSER', 'COOL', 'GDML', 'CORAL', 'RELAX']

lhcb_projects = {'GAUDI':'Gaudi',
                   'LHCB':'LHCb',
                   'LBCOM':'Lbcom',
                   'REC':'Rec',
                   'HLT':'Hlt',
                   'PHYS':'Phys',
                   'ONLINE':'Online',
                   'ALIGNMENT':'Alignment',
                   'MOORE':'Moore',
                   'EULER':'Euler',
                   'VETRA':'Vetra',
                   'VETRATB':'VetraTB',
                   'GEANT4':'Geant4',
                   'GAUSS':'Gauss',
                   'BOOLE':'Boole',
                   'BRUNEL':'Brunel',
                   'DAVINCI':'DaVinci',
                   'BENDER':'Bender',
                   'ANALYSIS':'Analysis',
                   'ORWELL':'Orwell',
                   'PANORAMIX':'Panoramix',
                   'LBSCRIPTS':'LbScripts',
                   'CURIE':'Curie',
                   'PANOPTES':'Panoptes',
                   'DIRAC':'Dirac',
                   'LHCBGRID':'LHCbGrid',
                   'VANDERMEER': 'VanDerMeer',
                   'LHCBDIRAC': 'LHCbDirac',
                   'INTEGRATION': 'Integration',
                   'NOETHER': 'Noether',
                   'TESLA': 'Tesla'}

applications = {'GAUSS':'Sim/Gauss',
                'BOOLE':'Digi/Boole',
                'BRUNEL':'Rec/Brunel',
                'DAVINCI':'Phys/DaVinci',
                'MOORE':'Hlt/Moore',
                'EULER':'Trg/Euler',
                'ALIGNMENT':'Alignment/Escher',
                'ORWELL':'Calib/Orwell',
                'PANOPTES': 'Rich/Panoptes',
                'PANORAMIX':'Vis/Panoramix',
                'BENDER':'Phys/Bender',
                'LHCB':'Ex/',
                'GAUDI':'GaudiExamples'}

lhcb_binary = ['slc4_ia32_gcc34_dbg', 'slc4_ia32_gcc34',
               'slc4_ia32_gcc345_dbg', 'slc4_ia32_gcc345',
               'slc4_amd64_gcc34_dbg', 'slc4_amd64_gcc34',
               'slc3_ia32_gcc323_dbg', 'slc3_ia32_gcc323',
               'x86_64-slc5-gcc34-opt', 'x86_64-slc5-gcc34-dbg',
               'x86_64-slc5-gcc43-opt', 'x86_64-slc5-gcc43-dbg',
               'win32_vc71_dbg', 'win32_vc71',
               'osx104_ia32_gcc401_dbg', 'osx104_ia32_gcc401']

lhcb_vol_size = {'GAUDI':'3000000',
                 'LHCB' :'8000000',
                 'REC'  :'6000000',
                 'HLT'  :'2500000',
                 'PHYS' :'2000000',
                 'LBCOM':'1500000',
                 'ALIGNMENT':'800000',
                 'MOORE':'700000',
                 'EULER':'6000',
                 'BRUNEL':'700000',
                 'GAUSS' :'700000',
                 'GEANT4':'2000000',
                 'BOOLE' :'1500000',
                 'DAVINCI':'800000',
                 'BENDER':'500000',
                 'ANALYSIS':'2000000',
                 'ORWELL':'50000',
                 'CURIE':'50000',
                 'PANOPTES' : '50000',
                 'PANORAMIX':'1000000',
                 'ONLINE':'1500000',
                 'DIRAC':'500000',
                 'LHCbGrid': '50000',
                 'VanDerMeer': '50000',
                 'LHCbDirac': '50000',
                 'Integration': '50000'}

lhcb_base_name = {'GAUDI' : 'LCGCMT',
                    'LHCB'  : 'GAUDI',
                    'BENDER': 'DAVINCI',
                    'GEANT4': 'GAUDI'   }


applications_other_executables = {'GAUSS':'BcVegPy,Herwig,HidValley,Hijing'}
