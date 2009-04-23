
__version__ = "$Id: Alignment.py,v 1.10 2009-04-23 15:05:33 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichAlignmentConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichAlignmentConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ ]

    ## Steering options
    __slots__ = {
        "Context": "Offline"  # The context within which to run
        ,"AlignmentSequencer" : None
        ,"Detectors"        : [ "Rich1", "Rich2" ] # The RICH detectors to monitor
        ,"NTupleProduce"    : True
        ,"HistoProduce"     : True
        ,"WithMC"           : False     # set to True to use MC truth
        ,"HistoOutputLevel" : ["Minimal", "Minimal"] # options are: Minimal, OnlyPrebookedMirrors, Full
        }

    ## Apply the configuration
    def applyConf(self):

        if not self.isPropertySet("AlignmentSequencer") :
            raise RuntimeError("ERROR : Alignment Sequencer not set")
        sequence = self.getProp("AlignmentSequencer")
        
        from Configurables import ( Rich__Rec__MC__AlignmentMonitor )

        # Mirror Alignment monitor for Rich1
        #-------------------------------------------------------------------------------
        if "Rich1" in self.getProp("Detectors") :

            RichAlignMoniR1 = Rich__Rec__MC__AlignmentMonitor("RichAlignMoniR1")
            sequence.Members += [RichAlignMoniR1]

            RichAlignMoniR1.UseMCTruth    = self.getProp("WithMC")
            RichAlignMoniR1.RichDetector  = 0   # Rich1 = 0, Rich2 = 1

            # Track selector
            trselname = "TrackSelector"
            RichAlignMoniR1.addTool( RichTools().trackSelector(trselname), name=trselname )

            RichAlignMoniR1.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            RichAlignMoniR1.TrackSelector.MinPCut   = 5

            # RichAlignMoniR1.NTupleProduce = self.getProp("NTupleProduce")
            RichAlignMoniR1.HistoProduce  = self.getProp("HistoProduce")

            if self.getProp("HistoOutputLevel")[0] == "Full" :
                RichAlignMoniR1.MinimalHistoOutput = False
                RichAlignMoniR1.OnlyPrebookedMirrors = False
            elif self.getProp("HistoOutputLevel")[0] == "OnlyPrebookedMirrors" :
                RichAlignMoniR1.MinimalHistoOutput = False
                RichAlignMoniR1.OnlyPrebookedMirrors = True
            else :
                RichAlignMoniR1.MinimalHistoOutput = True
                RichAlignMoniR1.OnlyPrebookedMirrors = True

            # This list is of "popular" mirrors. A longer list is required for full alignment
            # RichAlignMoniR1.PreBookHistos = ['0003','0106',  '0212','0309' ]

        # Mirror Alignment monitor for Rich2
        #-------------------------------------------------------------------------------
        if "Rich2" in self.getProp("Detectors") :

            RichAlignMoniR2 = Rich__Rec__MC__AlignmentMonitor("RichAlignMoniR2")
            sequence.Members += [RichAlignMoniR2]

            RichAlignMoniR2.UseMCTruth    = self.getProp("WithMC")
            RichAlignMoniR2.RichDetector  = 1   # Rich1 = 0, Rich2 = 1

            # Track selector
            trselname = "TrackSelector"
            RichAlignMoniR2.addTool( RichTools().trackSelector(trselname), name=trselname )

            RichAlignMoniR2.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            RichAlignMoniR2.TrackSelector.MinPCut   = 10

            # RichAlignMoniR2.NTupleProduce = self.getProp("NTupleProduce")
            RichAlignMoniR2.HistoProduce  = self.getProp("HistoProduce")

            if self.getProp("HistoOutputLevel")[1] == "Full" :
                RichAlignMoniR2.MinimalHistoOutput = False
                RichAlignMoniR2.OnlyPrebookedMirrors = False
            elif self.getProp("HistoOutputLevel")[1] == "OnlyPrebookedMirrors" :
                RichAlignMoniR2.MinimalHistoOutput = False
                RichAlignMoniR2.OnlyPrebookedMirrors = True
            else :
                RichAlignMoniR2.MinimalHistoOutput = True
                RichAlignMoniR2.OnlyPrebookedMirrors = True

            RichAlignMoniR2.HPDList = [ 200107, 200108, 200007, 200008 ]

            # List of combinations of RICH2 spheric and flat mirror segments where
            # e.g. '2719' means spheric mirror No 27 and flat mirror No 19.
            # Histograms for these combinations are prebooked from the beginnig to
            # avoid problems with merging ROOT files from all Ganga subjobs when
            # they are finished.
            #
            # The list is depicted here in a way that reflects the real-space
            # positions of the segments involved in each combination in XY plane,
            # as seen along the beam direction. So the beam pipe position, for
            # example, is somehere in the "geometrical center" of the list,
            # i.e.between '1208' and '4331'.
            # After the arrival of the python-based configuration, this is an array
            # of strings rather than integers.
            # Anatoly Solomin 2008-11-01.
            RichAlignMoniR2.PreBookHistos = [
                '2719','2718',  '2619','2618','2617',  '2518','2517','2516',  '2417','2416',  '5539','5538',  '5439','5438','5437',  '5338','5337','5336',  '5237','5236',


                '2219','2218',         '2118','2117',         '2017','2016',  '5139','5138',         '5038','5037',         '4937','4936',
                '2315',         '2215','2214',         '2114','2113',         '2013','2012',  '5135','5134',         '5034','5033',         '4933','4932',         '4832',


                '1915','1914',  '1815','1814','1813',  '1714','1713','1712',  '1613','1612',  '4735','4734',  '4635','4634','4633',  '4534','4533','4532',  '4433','4432',
                '1911',         '1811','1810',         '1710','1709',                '1608',  '4731',                '4630','4629',         '4529','4528',         '4428',


                '1511',         '1411','1410',         '1310','1309',         '1209','1208',  '4331','4330',         '4230','4229',         '4129','4128',         '4028',


                '1111',         '1011','1010',         '0910','0909',                '0808',  '3931',                '3830','3829',         '3729','3728',         '3628',
                '1107','1106',  '1007','1006','1005',  '0906','0905','0904',  '0805','0804',  '3927','3926',  '3827','3826','3825',  '3726','3725','3724',  '3625','3624',


                '0707',         '0607','0606',         '0506','0505',         '0405','0404',  '3527','3526',         '3426','3425',         '3325','3324',         '3224',
                '0603','0602',         '0502','0501',         '0401','0400',  '3523','3522',         '3422','3421',         '3321','3320',


                '0303','0302',  '0203','0202','0201',  '0102','0101','0100',  '0001','0000',  '3123','3122',  '3023','3022','3021',  '2922','2921','2920',  '2821','2820'
                ]

