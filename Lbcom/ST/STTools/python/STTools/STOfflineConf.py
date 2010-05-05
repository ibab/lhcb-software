from Configurables import STOfflinePosition

class EarlyDataConf(object):
    def configureTools(self,
                       IT=STOfflinePosition('ToolSvc.ITClusterPosition'),
                       TT=STOfflinePosition('ToolSvc.STOfflinePosition')
                       ):
        #New tune (March 2010)
        if IT is not None:
            IT.LinSharingCorr2 = 0.61
            IT.CubicSharingCorr2 = 7.4
            IT.LinSharingCorr4 = 0.63
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
        # New tune (JvT - 24.04.2009)
        if IT is not None:
            IT.ErrorVec = [ 0.189, 0.117, 0.166, 0.037 ]
            IT.LinSharingCorr2 = 0.61
            IT.CubicSharingCorr2 = 7.4
            IT.LinSharingCorr4 = 0.63
            IT.DetType = "IT"
            # Uncomment to switch to old settings
            #IT.MaxNtoCorr = 3
            #IT.SharingCorr = 112.0
            #IT.trim = 0.3
            #IT.ErrorVec =[ 0.22, 0.11, 0.24, 0.2]
            #IT.MaxNtoCorr = 3
            #IT.DetType = "IT"

        if TT is not None:
            pass
            # Uncomment to switch to old settings
            #TT.SharingCorr = 112.0
            #TT.trim = 0.3
            #TT.ErrorVec =[ 0.24, 0.13, 0.26, 0.23]
            
