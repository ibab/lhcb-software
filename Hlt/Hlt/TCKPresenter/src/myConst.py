
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## the replace rules for the save naming
## keep in mind: the order is very important because a '_' appear in the codsignes as well.
REPLACE_RULES = [('_','_001_','_'),
                 (',','_002_','&sbquo;'),
                 ('>','_003_','&gt;'),
                 ('<','_004_','&lt;'),
                 ('&','_005_','&amp;')]

## NODE_TYPE                = [BAR, BARCOLOR , BORDER, BGCOLOR  , MAINFONTSIZE, SECONDARYFONTSIZE, SIGNNAMES, CELLPADDING  ]
NODE_PLAIN                  = ['n', '#ffffff', '0'   , '#ffffff', '12'        , '8'             , ''      , '7'    ]
NODE_RARE                   = ['n', '#ffffff', '1'   , '#e0ffff', '12'        , '8'             , ''      , '7'    ]
NODE_SCALERLIKE             = ['n', '#ffffff', '0'   , '#ffffff', '12'        , '8'             , ''      , '7'    ]
NODE_CUT                    = ['n', '#ffffff', '1'   , '#eee8aa', '12'        , '8'             , ''      , '7'    ]
NODE_TECHNICAL              = ['y', '#000000', '1'   , '#e6e6fa', '12'        , '8'             , 'out'   , '7'    ]
NODE_PHYSICS                = ['y', '#000000', '1'   , '#f0fff0', '12'        , '8'             , 'out'   , '7'    ]
NODE_SPECIAL                = ['y', '#000000', '1'   , '#ffefd5', '12'        , '8'             , 'out'   , '7'    ]
NODE_CUT_WITHOUTRATE        = ['f', '#ffffff', '1'   , '#eee8aa', '12'        , '8'             , ''      , '7'    ]
NODE_TECHNICAL_WITHOUTRATE  = ['y', '#000000', '1'   , '#e6e6fa', '12'        , '8'             , ''      , '7'    ]
NODE_PHYSICS_WITHOUTRATE    = ['y', '#000000', '1'   , '#f0fff0', '12'        , '8'             , ''      , '7'    ]
NODE_SPECIAL_WITHOUTRATE    = ['y', '#000000', '1'   , '#ffefd5', '12'        , '8'             , ''      , '7'    ]
LABEL_PLAIN1                = ['y', '#000000', '1'   , '#f0fff0', '12'        , '8'             , ''      , '7'    ]
LABEL_PLAIN2                = ['y', '#000000', '1'   , '#e0eee0', '12'        , '8'             , ''      , '7'    ]

alley_s_p = ["Lumi","XPress","Velo",   "PA",   "L0","Hadron","SingleMuon","DiMuon","MuonTrack","Electron","Photon"]
alley_type               = {}
alley_type["PA"]         = NODE_SPECIAL
alley_type["L0"]         = NODE_PHYSICS
alley_type["Hadron"]     = NODE_PHYSICS
alley_type["SingleMuon"] = NODE_PHYSICS
alley_type["DiMuon"]     = NODE_PHYSICS
alley_type["MuonTrack"]  = NODE_PHYSICS
alley_type["Electron"]   = NODE_PHYSICS
alley_type["Photon"]     = NODE_PHYSICS
alley_type["XPress"]     = NODE_TECHNICAL
alley_type["Velo"]       = NODE_TECHNICAL
alley_type["Lumi"]       = NODE_TECHNICAL

alley_type_withoutrate               = {}
alley_type_withoutrate["PA"]         = NODE_SPECIAL_WITHOUTRATE
alley_type_withoutrate["L0"]         = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["Hadron"]     = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["SingleMuon"] = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["DiMuon"]     = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["MuonTrack"]  = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["Electron"]   = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["Photon"]     = NODE_PHYSICS_WITHOUTRATE
alley_type_withoutrate["XPress"]     = NODE_TECHNICAL_WITHOUTRATE
alley_type_withoutrate["Velo"]       = NODE_TECHNICAL_WITHOUTRATE
alley_type_withoutrate["Lumi"]       = NODE_TECHNICAL_WITHOUTRATE

alley_realname = {}
alley_realname["PA"] = "PA"
alley_realname["L0"] = "L0"
alley_realname["XPress"] = "XPress"
alley_realname["Hadron"] = "Hadron"
alley_realname["SingleMuon"] = "Muon"
alley_realname["DiMuon"] = "DiMuon"
alley_realname["MuonTrack"] = "MuTrack"
alley_realname["Velo"] = "Velo"
alley_realname["Electron"] = "Electron"
alley_realname["Photon"] = "Photon"
alley_realname["Lumi"] = "Lumi"

## first component of the 2-tupel should be part of the line, the second should not!
## these tupels are not real regular expressions they are used as regex later in the follwing applications.
alley_regex = {}
alley_regex["PA"] = ("Physics|Random|Incident",'Hlt1L0')
alley_regex["L0"] = ("Hlt1L0",'')
alley_regex["XPress"] = ("Hlt1XPress",'Hlt1L0')
alley_regex["Hadron"] = ("Hadron",'Hlt1L0')       
alley_regex["SingleMuon"] = ("Hlt1Muon|Hlt1SingleMuon",'Hlt1L0')
alley_regex["DiMuon"] = ("Hlt1DiMuon",'Hlt1L0')
alley_regex["MuonTrack"] = ("Hlt1MuTrack",'Hlt1L0')
alley_regex["Velo"] = ("Hlt1Velo",'Hlt1L0')
alley_regex["Electron"] = ("Electron",'Hlt1L0')
alley_regex["Photon"] = ("Photon",'Hlt1L0')
alley_regex["Lumi"] = ("Hlt1Lumi",'Hlt1L0')

