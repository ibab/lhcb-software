# A module to hold the hardcoded names of vertices in the Hlt
# and rules for deriving containers from them  
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
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
#
# names of vertex types (these go into the vertices field of _vertexLocation)
#
Hlt3DPrimaryVerticesName		= "PV3D"
#  
########################################################################
# The rules for generating vertex containers
########################################################################
# For vertices, the format is e.g. Hlt/Vertices/PV2D 
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
		#
		Hlt3DPrimaryVerticesName,
		#
		# The functions
		#
		_vertexLocation
	  ) 
