# $Id: Phi.py,v 1.1 2003-12-07 16:14:30 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Python based phi -> K+ K- "analysis"
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the bender itself  
from BenderModule import *

# create my own algorithm  
class Phi(Bender):
    " My own analysis algorithm for selection of phi -> K+ K- "
    def analyse ( self ) :

        kplus  = self.select( tag="K+" , cuts =  321 == ID )
        kminus = self.select( tag="K-" , cuts = -321 == ID )
        
        phis   = self.loop( formula = "K+ K-" , pid = "phi(1020)" )
        for phi in phis:
            if phi.mass(1,2) > 1050 * MeV : continue
            self.plot( title = "K+ K- mass" , value = M(phi)/MeV , low = 1000 , high = 1050 )
            
        return SUCCESS 

#    create my algorithm 
myAlg = Phi('Phi')

#def property( tag ):   # temporary trick!
#    return Service(tag)

# configure my algorithm 
p1 = Property('Phi')
p1.OutputLevel = 4
# configure my desktop 
desktop                 = Property('Phi.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged" ]
desktop.OutputLocation  =   "/Event/Phys/Phi"

# initialize the algorithms
myAlg.initialize()

# append it to the list of top level algorithms
g.topAlg = g.topAlg + [ 'Phi' ] 

# execute 1 event 
g.execute(1)

#myAlg.finalize()
#g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
