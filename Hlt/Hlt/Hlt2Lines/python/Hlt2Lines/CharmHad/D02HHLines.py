## Exclusive reconstruction of D0 -> h- h'+ modes, adapted directly from
## the former Hlt2CharmHadD02HHLines.py configurable.
##
## Defines sets of stages for 6 lines:
##      'D02KK'
##      'D02KPi'
##      'D02PiPi'
##      'D02KKWideMass'
##      'D02KPiWideMass'
##      'D02PiPiWideMass'
##
## @author Patrick Spradlin
__author__  = [ 'Patrick Spradlin' ]


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadD02HHLines : # {
    def __init__( self ) : # {
        ## Just to prevent typos in copying the strings.
        self.__slotname = "D02HH"

        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = { self.__slotname : {
                          'Pair_AMINDOCA_MAX'        : 0.10 * mm
                        , 'Trk_Max_APT_MIN'          : 1500.0 * MeV
                        , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                        , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                        #, 'D0_acosBPVDIRA_MAX'       : 17.3 * mrad
                        , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                        , 'D0_PT_MIN'                : 2000.0 * MeV
                        ## Should these be in separate config blocks?
                        , 'Mass_M_MIN'               : 1815.0 * MeV
                        , 'Mass_M_MAX'               : 1915.0 * MeV
                        , 'WideMass_M_MIN'           : 1715.0 * MeV
                        , 'WideMass_M_MAX'           : 2015.0 * MeV
                    }
                }

        self.__stages = { }
    # }


    def slots(self) : # {
        return self.slotDict
    # }


    def stages(self) : # {
        ## Define the sets of stages for the lines.
        ## NOTE!!!  This method should only be called from within the
        ##    __apply_configuration__() method of a class that inherits from
        ##    Hlt2LinesConfigurableUser.
        ## I would much prefer that this is done in the class constructor.
        ## However, the import from Stages will not work outside of the
        ##   __apply_configuration__ method of the Hlt2LinesConfigurableUser
        ##   that uses these lines.
        if len(self.__stages) == 0 : # {
            from Stages import MassFilter
            from Stages import D02HH_D0ToKmPipWideMass, D02HH_D0ToKmKpWideMass, D02HH_D0ToPimPipWideMass

            ## Hmm, having several MassFilter with the same name seems strange,
            ## but supposedly it does something sensible.
            ## <grumble, grumble, inscrutable obfuscated wrapper nonsense>
            D02KPi  = MassFilter( self.__slotname, inputs = [D02HH_D0ToKmPipWideMass] )
            D02KK   = MassFilter( self.__slotname, inputs = [D02HH_D0ToKmKpWideMass] )
            D02PiPi = MassFilter( self.__slotname, inputs = [D02HH_D0ToPimPipWideMass] )

            self.__stages = {   'D02KK'           : [D02KK]
                              , 'D02KPi'          : [D02KPi]
                              , 'D02PiPi'         : [D02PiPi]
                              , 'D02KKWideMass'   : [D02HH_D0ToKmKpWideMass]
                              , 'D02KPiWideMass'  : [D02HH_D0ToKmPipWideMass]
                              , 'D02PiPiWideMass' : [D02HH_D0ToPimPipWideMass]
            }
        # }

        return self.__stages
    # }
# }
