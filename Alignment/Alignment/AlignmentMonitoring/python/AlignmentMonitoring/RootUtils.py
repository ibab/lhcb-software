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


def getDofDeltas(aout, dof='Tx', regexes_alignables=['Velo/Velo(Left|Right)/Module..']):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values of difference after-before for all the alignables which satisfy one of the regexes
    '''
    import re
    deltas = []
    for alignable in aout.AlignIterations[0].Alignables.keys():
        match = [re.match(regex+'$', alignable) for regex in regexes_alignables]
        if match != [None]*len(match): # there is at least a match
            deltas.append(sum([j.Delta for j in sum([i.Alignables[alignable].AlignmentDOFs for i in aout.AlignIterations],[]) if j.Name == dof ]))
    if 'T' in dof:
        deltas = [i*1000 for i in deltas] # Translations in um 
    elif 'R' in dof:
        deltas = [i*1000000 for i in deltas] # Rotations in urad
    return deltas


def getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft'):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values and list errors
    '''
    list_delta = [(j.Delta, j.DeltaErr) for j in sum([i.Alignables[alignable].AlignmentDOFs for i in aout.AlignIterations],[]) if j.Name == dof ]
    if 'T' in dof:
        delta = [(i[0]*1000, i[1]*1000) for i in list_delta] # Translations in um 
    elif 'R' in dof:
        delta = [(i[0]*1000000, i[1]*1000000) for i in list_delta] # Rotations in urad
    return [i[0] for i in delta], [i[1] for i in delta]


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

def getDrawnCanvas(drawables):
    
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


def makeHisto(name, values, title = None, range = [None, None], nBins = 100):
    if range[0] == None: range[0] = min(values)
    if range[1] == None: range[1] = max(values)+1
    if title == None: title = name
    h = r.TH1D(name, title, nBins, *range)
    for i in values:
        h.Fill(i)
    return h

