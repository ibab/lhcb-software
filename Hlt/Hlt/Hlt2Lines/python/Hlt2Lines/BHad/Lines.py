from B02HHHHLines import BHadB02HHHHLines


from Configurables import HltANNSvc
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

B02HHHHLines = BHadB02HHHHLines()

theseslots = {   'Prescale' : {'Hlt2BHadB02HHHH'       : 1.0},
                 'TrackGEC' : { 'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS",
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,
                             'Trk_ALL_P_MIN'            :  5000 * MeV,
                             'VCHI2PDOF_MAX'            :  10.0,
                            },
                 # Particles for the "Detached" CPV lines
                 'B2HHHHSharedDetachedB0Child_K' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_pi' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_TightK' : {
                                 'PID_LIM'                  :  7,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_Tightpi' : {
                                 'PID_LIM'                  :  3,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_Pr' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 # Use tighter PID and (and momentum) cuts for protons which
                 # are B0  daughters
                 'TighterProtons' :  {
                                  'PIDp_MIN'              :  5.0,
                                  'DeltaPID_MIN'          :  5.0,
                                  'P_MIN'                 :  10000 * MeV
                                  },
                }

theseslots.update(B02HHHHLines.localcuts())

class BHadLines(Hlt2LinesConfigurableUser):
    from copy import deepcopy
    __slots__ = deepcopy(theseslots)
    __lines__ = {}



    def stages ( self , nickname = '' ) :

        if not self.__lines__ :
            self.__lines__.update(B02HHHHLines.locallines())

        return self.__lines__[nickname] if nickname else self.__lines__

##    def relatedInfo(self):
##        if not hasattr(self, "_relatedInfo"):
##            self._relatedInfo = {}
#### looks like code on mext line would require more than one entry
#### in the list delineated by the parentheses
##            for mod in (B02HHHHLines):
##                if hasattr(mod, 'relatedInfo'):
##                    self._relatedInfo.update(mod.relatedInfo())
##
##        return self._relatedInfo

    def __apply_configuration__(self):

        _stages = self.stages()
##        _relatedInfo = self.relatedInfo()


        from HltLine.HltLine import Hlt2Line
##        for nickname, algos, relatedInfo in self.algorithms( _stages ):
        for nickname, algos in self.algorithms( _stages ):
            doturbo = True if (nickname.find('Turbo') > -1) else False
            Hlt2Line('BHad' + nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale, Turbo = doturbo)
##                     RelatedInfo = relatedInfo)
