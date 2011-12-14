__all__=["OffSelFunc"]

#
# Example offline selection : B->hh
#
def OffSelFunc(myGlobs,Particle):
    import ROOT
    Result = 0 
    #Cut variables here
    Max_Mother_IPChi2 = 12  
        
    Mother_PT = 1200 
    Max_Daughter_PT = 2700

    Max_Daughter_IPChi2 = 200.
    Min_Daughter_IPChi2 = 100. 
  
    Max_Daughter_TrackChi2 = 3 
 
    B_VertexBase = myGlobs.TES[myGlobs.offlinePVs]
    B_imppar = 0.
    p1_imppar = 0.
    p2_imppar = 0.
    B_chi2 = 0.
    p1_chi2 = 0.
    p2_chi2 = 0.
    B_fd_chi2 = 0.  
    B_fd = 0.

    B_imppar = ROOT.Double()
    p1_imppar = ROOT.Double()
    p2_imppar = ROOT.Double()
    B_chi2 = ROOT.Double()
    p1_chi2 = ROOT.Double()
    p2_chi2 = ROOT.Double()
    B_fd_chi2 = ROOT.Double()
    B_fd = ROOT.Double()   
 
    myrelatedPV = myGlobs.m_relatedPVTool.relatedPV(Particle, B_VertexBase )#Feed function vertex base, which is then becomes best PV
    myGlobs.m_distanceTool.distance( Particle, myrelatedPV, B_imppar, B_chi2 ) #vertex base here becomes is PV
    myGlobs.m_distanceTool.distance( myrelatedPV,Particle.endVertex(),B_fd,B_fd_chi2)
    # Calculate IP_chi2 for daughter particle 1
    Daughter1 = Particle.daughters()[0].target() #Not sure about this
    #need to get vertex base for daughter1
    daug1relatedPV = myGlobs.m_relatedPVTool.relatedPV(Daughter1, B_VertexBase )
    myGlobs.m_distanceTool.distance( Daughter1, daug1relatedPV, p1_imppar, p1_chi2 )
    # Calculate IP_chi2 for daughter particle 2
    Daughter2 = Particle.daughters()[1].target() #Not sure about this
    #need to get vertex base for daughter2  
    daug2relatedPV= myGlobs.m_relatedPVTool.relatedPV(Daughter2, B_VertexBase )
    myGlobs.m_distanceTool.distance( Daughter2, daug2relatedPV, p2_imppar, p2_chi2 )
    #Apply cut conditions to check if pass/fail
    Min_daughter_chi2 = min(p1_chi2, p2_chi2)
    Max_daughter_chi2 = max(p1_chi2, p2_chi2)
    Min_daughter_ip   = min(p1_imppar, p2_imppar)
    Max_daughter_ip   = max(p1_imppar, p2_imppar)

    if myGlobs.DEBUGMODE:
        print "Quantities to cut offline on:"
        print Min_daughter_chi2, Max_daughter_chi2, Min_daughter_ip, Max_daughter_ip, B_imppar, B_chi2, B_fd, B_fd_chi2
 
    if (Min_daughter_chi2 > Min_Daughter_IPChi2) and \
       (Max_daughter_chi2 > Max_Daughter_IPChi2) and \
       (B_chi2            < Max_Mother_IPChi2)   :
        return True
    else :
        return False
