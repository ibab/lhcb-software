DiMuonForXsection = {
    'BUILDERTYPE' : 'DiMuonForXsectionConf',
    'CONFIG' : {
    'TotalCuts'  :  """
    (MINTREE('mu+'==ABSID,PT)>0.65*GeV)
    & (VFASPF(VCHI2PDOF)<20)
    & ((ADMASS('J/psi(1S)')<120*MeV) | (ADMASS('psi(2S)')<120*MeV) | (MM>8.5*GeV))
    """,
    'Prescale'   :  1.,
    'CheckPV'    :  False
    },
    'STREAMS' : [ 'ICHEP' ] ,
    'WGs'    : [ 'BandQ' ]
    }
