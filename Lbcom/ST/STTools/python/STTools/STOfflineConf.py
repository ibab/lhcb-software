from Configurables import STOfflinePosition

class EarlyDataConf(object):
    def configureTools(self,
                       IT=STOfflinePosition('ToolSvc.ITClusterPosition'),
                       TT=STOfflinePosition('ToolSvc.STOfflinePosition')
                       ):
        #New tune (March 2010)
        if IT is not None:
            IT.LinSharingCorr2 = 0.0398 
            IT.CubicSharingCorr2 = 13.6;
            IT.LinSharingCorr4 = 0.866
            IT.DetType = "IT"
            IT.ErrorVec = [0.28, 0.20, 0.3, 0.2]
            IT.APE = 0.05
        
        
        if TT is not None:
            TT.ErrorVec = [0.289, 0.339, 0.322, 0.459]
            TT.APE = 0.197 #corresponds to 36 um

class DefaultConf(object):
    def configureTools(self,
                       IT=STOfflinePosition('ToolSvc.ITClusterPosition'),
                       TT=STOfflinePosition('ToolSvc.STOfflinePosition')
                       ):
        # New tune (JvT - 17.06.2009)
        if IT is not None:
            IT.LinSharingCorr2 = 0.0398 
            IT.CubicSharingCorr2 = 13.6;
            IT.LinSharingCorr4 = 0.866
            IT.DetType = "IT"
            IT.ErrorVec =[ 0.258, 0.255, 0.290, 0.163 ];
            # Uncomment to switch to old settings
            #IT.LinSharingCorr2 = 0.61
            #IT.CubicSharingCorr2 = 7.4;
            #IT.LinSharingCorr4 = 0.63
            #IT.applyLorentzCorrection = False;
            #IT.ErrorVec =[  0.189, 0.117, 0.166, 0.037 ]
            #IT.DetType = "IT"

        if TT is not None:
            pass
            # Uncomment to switch to old settings
            #TT.LinSharingCorr2 = 1.1
            #TT.CubicSharingCorr2 = 5.4;
            #TT.LinSharingCorr4 = 0.67
            #TT.applyLorentzCorrection = False;
            #TT.ErrorVec =[ 0.196, 0.147, 0.177, 0.037 ]
