
__version__ = "$Id: Alignment.py,v 1.6 2009-02-02 10:09:01 jonrob Exp $"
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
        ,"NTupleProduce" : True
        ,"HistoProduce" : True
        }
    
    ## Apply the configuration
    def applyConf(self):

        sequence = self.getProp("AlignmentSequencer")
        if sequence == None : raise RuntimeError("ERROR : Alignment Sequencer not set")

        from Configurables import ( Rich__Rec__MC__AlignmentMonitor )
        
        # Mirror Alignment monitor for Rich1
        #-------------------------------------------------------------------------------
        RichAlignMoniR1 = Rich__Rec__MC__AlignmentMonitor("RichAlignMoniR1")
        sequence.Members += [RichAlignMoniR1]
     
        RichAlignMoniR1.UseMCTruth    = False
        RichAlignMoniR1.RichDetector  = 0   # Rich1 = 0, Rich2 = 1

        # Track selector
        trselname = "TrackSelector"
        RichAlignMoniR1.addTool( RichTools().trackSelector(trselname), name=trselname )
        
        RichAlignMoniR1.TrackSelector.TrackAlgs = [ "Match","Forward" ]
        RichAlignMoniR1.TrackSelector.MinPCut   = 5

        #RichAlignMoniR1.NTupleProduce = self.getProp("NTupleProduce")
        RichAlignMoniR1.HistoProduce  = self.getProp("HistoProduce")
        
        # Mirror Alignment monitor for Rich2
        #-------------------------------------------------------------------------------
        RichAlignMoniR2 = Rich__Rec__MC__AlignmentMonitor("RichAlignMoniR2")
        sequence.Members += [RichAlignMoniR2]
        
        RichAlignMoniR2.UseMCTruth    = False
        RichAlignMoniR2.RichDetector  = 1   # Rich1 = 0, Rich2 = 1

        # Track selector
        trselname = "TrackSelector"
        RichAlignMoniR2.addTool( RichTools().trackSelector(trselname), name=trselname )

        RichAlignMoniR2.TrackSelector.TrackAlgs = [ "Match","Forward" ]
        RichAlignMoniR2.TrackSelector.MinPCut   = 10

        #RichAlignMoniR2.NTupleProduce = self.getProp("NTupleProduce")
        RichAlignMoniR2.HistoProduce  = self.getProp("HistoProduce")

        RichAlignMoniR2.HPDList = [ 200107, 200108, 200007, 200008,
                                    210107, 210108, 210007, 210008 ]

        # CRJ : For reasons beyond me right now, the follow line does not
        # work, so just comment it out for the moment
        #RichAlignMoniR2.PreBookHistos = [ 0000, 0100, 0101, 0102, 0201, 0202, 0203, 0302, 0303, 0400, 0401, 0404, 0405, 0501, 0502, 0505, 0506, 0602, 0603, 0606, 0607, 0707, 0804, 0805, 0808, 0904, 0905, 0906, 0909, 0910, 1005, 1006, 1007, 1010, 1011, 1106, 1107, 1111, 1208, 1209, 1309, 1310, 1410, 1411, 1511, 1608, 1612, 1613, 1709, 1712, 1713, 1714, 1810, 1813, 1814, 1815, 1915, 2012, 2013, 2016, 2017, 2113, 2114, 2117, 2118, 2214, 2215, 2218, 2219, 2315, 2416, 2417, 2516, 2517, 2518, 2617, 2618, 2619, 2719, 2820, 2821, 2920, 2921, 2922, 3021, 3022, 3023, 3123, 3224, 3320, 3321, 3324, 3325, 3421, 3422, 3425, 3426, 3522, 3523, 3526, 3527, 3624, 3625, 3628, 3724, 3725, 3726, 3728, 3729, 3825, 3826, 3827, 3829, 3830, 3926, 3927, 3931, 4028, 4128, 4129, 4229, 4230, 4330, 4331, 4432, 4529, 4532, 4533, 4534, 4630, 4633, 4634, 4635, 4731, 4734, 4735, 4832, 4932, 4933, 4936, 4937, 5033, 5034, 5037, 5038, 5134, 5135, 5138, 5139, 5236, 5336, 5337, 5338, 5437, 5438, 5439, 5538, 5539 ]
        
