__all__=["OffSelFunc"]

#
# Example offline selection : B->hh
#
def OffSelFunc(myGlobs,Particle):
    import ROOT
    Result = 0 
    #Cut variables here
    Min_Mother_FD = 2.4 
    Min_Mother_FDChi2 = 50
    Max_Mother_IP = 0.09
    Max_Mother_IPChi2 = 16  
        
    Mother_PT = 900 
    Max_Daughter_PT = 2450
    Min_Daughter_PT = 700 
    Min_Daughter_P  = 13500

    Max_Daughter_IP = 0.25
    Min_Daughter_IP = 0.11
    Max_Daughter_IPChi2 = 140.
    Min_Daughter_IPChi2 = 80. 
  
    Min_Daughter_TrackChi2 = 3 
    Max_Daughter_TrackChi2 = 4 
 
    B_VertexBase = myGlobs.TES[myGlobs.offlinePVs]
    B_imppar = 0.
    p1_imppar = 0.
    p2_imppar = 0.
    B_chi2 = 0.
    p1_chi2 = 0.
    p2_chi2 = 0.
    B_fd_chi2 = 0.  
    B_fd = 0.

    B_imppar = Double()
    p1_imppar = Double()
    p2_imppar = Double()
    B_chi2 = Double()
    p1_chi2 = Double()
    p2_chi2 = Double()
    B_fd_chi2 = Double()
    B_fd = Double()   
 
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
       (Min_daughter_ip   > Min_Daughter_IP)     and \
       (Max_daughter_ip   > Max_Daughter_IP)     and \
       (B_imppar          < Max_Mother_IP)       and \
       (B_chi2            < Max_Mother_IPChi2)   and \
       (B_fd              > Min_Mother_FD)       and \
       (B_fd_chi2         > Min_Mother_FDChi2) :
        return True
    else :
        return False
