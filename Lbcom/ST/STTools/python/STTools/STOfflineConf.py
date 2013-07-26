from Configurables import STOfflinePosition

class DefaultConfMC09(object):
    def configureTools(self,
                       IT=STOfflinePosition('ToolSvc.ITClusterPosition'),
                       TT=STOfflinePosition('ToolSvc.STOfflinePosition')
                       ):
        # New tune (JvT - 14.07.2010)
        if IT is not None:
            IT.LinSharingCorr2 = -0.0152
            IT.CubicSharingCorr2 = 12.33
            IT.CubicSharingCorr3 = 4.369
            IT.LinSharingCorr4 = 0.530
            IT.DetType = "IT"
            IT.ErrorVec =[ 0.253, 0.236, 0.273, 0.185 ];
            IT.APE = 0.0
        if TT is not None:
            TT.APE = 0.0
                            



class DefaultConf(object):
    def configureTools(self,
                       IT=STOfflinePosition('ToolSvc.ITClusterPosition'),
                       TT=STOfflinePosition('ToolSvc.STOfflinePosition'),
                       UT=STOfflinePosition('ToolSvc.UTClusterPosition')
                       ):
        # New tune (JvT - 14.07.2010)
        if IT is not None:
            IT.LinSharingCorr2 = -0.0152 
            IT.CubicSharingCorr2 = 12.33
            IT.CubicSharingCorr3 = 4.369
            IT.LinSharingCorr4 = 0.530
            IT.DetType = "IT"
            IT.ErrorVec =[ 0.253, 0.236, 0.273, 0.185 ];
            IT.APE = 0.0758 # corresponds to 15 um
            # Uncomment to switch to old settings
            #IT.LinSharingCorr2 = 0.61
            #IT.CubicSharingCorr2 = 7.4;
            #IT.CubicSharingCorr3 = 4.0;
            #IT.LinSharingCorr4 = 0.63
            #IT.applyLorentzCorrection = False;
            #IT.ErrorVec =[  0.189, 0.117, 0.166, 0.037 ]
            #IT.DetType = "IT"

        if TT is not None:
            TT.APE = 0.197 # corresponds to 36 um
            #pass
            # Uncomment to switch to old settings
            #TT.LinSharingCorr2 = 1.1
            #TT.CubicSharingCorr2 = 5.4;
            #TT.CubicSharingCorr3 = 4.0;
            #TT.LinSharingCorr4 = 0.67
            #TT.applyLorentzCorrection = False;
            #TT.ErrorVec =[ 0.196, 0.147, 0.177, 0.037 ]
        if UT is not None:
            UT.DetType="UT"

