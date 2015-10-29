#!/usr/bin/env python

import ROOT as r

def getExpression(aout, expression):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values

    expression in [Chi2, DeltaChi2, DeltaChi2nDofs, DeltaChi2overNDofs, LocalDeltaChi2, MaxModeChi2, NormalisedChi2Change, TrChi2nDof, VChi2nDof]
    '''
    return [getattr(i,expression) for i in aout.AlignIterations]
    
def wrapStringArray(string_list):
    sa = r.std.vector(r.std.string) ()
    for s in string_list:
        sa.push_back( s )
    return sa

def wrapNumericArray(num_list,data_type):
    na = r.std.vector(data_type) ()
    for n in num_list:
        na.push_back( n )
    return na

def getDrawnCanvas(drawables):#, title = 'Pollo'):
    
    if len(drawables) == 1:
        canvas_divsions = [1]
    elif len(drawables) == 2:
        canvas_divsions = [2]
    elif len(drawables) in [3, 4]:
        canvas_divsions = [2,2]
    elif len(drawables) in [5, 6]:
        canvas_divsions = [3,2]
    elif len(drawables) in [7, 8, 9]:
        canvas_divsions = [3,3]
        
    c = r.TCanvas()

    c.Divide(*canvas_divsions)
    
    for i in range(len(drawables)):
        c.cd(i+1)
        drawables[i].Draw()
        
    c.Update()
    return c

def makeGraph(values, errors=None):
    import numpy as np
    x = np.array([float(i) for i in range(len(values))])
    ex = np.array([0 for i in range(len(x))])
    y = np.array([float(i) for i in values])
    if errors:
        ey = np.array([float(i) for i in errors])
    else:
        ey = np.array([0 for i in range(len(x))])
    return r.TGraphErrors(len(x),x, y, ex, ey)

