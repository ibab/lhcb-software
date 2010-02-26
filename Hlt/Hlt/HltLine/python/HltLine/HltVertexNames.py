# A module to hold the hardcoded names of vertices in the Hlt
# and rules for deriving containers from them  

########################################################################
# Globals
########################################################################
# These "globals" define that Vertices go into .../Vertex/
HltGlobalVertexLocation			= "Vertex"

########################################################################
# Vertices
########################################################################
# prefixes where to put the vertices (these go into the prefix field of 
# _vertexLocation and _protosLocation)
#
# We link the vertex and track prefixes
#
from HltTrackNames import HltSharedTracksPrefix, Hlt1TracksPrefix, Hlt2TracksPrefix
HltSharedVerticesPrefix 		= HltSharedTracksPrefix
Hlt1VerticesPrefix 			= Hlt1TracksPrefix
Hlt2VerticesPrefix 			= Hlt2TracksPrefix
#
# names of vertex types (these go into the vertices field of _vertexLocation 
# and _protosLocation)
#
Hlt2DPrimaryVerticesName		= "PV2D"
Hlt3DPrimaryVerticesName		= "PV3D"
#  
########################################################################
# The rules for generating vertex and proto particle containers
########################################################################
# For vertices, the format is e.g. Hlt2/Track/Unfitted/Forward 
#
def _vertexLocation(prefix,type,vertices):
    return prefix + "/" + type + "/" + vertices
__all__ = (	
		#
		# The strings
		#
		HltGlobalVertexLocation,
		#
		HltSharedVerticesPrefix, 
           	Hlt1VerticesPrefix, 
           	Hlt2VerticesPrefix, 
		#
		Hlt2DPrimaryVerticesName,
		Hlt3DPrimaryVerticesName,
		#
		# The functions
		#
		_vertexLocation
	  ) 
