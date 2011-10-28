#Defines the "global" objects to be
#used across the various modules and 
#functions

class globalParams:
    def __init__(self,gp,
                 gaudi,SEL,TES,incidentSvc,
                 m_lifetimeTool,m_distanceTool,m_relatedPVTool,
                 tistostool,tistosbools,triggers,
                 hlt1recoline,hlt2recoline,
                 granularityRefinement,
                 DEBUGMODE,startingEvent,swimStripping,swimOffline,
                 offSelName,offCands,offlinePVs
                ) :
        self.GaudiPython = gp
        self.incidentSvc = incidentSvc
        self.gaudi = gaudi
        self.TES   = TES
        self.SEL   = SEL
     
        self.m_lifetimeTool  = m_lifetimeTool
        self.m_distanceTool  = m_distanceTool
        self.m_relatedPVTool = m_relatedPVTool       

        self.tistostool     = tistostool
        self.tistosbools    = tistosbools
        self.triggers       = triggers
       
        self.hlt1recoline   = hlt1recoline
        self.hlt2recoline   = hlt2recoline
 
        self.granularityRefinement  = granularityRefinement
        self.DEBUGMODE              = DEBUGMODE
        self.startingEvent          = startingEvent
        self.swimStripping          = swimStripping
        self.swimOffline            = swimOffline
        
        self.offSelName             = offSelName
        self.offCands               = offCands
        self.offlinePVs             = offlinePVs          

        # These are some default setups for return values,
        # - The first value is a global Pass/Fail
        # - The second a {name : boolean} where the name is a trigger
        # decision or a stripping candidate location
        # - The third is only filled for when swimming the trigger with a {name : {FSP.key() : boolean}}
        if not swimStripping:
            self.HltDecFalse = (False, {}, {})
        else:
            self.StrDecFalse = (False, {triggers : False}, {})
            self.StrDecTrue  = (True , {triggers : True },  {})
