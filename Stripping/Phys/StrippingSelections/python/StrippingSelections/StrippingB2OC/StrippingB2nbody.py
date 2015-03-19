# $Id: StrippingB2nbody.py,v 1.0 2011-02-8 12:53:17 roldeman Exp $
__author__ = ['Rudolf Oldeman']
__date__ = '08/02/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2nbodyConf',
           'makeB2nbody',
           'makePi',
           'makeK',
           'makep', 
           'makeKs',
           'makeLm',
           'makeDz',
           'makeDp',
           'makeDs',
           'makeLc',
           'makePh',
           'makeKS',
           'makeJp',
           'makeDS',
           'default_config')


default_config ={
  'B2twobody': {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':                2,
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'prescale':           1.0,
                  'MaxTrLong':           70,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MaxPiGHP':           0.3,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MaxKGHP':            0.3,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxpGHP':            0.3,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'B2OC' ]
    },

 'B2threebody' : {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':                3,
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'prescale':           1.0,
                  'MaxTrLong':           70,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MaxPiGHP':           0.3,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MaxKGHP':            0.3,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxpGHP':            0.3,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'B2OC' ]
    },

  'B2fourbody': {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : { 'nbody':                 4,
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'prescale':           1.0,
                  'MaxTrLong':           70,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MaxPiGHP':           0.3,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MaxKGHP':            0.3,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxpGHP':            0.3,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'B2OC' ]
    }    
}

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection, DataOnDemand, VoidEventSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class B2nbodyConf(LineBuilder) :
    __configuration_keys__ = ('nbody','MinBMass','MaxBMass','MinBPt','MaxBVertChi2DOF','MinBPVVDChi2','MaxBPVIPChi2','MinBPVDIRA','MaxMass','MaxNtrk','MinNvc',
                              'doPi','doK','dop','doKs','doLm','doDz','doDp','doDs','doLc','doPh','doKS','doJp','doDS','prescale','MaxTrLong',
                              'MinPiPt','MinPiIPChi2DV','MaxPiChi2','MinPiPIDK','MinPiPIDp', 'MaxPiGHP',
                              'MinKPt','MinKIPChi2DV','MaxKChi2','MinKPIDPi','MinKPIDp', 'MaxKGHP',
                              'MinpPt','MinpIPChi2DV','MaxpChi2','MinpPIDPi','MinpPIDK', 'MaxpGHP',
                              'MaxKsDeltaM','MinKsPt','MaxKsVertChi2DOF','MinKsPVVDChi2','MinKsIPChi2','MinKsDauPt','MinKsDauIPChi2','MaxKsDauTrkChi2',
                              'MaxLmDeltaM','MinLmPt','MaxLmVertChi2DOF','MinLmPVVDChi2','MinLmIPChi2','MinLmPrtPt','MinLmPiPt','MinLmPrtIPChi2','MinLmPiIPChi2','MaxLmPrtTrkChi2','MaxLmPiTrkChi2',
                              'MaxDzDeltaM','MinDzPt','MaxDzVertChi2DOF','MinDzPVVDChi2','MinDzIPChi2','MinDzDauPt','MinDzDauIPChi2','MaxDzDauTrkChi2','MinDzKPIDPi','MinDzPiPIDK',
                              'MaxDpDeltaM','MinDpPt','MaxDpVertChi2DOF','MinDpPVVDChi2','MinDpIPChi2','MinDpDauPt','MinDpDauIPChi2','MaxDpDauTrkChi2','MinDpKPIDPi','MinDpPiPIDK',
                              'MaxDsDeltaM','MinDsPt','MaxDsVertChi2DOF','MinDsPVVDChi2','MinDsIPChi2','MinDsDauPt','MinDsDauIPChi2','MaxDsDauTrkChi2','MinDsKmPIDPi','MinDsKpPIDPi','MinDsPiPIDK',
                              'MaxLcDeltaM','MinLcPt','MaxLcVertChi2DOF','MinLcPVVDChi2','MinLcIPChi2','MinLcDauPt','MinLcDauIPChi2','MaxLcDauTrkChi2','MinLcKPIDPi','MinLcpPIDPi','MinLcpPIDK','MinLcPiPIDK',
                              'MaxPhDeltaM','MinPhPt','MaxPhVertChi2DOF','MinPhPVVDChi2','MinPhIPChi2','MinPhDauPt','MinPhDauIPChi2','MaxPhDauTrkChi2',
                              'MaxKSDeltaM','MinKSPt','MaxKSVertChi2DOF','MinKSPVVDChi2','MinKSIPChi2','MinKSDauPt','MinKSDauIPChi2','MaxKSDauTrkChi2','MinKSKPIDPi','MinKSPiPIDK',
                              'MaxJpDeltaM','MinJpPt','MaxJpVertChi2DOF','MinJpPVVDChi2','MinJpIPChi2','MinJpDauPt','MinJpDauIPChi2','MaxJpDauTrkChi2',
                              'MaxDSDeltaM','MinDSPt','MaxDSVertChi2DOF','MinDSPVVDChi2','MinDSIPChi2','MinDSPiPt','MinDSPiIPChi2','MaxDSPiTrkChi2','MaxDSD0DeltaM',
                              'MinDSD0Pt','MaxDSD0VertChi2DOF','MinDSD0PVVDChi2','MinDSD0IPChi2','MinDSD0DauPt','MinDSD0DauIPChi2','MaxDSD0DauTrkChi2')
    
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selPi = makePi( 'PiFor'+name,
                             MinPiPt      =config['MinPiPt'],
                             MinPiIPChi2DV=config['MinPiIPChi2DV'],
                             MaxPiChi2    =config['MaxPiChi2'],
                             MinPiPIDK    =config['MinPiPIDK'],
                             MinPiPIDp    =config['MinPiPIDp'], 
                             MaxPiGHP     =config['MaxPiGHP'])
        self.selK = makeK( 'KFor'+name,
                             MinKPt      =config['MinKPt'],
                             MinKIPChi2DV=config['MinKIPChi2DV'],
                             MaxKChi2    =config['MaxKChi2'],
                             MinKPIDPi   =config['MinKPIDPi'],
                             MinKPIDp    =config['MinKPIDp'],
                             MaxKGHP     =config['MaxKGHP'])
        self.selp = makep( 'pFor'+name,
                             MinpPt      =config['MinpPt'],
                             MinpIPChi2DV=config['MinpIPChi2DV'],
                             MaxpChi2    =config['MaxpChi2'],
                             MinpPIDPi   =config['MinpPIDPi'],
                             MinpPIDK    =config['MinpPIDK'],
                             MaxpGHP     =config['MaxpGHP'])
        self.selKs = makeKs('KsFor'+name,
                            MaxKsDeltaM     =config['MaxKsDeltaM'],    
                            MinKsPt         =config['MinKsPt'],        
                            MaxKsVertChi2DOF=config['MaxKsVertChi2DOF'],
                            MinKsPVVDChi2   =config['MinKsPVVDChi2'],  
                            MinKsIPChi2     =config['MinKsIPChi2'],
                            MinKsDauPt      =config['MinKsDauPt'],     
                            MinKsDauIPChi2  =config['MinKsDauIPChi2'], 
                            MaxKsDauTrkChi2 =config['MaxKsDauTrkChi2'])
        self.selLm = makeLm('LmFor'+name,
                            MaxLmDeltaM     =config['MaxLmDeltaM'],    
                            MinLmPt         =config['MinLmPt'],        
                            MaxLmVertChi2DOF=config['MaxLmVertChi2DOF'],
                            MinLmPVVDChi2   =config['MinLmPVVDChi2'],  
                            MinLmIPChi2     =config['MinLmIPChi2'],    
                            MinLmPrtPt      =config['MinLmPrtPt'],     
                            MinLmPiPt       =config['MinLmPiPt'],      
                            MinLmPrtIPChi2  =config['MinLmPrtIPChi2'], 
                            MinLmPiIPChi2   =config['MinLmPiIPChi2'],  
                            MaxLmPrtTrkChi2 =config['MaxLmPrtTrkChi2'],
                            MaxLmPiTrkChi2  =config['MaxLmPiTrkChi2'])
        self.selDz = makeDz('DzFor'+name,
                            MaxDzDeltaM     =config['MaxDzDeltaM'],   
                            MinDzPt         =config['MinDzPt'],        
                            MaxDzVertChi2DOF=config['MaxDzVertChi2DOF'],
                            MinDzPVVDChi2   =config['MinDzPVVDChi2'],  
                            MinDzIPChi2     =config['MinDzIPChi2'],    
                            MinDzDauPt      =config['MinDzDauPt'],     
                            MinDzDauIPChi2  =config['MinDzDauIPChi2'], 
                            MaxDzDauTrkChi2 =config['MaxDzDauTrkChi2'],
                            MinDzKPIDPi     =config['MinDzKPIDPi'],
                            MinDzPiPIDK     =config['MinDzPiPIDK'])
        self.selDp = makeDp('DpFor'+name,
                            MaxDpDeltaM     =config['MaxDpDeltaM'],   
                            MinDpPt         =config['MinDpPt'],        
                            MaxDpVertChi2DOF=config['MaxDpVertChi2DOF'],
                            MinDpPVVDChi2   =config['MinDpPVVDChi2'],  
                            MinDpIPChi2     =config['MinDpIPChi2'],    
                            MinDpDauPt      =config['MinDpDauPt'],     
                            MinDpDauIPChi2  =config['MinDpDauIPChi2'], 
                            MaxDpDauTrkChi2 =config['MaxDpDauTrkChi2'],
                            MinDpKPIDPi     =config['MinDpKPIDPi'],
                            MinDpPiPIDK     =config['MinDpPiPIDK'])
        self.selDs = makeDs('DsFor'+name,
                            MaxDsDeltaM     =config['MaxDsDeltaM'],   
                            MinDsPt         =config['MinDsPt'],        
                            MaxDsVertChi2DOF=config['MaxDsVertChi2DOF'],
                            MinDsPVVDChi2   =config['MinDsPVVDChi2'],  
                            MinDsIPChi2     =config['MinDsIPChi2'],    
                            MinDsDauPt      =config['MinDsDauPt'],     
                            MinDsDauIPChi2  =config['MinDsDauIPChi2'], 
                            MaxDsDauTrkChi2 =config['MaxDsDauTrkChi2'],
                            MinDsKmPIDPi    =config['MinDsKmPIDPi'],
                            MinDsKpPIDPi    =config['MinDsKpPIDPi'],
                            MinDsPiPIDK     =config['MinDsPiPIDK'])
        self.selLc = makeLc('LcFor'+name,
                            MaxLcDeltaM     =config['MaxLcDeltaM'],   
                            MinLcPt         =config['MinLcPt'],        
                            MaxLcVertChi2DOF=config['MaxLcVertChi2DOF'],
                            MinLcPVVDChi2   =config['MinLcPVVDChi2'],  
                            MinLcIPChi2     =config['MinLcIPChi2'],    
                            MinLcDauPt      =config['MinLcDauPt'],     
                            MinLcDauIPChi2  =config['MinLcDauIPChi2'], 
                            MaxLcDauTrkChi2 =config['MaxLcDauTrkChi2'],
                            MinLcKPIDPi     =config['MinLcKPIDPi'],
                            MinLcpPIDPi     =config['MinLcpPIDPi'],
                            MinLcpPIDK      =config['MinLcpPIDK'],
                            MinLcPiPIDK     =config['MinLcPiPIDK'])
        self.selPh = makePh('PhFor'+name,
                            MaxPhDeltaM     =config['MaxPhDeltaM'],     
                            MinPhPt         =config['MinPhPt'],         
                            MaxPhVertChi2DOF=config['MaxPhVertChi2DOF'],
                            MinPhPVVDChi2   =config['MinPhPVVDChi2'],   
                            MinPhIPChi2     =config['MinPhIPChi2'],     
                            MinPhDauPt      =config['MinPhDauPt'],      
                            MinPhDauIPChi2  =config['MinPhDauIPChi2'],  
                            MaxPhDauTrkChi2 =config['MaxPhDauTrkChi2'])
        self.selKS = makeKS('KSFor'+name,
                            MaxKSDeltaM     =config['MaxKSDeltaM'],     
                            MinKSPt         =config['MinKSPt'],         
                            MaxKSVertChi2DOF=config['MaxKSVertChi2DOF'],
                            MinKSPVVDChi2   =config['MinKSPVVDChi2'],   
                            MinKSIPChi2     =config['MinKSIPChi2'],     
                            MinKSDauPt      =config['MinKSDauPt'],      
                            MinKSDauIPChi2  =config['MinKSDauIPChi2'],  
                            MaxKSDauTrkChi2 =config['MaxKSDauTrkChi2'],
                            MinKSKPIDPi     =config['MinKSKPIDPi'],
                            MinKSPiPIDK     =config['MinKSPiPIDK'])
        self.selJp = makeJp('JpFor'+name,
                            MaxJpDeltaM     =config['MaxJpDeltaM'],     
                            MinJpPt         =config['MinJpPt'],         
                            MaxJpVertChi2DOF=config['MaxJpVertChi2DOF'],
                            MinJpPVVDChi2   =config['MinJpPVVDChi2'],   
                            MinJpIPChi2     =config['MinJpIPChi2'],     
                            MinJpDauPt      =config['MinJpDauPt'],      
                            MinJpDauIPChi2  =config['MinJpDauIPChi2'],  
                            MaxJpDauTrkChi2 =config['MaxJpDauTrkChi2']) 
        self.selDS = makeDS('DSFor'+name,
                            MaxDSDeltaM       =config['MaxDSDeltaM'],      
                            MinDSPt           =config['MinDSPt'],          
                            MaxDSVertChi2DOF  =config['MaxDSVertChi2DOF'], 
                            MinDSPVVDChi2     =config['MinDSPVVDChi2'],    
                            MinDSIPChi2       =config['MinDSIPChi2'],      
                            MinDSPiPt         =config['MinDSPiPt'],        
                            MinDSPiIPChi2     =config['MinDSPiIPChi2'],    
                            MaxDSPiTrkChi2    =config['MaxDSPiTrkChi2'],   
                            MaxDSD0DeltaM     =config['MaxDSD0DeltaM'],    
                            MinDSD0Pt         =config['MinDSD0Pt'],        
                            MaxDSD0VertChi2DOF=config['MaxDSD0VertChi2DOF'],
                            MinDSD0PVVDChi2   =config['MinDSD0PVVDChi2'],  
                            MinDSD0IPChi2     =config['MinDSD0IPChi2'],    
                            MinDSD0DauPt      =config['MinDSD0DauPt'],     
                            MinDSD0DauIPChi2  =config['MinDSD0DauIPChi2'], 
                            MaxDSD0DauTrkChi2 =config['MaxDSD0DauTrkChi2'])

        
        self.selB2nbody = makeB2nbody(name,
                                     PiSel=self.selPi,
                                     KSel =self.selK,
                                     pSel =self.selp,
                                     KsSel=self.selKs,
                                     LmSel=self.selLm,
                                     DzSel=self.selDz,
                                     DpSel=self.selDp,
                                     DsSel=self.selDs,
                                     LcSel=self.selLc,
                                     PhSel=self.selPh,
                                     KSSel=self.selKS,
                                     JpSel=self.selJp,
                                     DSSel=self.selDS,
                                     nbody          =config['nbody'],       
                                     MinBMass       =config['MinBMass'],       
                                     MaxBMass       =config['MaxBMass'],       
                                     MinBPt         =config['MinBPt'],         
                                     MaxBVertChi2DOF=config['MaxBVertChi2DOF'],
                                     MinBPVVDChi2   =config['MinBPVVDChi2'],   
                                     MaxBPVIPChi2   =config['MaxBPVIPChi2'],   
                                     MinBPVDIRA     =config['MinBPVDIRA'],     
                                     MaxMass        =config['MaxMass'],
                                     MaxNtrk        =config['MaxNtrk'],
                                     MinNvc         =config['MinNvc'],
                                     doPi           =config['doPi'],
                                     doK            =config['doK'],
                                     dop            =config['dop'],
                                     doKs           =config['doKs'],
                                     doLm           =config['doLm'],
                                     doDz           =config['doDz'],
                                     doDp           =config['doDp'],
                                     doDs           =config['doDs'],
                                     doLc           =config['doLc'],
                                     doPh           =config['doPh'],
                                     doKS           =config['doKS'],
                                     doJp           =config['doJp'],
                                     doDS           =config['doDS'])
        TrLongFilter={'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )"% config['MaxTrLong'],
                      'Preambulo' : [ "from LoKiTracks.decorators import *",'from LoKiCore.functions import *' ]}


        self.line = StrippingLine(name+"Line",
                                  selection = self.selB2nbody,
                                  prescale = config['prescale'],
                                  FILTER = TrLongFilter )
        self.registerLine(self.line)



def makeB2nbody(name,PiSel,KSel,pSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,PhSel,KSSel,JpSel,DSSel,
                nbody,MinBMass,MaxBMass,MinBPt,MaxBVertChi2DOF,MinBPVVDChi2,MaxBPVIPChi2,MinBPVDIRA,MaxMass,MaxNtrk,MinNvc,
                doPi,doK,dop,doKs,doLm,doDz,doDp,doDs,doLc,doPh,doKS,doJp,doDS):

    # Define a class that contains all relevant particle properties
    class particle:
        def __init__(self,name,m,ntrk,nvc,q,b):
            self.name=name
            self.m=m            #mass
            self.ntrk=ntrk      # number of charged tracks in fian state
            self.nvc=nvc        # number of vertex-contraining (pseudo)tracks
            self.Q=q            # electrical charge
            self.B=b            # baryon number
            self.cp=self
        def CP(self,name):
            p= particle(name,self.m,self.ntrk,self.nvc,-self.Q,-self.B)
            p.cp=self
            self.cp=p
            return p
        def isCP(self):
            if self.cp.name==self.name: return True
            else: return False



    # Define the particles
    particles=[]
    if doPi: particles.append(particle(        "pi+"       , 139.6,1,1,  +1,  0))
    if doPi: particles.append(particles[-1].CP("pi-"       ))
    if doK:  particles.append(particle(        "K+"        , 493.7,1,1,  +1,  0))
    if doK:  particles.append(particles[-1].CP("K-"        ))
    if dop:  particles.append(particle(        "p+"        , 938.3,1,1,  +1, +1))
    if dop:  particles.append(particles[-1].CP("p~-"       ))
    if doKs: particles.append(particle(        "KS0"       , 497.6,2,0,   0,  0))
    if doLm: particles.append(particle(        "Lambda0"   ,1115.7,2,0,   0, +1))
    if doLm: particles.append(particles[-1].CP("Lambda~0"  ))
    if doDz: particles.append(particle(        "D0"        ,1864.8,2,1,   0,  0))
    if doDz: particles.append(particles[-1].CP("D~0"       ))
    if doDp: particles.append(particle(        "D+"        ,1869.6,3,1,  +1,  0))
    if doDp: particles.append(particles[-1].CP("D-"        ))
    if doDs: particles.append(particle(        "D_s+"      ,1968.5,3,1,  +1,  0))
    if doDs: particles.append(particles[-1].CP("D_s-"      ))
    if doLc: particles.append(particle(        "Lambda_c+" ,2286.5,3,1,  +1, +1))
    if doLc: particles.append(particles[-1].CP("Lambda_c~-"))
    if doPh: particles.append(particle(        "phi(1020)" ,1019.5,2,2,   0,  0))
    if doKS: particles.append(particle(        "K*(892)0"  , 895.9,2,2,   0,  0))
    if doKS: particles.append(particles[-1].CP("K*(892)~0" ))
    if doJp: particles.append(particle(        "J/psi(1S)" ,3096.9,2,2,   0,  0))
    if doDS: particles.append(particle(        "D*(2010)+" ,2010.2,3,2,  +1,  0))
    if doDS: particles.append(particles[-1].CP("D*(2010)-" ))

    # in itertools only since python 2.7
    def combinations_with_replacement(iterable, r):
        pool = tuple(iterable)
        n = len(pool)
        if not n and r: return
        indices = [0] * r
        yield tuple(pool[i] for i in indices)
        while True:
            for i in reversed(range(r)):
                if indices[i] != n - 1: break
            else: return
            indices[i:] = [indices[i] + 1] * (r - i)
            yield tuple(pool[i] for i in indices)

    #alphabetically ordered name of particles
    def pname(ps):
        names=[]
        for p in ps: names.append(p.name)
        names.sort()
        result=""
        for name in names:
            result=result+name+' '
        return result[:-1]

    descriptors = []
    for comb in combinations_with_replacement(particles,nbody):
        #check for the mass
        m=0
        for p in comb: m+=p.m
        if m>MaxMass: continue
        #check for the number of tracks in the final state
        ntrk=0
        for p in comb: ntrk+=p.ntrk
        if ntrk>MaxNtrk: continue
        #check for the number of vertex-constraining (pseudo)tracks
        nvc=0
        for p in comb: nvc+=p.nvc
        if nvc<MinNvc: continue
        #check for charge and baryon number
        Q=0
        for p in comb: Q+=p.Q
        if abs(Q)>1: continue
        B=0
        for p in comb: B+=p.B
        if abs(B)>1: continue

        #assign identity to mother 
        mother=""
        if Q==0  and B==0: mother="B0"
        if Q==1  and B==0: mother="B+"
        if Q==0  and B==1: mother="Lambda_b0"
        if Q==-1 and B==1: mother="Xi_b-"
        if Q==+1 and B==1: mother="Xi_bc+"
        if mother=="": continue

        #check if final state is CP eigenstate
        ps=[p for p in comb if not p.isCP()]
        while True:
            doagain=False
            for p in ps:
                if ps.count(p.cp)>0:
                    ps.remove(p)
                    ps.remove(p.cp)
                    doagain=True
                    break
            if not doagain: break
        if len(ps)==0: CP=True
        else: CP=False
        
        #for B0 non-CP combinations come twice. 
        if mother=="B0" and not CP:
            combcp=[x.cp for x in comb]
            if pname(comb)<pname(combcp):continue

        #build the decay descriptor
        descriptor=mother+" -> "+pname(comb)
        if not CP:descriptor="["+descriptor+"]cc"
        descriptors.append(descriptor)

    #for descriptor in descriptors: print "DESCRIPTOR:",descriptor

    #make a merge of the input selections
    AllSel=[]
    if doPi:AllSel.append(PiSel)
    if doK: AllSel.append(KSel)
    if dop: AllSel.append(pSel)
    if doKs:AllSel.append(KsSel)
    if doLm:AllSel.append(LmSel)
    if doDz:AllSel.append(DzSel)
    if doDp:AllSel.append(DpSel)
    if doDs:AllSel.append(DsSel)
    if doLc:AllSel.append(LcSel)
    if doPh:AllSel.append(PhSel)
    if doKS:AllSel.append(KSSel)
    if doJp:AllSel.append(JpSel)
    if doDS:AllSel.append(DSSel)
    InputSel= MergedSelection("InputFor"+name, RequiredSelections = AllSel )

    _combinationCuts =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    _combinationCuts += "& (APT>%(MinBPt)s*MeV)"%locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s)"%locals()
    _motherCuts += "& (BPVVDCHI2 > %(MinBPVVDChi2)s)"% locals()
    _motherCuts += "& (BPVIPCHI2() < %(MaxBPVIPChi2)s)"%locals()
    _motherCuts += "& (BPVDIRA > %(MinBPVDIRA)s)"% locals()

    #make a selection
    _B=CombineParticles(DecayDescriptors = descriptors, CombinationCut = _combinationCuts, MotherCut = _motherCuts)

    #make a preselection
    _presel=VoidEventSelection("preselFor"+name,
                               Code="(CONTAINS('%s')> %3.1f)"%(InputSel.outputLocation(),nbody-0.5 ),
                               RequiredSelection=InputSel
                               )

    return Selection(name, Algorithm = _B, RequiredSelections = [_presel] )

from StandardParticles import StdLoosePions
def makePi(name, MinPiPt, MinPiIPChi2DV, MaxPiChi2, MinPiPIDK, MinPiPIDp, MaxPiGHP) :
    _code =  "(PT > %(MinPiPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinPiIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxPiChi2)s)"%locals()
    _code+="& (PIDpi-PIDK > %(MinPiPIDK)s)"%locals()
    _code+="& (PIDpi-PIDp > %(MinPiPIDp)s)"%locals()
    _code+="& (TRGHP < %(MaxPiGHP)s)"%locals()
    _PiFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _PiFilter, RequiredSelections = [StdLoosePions])
                        
from StandardParticles import StdLooseKaons
def makeK(name, MinKPt, MinKIPChi2DV, MaxKChi2, MinKPIDPi, MinKPIDp, MaxKGHP) :
    _code =  "(PT > %(MinKPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinKIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxKChi2)s)"%locals()
    _code+="& (PIDK-PIDpi > %(MinKPIDPi)s)"%locals()
    _code+="& (PIDK-PIDp > %(MinKPIDp)s)"%locals()
    _code+="& (TRGHP < %(MaxKGHP)s)"%locals()
    _KFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _KFilter, RequiredSelections = [StdLooseKaons])
                        
from StandardParticles import StdLooseProtons
def makep(name, MinpPt, MinpIPChi2DV, MaxpChi2, MinpPIDPi, MinpPIDK, MaxpGHP) :
    _code =  "(PT > %(MinpPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinpIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxpChi2)s)"%locals()
    _code+="& (PIDp-PIDpi > %(MinpPIDPi)s)"%locals()
    _code+="& (PIDp-PIDK > %(MinpPIDK)s)"%locals()
    _code+="& (TRGHP < %(MaxpGHP)s)"%locals()
    _pFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _pFilter, RequiredSelections = [StdLooseProtons])

def makeKs(name, MaxKsDeltaM, MinKsPt, MaxKsVertChi2DOF, MinKsPVVDChi2, MinKsIPChi2, MinKsDauPt, MinKsDauIPChi2, MaxKsDauTrkChi2) :
    _code =  "(ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()
    _code+="& (PT>%(MinKsPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxKsVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinKsPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinKsIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinKsDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKsDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 2 )"%locals()
    _KsFilter = FilterDesktop(Code = _code)
    _stdKsLL    = DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")
    _stdKsDD    = DataOnDemand(Location = "Phys/StdLooseKsDD/Particles")
    _KsMerged = MergedSelection( "MergedKsFor" + name, RequiredSelections = [_stdKsDD,_stdKsLL] )
    return Selection (name, Algorithm = _KsFilter, RequiredSelections = [_KsMerged])


def makeLm(name, MaxLmDeltaM, MinLmPt, MaxLmVertChi2DOF, MinLmPVVDChi2, MinLmIPChi2, MinLmPrtPt, MinLmPiPt, MinLmPrtIPChi2, MinLmPiIPChi2, MaxLmPrtTrkChi2, MaxLmPiTrkChi2) :
    _code =  "(ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()
    _code+="& (PT>%(MinLmPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLmVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinLmPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinLmIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinLmPrtPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLmPiPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPrtIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPiIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPrtTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPiTrkChi2)s , 2 )"%locals()
    _LmFilter = FilterDesktop(Code = _code)
    _stdLmLL    = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
    _stdLmDD    = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    _LmMerged = MergedSelection( "MergedLmFor" + name, RequiredSelections = [_stdLmDD,_stdLmLL] )
    return Selection (name, Algorithm = _LmFilter, RequiredSelections = [_LmMerged])

def makeDz(name, MaxDzDeltaM, MinDzPt, MaxDzVertChi2DOF, MinDzPVVDChi2, MinDzIPChi2, MinDzDauPt, MinDzDauIPChi2, MaxDzDauTrkChi2, MinDzKPIDPi,MinDzPiPIDK):
    _code = "(PT>%(MinDzPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDzVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDzPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDzIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDzDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDzDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDzDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDzDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDzDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDzDauTrkChi2)s , 2 )"%locals()
    _code+="& (ADMASS('D0') < %(MaxDzDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDzKPIDPi)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinDzPiPIDK)s , 2 )"%locals()
    _DzFilter = FilterDesktop(Code = _code)
    _stdDz = DataOnDemand(Location = "Phys/StdLooseD02KPi/Particles")
    return Selection (name, Algorithm = _DzFilter, RequiredSelections = [_stdDz])

def makeDp(name, MaxDpDeltaM, MinDpPt, MaxDpVertChi2DOF, MinDpPVVDChi2, MinDpIPChi2, MinDpDauPt, MinDpDauIPChi2, MaxDpDauTrkChi2, MinDpKPIDPi, MinDpPiPIDK):
    _code = "(PT>%(MinDpPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDpVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDpPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDpIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D+') < %(MaxDpDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDpKPIDPi)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinDpPiPIDK)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinDpPiPIDK)s , 3 )"%locals()
    _DpFilter = FilterDesktop(Code = _code)
    _stdDp = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    return Selection (name, Algorithm = _DpFilter, RequiredSelections = [_stdDp])

def makeDs(name, MaxDsDeltaM, MinDsPt, MaxDsVertChi2DOF, MinDsPVVDChi2, MinDsIPChi2, MinDsDauPt, MinDsDauIPChi2, MaxDsDauTrkChi2, MinDsKmPIDPi, MinDsKpPIDPi,MinDsPiPIDK):
    _code = "(PT>%(MinDsPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDsVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDsPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDsIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D_s+') < %(MaxDsDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDsKmPIDPi)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDsKpPIDPi)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinDsPiPIDK)s , 3 )"%locals()
    _DsFilter = FilterDesktop(Code = _code)
    _stdDs = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPi/Particles")
    return Selection (name, Algorithm = _DsFilter, RequiredSelections = [_stdDs])

def makeLc(name, MaxLcDeltaM, MinLcPt, MaxLcVertChi2DOF, MinLcPVVDChi2, MinLcIPChi2, MinLcDauPt, MinLcDauIPChi2, MaxLcDauTrkChi2, MinLcKPIDPi, MinLcpPIDPi, MinLcpPIDK, MinLcPiPIDK):
    _code = "(PT>%(MinLcPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLcVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinLcPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinLcIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('Lambda_c+') < %(MaxLcDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinLcKPIDPi)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDp-PIDpi > %(MinLcpPIDPi)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDp-PIDK > %(MinLcpPIDK)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinLcPiPIDK)s , 3 )"%locals()
    _LcFilter = FilterDesktop(Code = _code)
    _stdLc = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    return Selection (name, Algorithm = _LcFilter, RequiredSelections = [_stdLc])


def makePh(name, MaxPhDeltaM, MinPhPt, MaxPhVertChi2DOF, MinPhPVVDChi2, MinPhIPChi2, MinPhDauPt, MinPhDauIPChi2, MaxPhDauTrkChi2):
    _code =  "( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinPhPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinPhPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinPhIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 2 )"%locals()
    _PhFilter = FilterDesktop(Code = _code)
    _stdPh = DataOnDemand(Location = "Phys/StdLooseDetachedPhi2KK/Particles")
    return Selection (name, Algorithm = _PhFilter, RequiredSelections = [_stdPh])

def makeKS(name, MaxKSDeltaM, MinKSPt, MaxKSVertChi2DOF, MinKSPVVDChi2, MinKSIPChi2, MinKSDauPt, MinKSDauIPChi2, MaxKSDauTrkChi2, MinKSKPIDPi, MinKSPiPIDK):
    _code =  "(ADMASS('K*(892)0') < %(MaxKSDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinKSPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxKSVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinKSPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinKSIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKSDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKSDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinKSKPIDPi)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDpi-PIDK > %(MinKSPiPIDK)s , 2 )"%locals()
    _KSFilter = FilterDesktop(Code = _code)
    _stdKS = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles")
    return Selection (name, Algorithm = _KSFilter, RequiredSelections = [_stdKS])
                                                            
def makeJp(name, MaxJpDeltaM, MinJpPt, MaxJpVertChi2DOF, MinJpPVVDChi2, MinJpIPChi2, MinJpDauPt, MinJpDauIPChi2, MaxJpDauTrkChi2):
    _code =  "( ADMASS('J/psi(1S)') < %(MaxJpDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinJpPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinJpPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinJpIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 2 )"%locals()
    _JpFilter = FilterDesktop(Code = _code)
    _stdJp = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
    return Selection (name, Algorithm = _JpFilter, RequiredSelections = [_stdJp])

def makeDS(name, MaxDSDeltaM, MinDSPt, MaxDSVertChi2DOF, MinDSPVVDChi2, MinDSIPChi2, MinDSPiPt, MinDSPiIPChi2,MaxDSPiTrkChi2, MaxDSD0DeltaM,
           MinDSD0Pt, MaxDSD0VertChi2DOF, MinDSD0PVVDChi2, MinDSD0IPChi2, MinDSD0DauPt, MinDSD0DauIPChi2, MaxDSD0DauTrkChi2):
    _code =  "(M-MAXTREE('D0'==ABSID,M)<%(MaxDSDeltaM)s)"%locals()
    _code+="& ( PT > %(MinDSPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxDSVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinDSPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinDSIPChi2)s )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDSPiPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDSPiIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDSPiTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDSD0Pt)s , 2 )"%locals()
    _code+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    _code+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV(PRIMARY) > %(MinDSD0IPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    _code+="& ( NINGENERATION ( ( PT > %(MinDSD0DauPt)s ) , 2 ) == 2 )"%locals()
    _code+="& ( NINGENERATION ( ( MIPCHI2DV ( PRIMARY ) > %(MinDSD0DauIPChi2)s ) , 2 ) == 2 )"%locals()
    _code+="& ( NINGENERATION ( ( TRCHI2DOF < %(MaxDSD0DauTrkChi2)s ) , 2 ) == 2 )"%locals()
    _DSFilter = FilterDesktop(Code = _code)
    _stdDS = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi/Particles")
    return Selection (name, Algorithm = _DSFilter, RequiredSelections = [_stdDS])
                                                               

