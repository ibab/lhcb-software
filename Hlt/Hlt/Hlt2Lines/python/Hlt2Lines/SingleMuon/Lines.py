##
#  @author F. Dettori  Francesco.Dettori@cern.ch
# 
#  Please contact the responsible before editing this file
#
#  For the Hlt2SingleMuonRare line please contact also Xabier Cid Vidal (@ cern.ch)
##

from GaudiKernel.SystemOfUnits import GeV, MeV, mm

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class SingleMuonLines(Hlt2LinesConfigurableUser) :
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'HltReq'  : {"SingleMuon" :  "HLT_PASS_RE('Hlt1TrackMuonDecision')",
                              "NoSPD" :  "HLT_PASS_RE('Hlt1TrackMuonNoSPDDecision')"
                              },
                 'Common' :        {'TrChi2'     :   5,    # Adimensional
                                    'Pt':            1000 * MeV },

                 'SingleMuon' :    {'IP'     : 0.25 * mm,
                                    'IPChi2' : 100, # Adimensional
                                   },

                 'NoSPD' :    {'IP'     : 0.25 * mm,
                                    'IPChi2' : 100, # Adimensional
                                   },


                 'HighPT':         { 'HighPt' : 10000 *MeV },
                 
                 'VHighPT':        { 'HighPt' : 15000 *MeV },
                 
                 'LowPT'   :       { 'HighPt' : 4800 * MeV },

                 'Rare'    :       { 'RarePtMin'          : 5000 * MeV,
                                     'RarePtMax'       : 40000* MeV,
                                     'RareIPChi2'      : 500, # Adimensional
                                     'RareTrChi2Tight' : 2 ,
                                     'RarePMax'  : 500 * GeV,
                                     'muID'  : 1
                                     }
                 
                 }

    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import (SingleMuonFilter, DetachedSingleMuonFilter, SingleMuonHighPTFilter, SingleMuonRareFilter)

        self._stages = {
                        'SingleMuon'       : [DetachedSingleMuonFilter('SingleMuon')],
                        'NoSPD'            : [DetachedSingleMuonFilter('NoSPD')],
                        'HighPT'           : [SingleMuonHighPTFilter('HighPT')],
                        'VHighPT'          : [SingleMuonHighPTFilter('VHighPT')],
                        'LowPT'            : [SingleMuonHighPTFilter('LowPT')],
                        'Rare'             : [SingleMuonRareFilter('Rare')]
                        }
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def hlt_req(self, nickname = ""):
        if hasattr(self, 'HltReq') and self.HltReq:
            if nickname in self.HltReq: 
                return self.HltReq[nickname]
            else:
                return ""

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages).iteritems():
            linename = 'SingleMuon' + nickname if nickname != 'SingleMuon' else nickname

            Hlt2Line(linename, prescale = self.prescale,
                     HLT1 = self.hlt_req(nickname) ,
                     algos = algos, postscale = self.postscale)

