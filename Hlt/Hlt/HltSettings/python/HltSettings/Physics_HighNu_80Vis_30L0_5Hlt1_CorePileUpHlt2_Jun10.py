from  Physics_HighNu_80Vis_30L0_5Hlt1_CoreHlt2_Jun10 import  Physics_HighNu_80Vis_30L0_5Hlt1_CoreHlt2_Jun10 
class Physics_HighNu_80Vis_30L0_5Hlt1_CorePileUpHlt2_Jun10 ( Physics_HighNu_80Vis_30L0_5Hlt1_CoreHlt2_Jun10) :
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, J. Albrecht
    @date 2010-06-23
    """
    
    def __init__(self) :
        from HltTracking.HltVertexNames import HltSharedVerticesPrefix
        from HltTracking.HltVertexNames import Hlt3DPrimaryVerticesName,_vertexLocation
        from HltTracking.HltVertexNames import HltGlobalVertexLocation
        self.Hlt2DefaultVoidFilter = "CONTAINS( '%s' ) < 4" % _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt3DPrimaryVerticesName)

    def HltType(self) :
        self.verifyType( Physics_HighNu_80Vis_30L0_5Hlt1_CorePileUpHlt2_Jun10 )
        return          'Physics_HighNu_80Vis_30L0_5Hlt1_CorePileUpHlt2_Jun10'
