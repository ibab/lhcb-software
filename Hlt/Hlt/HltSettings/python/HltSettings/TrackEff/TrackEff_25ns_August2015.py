from GaudiKernel.SystemOfUnits import GeV, mm, MeV

class TrackEff_25ns_August2015 :
    """
    Threshold settings for Hlt2 Hadronic TrackEff lines: 25ns data taking, August 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author M. Kolpin
    @date 2015-07-23
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2TrackEff_D0ToKpiPionProbe',
            'Hlt2TrackEff_D0ToKpiKaonProbe'
        ]


        return lines

    def Thresholds(self) :

        d = {}

        from Hlt2Lines.TrackEff.Lines     import TrackEffLines
        d.update ({TrackEffLines : { 
                                    'Prescale' : {},
                                    'TrackGEC' : {'NTRACK_MAX'           : 180},
                                    'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS"
                                                , 'D0_MinAM' : 900.*MeV
                                                , 'D0_MaxAM' : 2200.*MeV
                                                , 'D0_MinSumIP' : 0.2*mm
                                                , 'D0_MinVDZ' : 4.0*mm
                                                , 'D0_MaxVCHI2NDF' : 3.5
                                                , 'D0_MaxACosDIRADeg' : 1.2 #degrees (hand made variable)
                                                , 'D0_MaxDOCA' : 0.1 * mm
                                                , 'D0_MinSimpleFitM' : 1865.-450. * MeV
                                                , 'D0_MaxSimpleFitM' : 1865.+450. * MeV
                                                , 'Dst_MaxAOA'  : 2.5 # degrees (hand made variable)
                                                , 'Dst_MinAPT'  : 1500. *MeV 
                                                , 'Dst_MaxDTFM' : 2030. *MeV 
                                                , 'Dst_MaxDTFCHI2NDF' : 3.0 
                                                , 'Dst_MaxSimpleFitDeltaMass' : 250. * MeV
                                                , 'Tag_MinPT' : 1300. * MeV
                                                , 'Tag_MinIPCHI2' : 25. 
                                                , 'Tag_MaxTrchi2' : 2.0 
                                                , 'TagK_MinPIDK' : 0. 
                                                , 'TagPi_MaxPIDK' : 10. 
                                                , 'Slowpi_MinPt' : 120*MeV
                                                }
                               }
                 })
        return d

