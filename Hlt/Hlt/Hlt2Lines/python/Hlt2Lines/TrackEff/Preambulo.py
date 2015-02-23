__author__  = 'Mika Vesterinen'
__date__    = '$Date: 2015-02-23$'
__version__ = '$Revision: 0.0 $'

def D0Preambulo():
    return  [
        'Flight = [VFASPF(VX)-BPV(VX),VFASPF(VY)-BPV(VY),VFASPF(VZ)-BPV(VZ)]',
        'Flight[0] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Flight[1] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Flight[2] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Tag    = [CHILD(PX,1),CHILD(PY,1),CHILD(PZ,1)]',
        'Prb    = [CHILD(PX,2),CHILD(PY,2),CHILD(PZ,2)]',
        'Tag_M    = CHILD(M,1)',
        'Prb_M    = CHILD(M,2)',
        'Tag_PERP = [Tag[1]*Flight[2]-Tag[2]*Flight[1],Tag[2]*Flight[0]-Tag[0]*Flight[2],Tag[0]*Flight[1]-Tag[1]*Flight[0]]',
        'Tag_P_PERP = math.sqrt(Tag_PERP[0]**2+Tag_PERP[1]**2+Tag_PERP[2]**2)',
        'Prb_PERP = [Prb[1]*Flight[2]-Prb[2]*Flight[1],Prb[2]*Flight[0]-Prb[0]*Flight[2],Prb[0]*Flight[1]-Prb[1]*Flight[0]]',
        'Prb_P_PERP = math.sqrt(Prb_PERP[0]**2+Prb_PERP[1]**2+Prb_PERP[2]**2)',
        'Scale = Tag_P_PERP/Prb_P_PERP',
        'Prb = [Prb[0]*Scale,Prb[1]*Scale,Prb[2]*Scale]',
        'Tag_PE    = math.sqrt(Tag_M**2 + Tag[0]**2+Tag[1]**2+Tag[2]**2)',
        'Prb_PE    = math.sqrt(Prb_M**2 + Prb[0]**2+Prb[1]**2+Prb[2]**2)',
        'D0_M  = math.sqrt(Tag_M**2 + Prb_M**2 + 2*(Tag_PE*Prb_PE - Prb[0]*Tag[0] - Prb[1]*Tag[1] - Prb[2]*Tag[2]))']

def DstPreambulo():
    return  [
        'Flight = [CHILD(VFASPF(VX),1)-BPV(VX),CHILD(VFASPF(VY),1)-BPV(VY),CHILD(VFASPF(VZ),1)-BPV(VZ)]',
        'Flight[0] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Flight[1] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Flight[2] /= math.sqrt(Flight[0]**2+Flight[1]**2+Flight[2]**2)',
        'Tag    = [CHILD(CHILD(PX,1),1),CHILD(CHILD(PY,1),1),CHILD(CHILD(PZ,1),1)]',
        'Prb    = [CHILD(CHILD(PX,2),1),CHILD(CHILD(PY,2),1),CHILD(CHILD(PZ,2),1)]',
        'Slowpi = [CHILD(PX,2),CHILD(PY,2),CHILD(PZ,2)]',
        'Tag_M    = CHILD(CHILD(M,1),1)',
        'Prb_M    = CHILD(CHILD(M,2),1)',
        'Slowpi_M = CHILD(M,2)',
        'Tag_PERP = [Tag[1]*Flight[2]-Tag[2]*Flight[1],Tag[2]*Flight[0]-Tag[0]*Flight[2],Tag[0]*Flight[1]-Tag[1]*Flight[0]]',
        'Tag_P_PERP = math.sqrt(Tag_PERP[0]**2+Tag_PERP[1]**2+Tag_PERP[2]**2)',
        'Prb_PERP = [Prb[1]*Flight[2]-Prb[2]*Flight[1],Prb[2]*Flight[0]-Prb[0]*Flight[2],Prb[0]*Flight[1]-Prb[1]*Flight[0]]',
        'Prb_P_PERP = math.sqrt(Prb_PERP[0]**2+Prb_PERP[1]**2+Prb_PERP[2]**2)',
        'Scale = Tag_P_PERP/Prb_P_PERP',
        'Prb = [Prb[0]*Scale,Prb[1]*Scale,Prb[2]*Scale]',
        'Tag_PE    = math.sqrt(Tag_M**2 + Tag[0]**2+Tag[1]**2+Tag[2]**2)',
        'Prb_PE    = math.sqrt(Prb_M**2 + Prb[0]**2+Prb[1]**2+Prb[2]**2)',
        'Slowpi_PE = math.sqrt(Slowpi_M**2 + Slowpi[0]**2+Slowpi[1]**2+Slowpi[2]**2)',
        'D0_M  = math.sqrt(Tag_M**2 + Prb_M**2 + 2*(Tag_PE*Prb_PE - Prb[0]*Tag[0] - Prb[1]*Tag[1] - Prb[2]*Tag[2]))',
        'D0 = [Tag[0] + Prb[0],Tag[1] + Prb[1],Tag[2] + Prb[2]]',
        'D0_PE = math.sqrt(D0_M**2 + D0[0]**2 + D0[1]**2 + D0[2]**2)',
        'Dst_M = math.sqrt(D0_M**2 + Slowpi_M**2 + 2*(D0_PE*Slowpi_PE - D0[0]*Slowpi[0] - D0[1]*Slowpi[1] - D0[2]*Slowpi[2]))']
