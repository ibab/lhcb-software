from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *
from Configurables import TAlignment

#class AlignmentScenario():
#    __slots__ = {
#        "Name"                 : "" # name of the scenario
#        ,"ElementsToAlign"     : [] # list with alignment elements
#        ,"Constraints"         : [] # list with lagrange constraints
#        ,"SurveyConstraints"   : SurveyConstraints() # survey constraints
#        ,"WriteCondSubDetList" : [] # list of subdetectors written to xml
#        }
#    def __init__( aName ) :
#        Name = aName
        
# scenario used for prompt alignment

def configurePromptAlignment() :
    TAlignment().WriteCondSubDetList += ['Velo','TT','IT','OT','MUON']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("Tx")
    elements.VeloLeft("Tx")
    elements.IT("None")
    elements.ITBoxes("TxTyTzRxRyRz")
    elements.ITLayers("Tx")
    elements.OT("None")
    elements.OTCFrames("Tx")
    elements.OTCFrameLayers("Tz")
    elements.TT("None")
    elements.TTLayers("None")
    elements.TTModules("TxRz")
    elements.Tracker("None")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx : total")
    # fix the q/p scale by not moving T in X
    constraints.append("OT3X : OT/T3X1U.Side : Tx")
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]


# define additional scenarios below
def configureVeloHalfAlignment():
    TAlignment().WriteCondSubDetList  += ['Velo']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("Tx")
    elements.VeloLeft("Tx")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx : total")
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()

