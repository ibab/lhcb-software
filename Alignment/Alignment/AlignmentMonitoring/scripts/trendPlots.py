#!/usr/bin/env python

import os, sys, re, math

std_AligWork_dir = '/group/online/AligWork/'
std_alignment_dir = '/group/online/alignment/'
std_references = os.path.expandvars('$ALIGNMENTMONITORINGROOT/files/ConstantsReferences.txt')

##########################
###   Options parser   ###
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description ="Macro to make trend plots alignments")
    parser.add_argument('-a','--alignables', help='eg. VeloLeft.Tx Module01.Rz, for having all 6 dof for an alignable: VeloLeft.*', default= ['VeloLeft.Tx', 'VeloLeft.Ty'],nargs='+')
    parser.add_argument('-r','--runs', help='run numbers, default is all the availables', nargs=2, type=int,default=[0, 1e20])
    parser.add_argument('--activity', help='choose between Velo, Tracker, Muon; default is Velo', choices = ['Velo', 'Tracker', 'Muon'] ,default= 'Velo')
    parser.add_argument('-o','--outName',help='output file name without extension, make both pdf and directory', default='trendPlots')
    parser.add_argument('-s', '--samePlot', help='Plot all the alignables in the same plot', action='store_true')
    parser.add_argument('-d', '--drawLegend', help='Draw legend also for plots with only one data-serie', action='store_true')
    parser.add_argument('-u', '--diffUpdate', help='Plot difference wrt update', action='store_true')
    parser.add_argument('-n', '--noUpdate', help='Do not plot with empty dots alignments that did not trigger update', action='store_true')
    parser.add_argument('-p', '--projection', help='Make histogram with projection instead of trend', action='store_true')
    parser.add_argument('-l', '--labelAU', help='Use label in AU as x axis', action='store_true')
    parser.add_argument('-y', '--freeY', help='Leave Y axis range free, implies -l', action='store_true') # not yet implemented
    parser.add_argument('-c', '--canvasSize', help='Canvas dimensions in pixels, default is 900, 500', nargs=2, type=int, default=[900, 500]) # not yet implemented
    parser.add_argument('--AligWork_dir', help='folder with alignment', default = std_AligWork_dir)
    parser.add_argument('--alignment_dir', help='folder with alignment', default = std_alignment_dir)
    parser.add_argument('--references', help='folder with alignment', default = std_references)
    args = parser.parse_args()
    if args.freeY: args.labelAU = True
    if args.activity == 'Muon': args.noUpdate = True
        
##########################

import ROOT as r
from AlignmentMonitoring.MultiPlot import MultiPlot

import AlignmentMonitoring.LHCbStyle # process macro that sets LHCb style
r.gStyle.SetTitleOffset(0.7,"Y")

r.gROOT.SetBatch(True)

def getListRuns(activity, AligWork_dir = std_AligWork_dir):
    runs = []
    for directory in os.listdir(os.path.join(AligWork_dir, activity)): 
        match = re.findall('^run([0-9]+)$', directory)
        if match: runs.append(int(match[0]))
    return sorted(runs)

    
def getRunFromXml(xml_file):
    text = open(xml_file).read()
    try:
        run = re.findall('<!-- Version:: run([0-9]*) -->', text)[0]
        return int(run)
    except IndexError:
        return None

    
def getListRunsUpdated(activity, alignment_dir = std_alignment_dir):
    runs = []
    if activity == 'Tracker': activity = 'IT'
    directory = os.path.join(alignment_dir, '{0}/{0}Global'.format(activity))
    for ff in os.listdir(directory):
        run = getRunFromXml(os.path.join(directory, ff))
        if run: runs.append(run)
    return sorted(runs)

    
def getLimits(dof, alignable, references = std_references):
    for line in open(references).readlines():
        if line[0] != '#':
            try:
                regex, line, maxim = line.split()
                if re.match(regex, '{alignable}.{dof}'.format(**locals())):
                    return (float(line), float(maxim))
            except ValueError:
                pass
    raise IOError('No reference for {alignable} {dof}'.format(**locals()))


def readXML(inFile):
    '''
    Read XML file and return dictionary with {name: align_params}
    where align_params is a list [Tx, Ty, Tz, Rx, Ry, Rz]
    '''
    import xml.etree.ElementTree
    text = open(inFile).read()
    if '<DDDB>' not in text:
        text = '<?xml version="1.0" encoding="ISO-8859-1"?><!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd"><DDDB>\n'+text+'\n</DDDB>'
    root = xml.etree.ElementTree.fromstring(text)
    alignParams = {}
    for alignable in root:
        name = alignable.attrib['name']
        for vec in alignable:
            if vec.attrib['name'] == 'dPosXYZ':
                [Tx, Ty, Tz] = [float(i) for i in vec.text.split()]
            elif vec.attrib['name'] == 'dRotXYZ':
                [Rx, Ry, Rz] = [float(i) for i in vec.text.split()] 

        alignParams[name] = [Tx, Ty, Tz, Rx, Ry, Rz]
    return alignParams


def getAllConstants(runs, activity, AligWork_dir = std_AligWork_dir):
    '''
    return list of pairs (runNumber, all_aligConstants)
    '''
    all_constants = []
    if activity == 'Velo':
        detectors = ['Velo']
    elif activity == 'Tracker':
        detectors = ['TT', 'IT', 'OT']
    elif activity == 'Muon':
        detectors = ['Muon']
    for run in runs:
        dictConstants = {}    
        for detector in detectors:
            directory = os.path.join(AligWork_dir, '{activity}/run{run}/xml/{detector}'.format(**locals()))
            for xml_file in os.listdir(directory):
                dictConstants.update(readXML(os.path.join(directory, xml_file)))
        all_constants.append((run, dictConstants))
    return sorted(all_constants, key=lambda x: x[0])
    


def getConstants(dof, alignable, all_constants):
    '''
    given all_constatns produced with previous function
    return list of pairs (runNumber, constant)
    '''
    dofs = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz']
    try:
        return [(run, dictConstants[alignable][dofs.index(dof)]) for run, dictConstants in all_constants]
    except KeyError as e:
        raise KeyError('Cannot find alignable "{0}", are you sure you selected the correct activity?'.format(e.message))


def subtractMean(values):
    '''
    Subtract to every value the mean of the serie
    '''
    runs, constants = [i[0] for i in values],  [i[1] for i in values]
    mean_const = float(sum(constants))/len(constants)
    return [(run, const-mean_const) for run, const in zip(runs, constants)]



def diffWRTUpdate(values, runsUpdated):
    valuesUsed = [[i,j] for i,j in values]
    currentValue = valuesUsed[0]
    for i in range(len(valuesUsed)):
        tmp = valuesUsed[i][:]
        valuesUsed[i][1] = currentValue[1]
        try:
            if valuesUsed[i][0] in runsUpdated: currentValue = tmp
        except IndexError:
            pass
    return [(i[0], i[1]-j[1]) for i, j in zip(values, valuesUsed)]


def makeHisto(name, values, title='', y_errors=None,):
    '''
    value is a list of pairs (runNumber, constant)
    '''
    x_values = [i[0] for i in values]
    y_values = [i[1] for i in values]
    if y_errors == None: y_errors = [1e-20 for i in y_values]
    histo = r.TH1D(name+'_',title,len(x_values),1,len(x_values)+1)
    for nBin, (x_, y_, ey_) in enumerate(zip(x_values, y_values, y_errors),1):
        if y_ != None:
            r.gStyle.SetErrorX(0.)
            histo.Fill(nBin, y_)
            histo.SetBinError(nBin, ey_)
    return histo.Clone(name)


def makeHistoProjection(name, values, title = None, range = [None, None], nBins = None):
    '''
    value is a list of pairs (runNumber, constant)
    '''
    values = [i[1] for i in values]
    if range[0] == None: range[0] = min(values)
    if range[1] == None: range[1] = max(values)+1
    if nBins == None: nBins = int(2*math.sqrt(len(values)))
    if title == None: title = name
    histo = r.TH1D(name+'_tmp', title, nBins, *range)
    for i in values:
        histo.Fill(i)
    return histo.Clone(name)


def addXLabels(mp, maxim, runs):
    text = r.TText()
    text.SetTextAlign(32)
    text.SetTextAngle(-80)
    text.SetTextSize(0.02)
    axis = mp.hs.GetXaxis()
    for nBin,fillID in enumerate(runs, 1):
        xlow = axis.GetBinLowEdge(nBin)
        xup  = axis.GetBinUpEdge(nBin)
        xmid = axis.GetBinCenter(nBin)
        text.DrawText(xmid,-1.2*maxim,str(int(fillID)))

               
def drawPlot(allConstants, runsUpdated, aligndofs, diffUpdate = False, noUpdate = False, labelAU = False, freeY = False, isProjection = False, references = std_references, onlyRuns2show=None, drawLegend41=False):
    '''
    aligndof is a list of alignables and dof eg [VeloLeft.Tx, ]
    '''

    if not labelAU and not isProjection:
        r.gStyle.SetLabelSize(0,"x")
        r.gStyle.SetTitleOffset(1.1,"X")
    else:
        r.gStyle.SetTitleOffset(1,"X")

    if isinstance(aligndofs, basestring):
        aligndofs = [aligndofs]


    alignable, dof = aligndofs[0].split('.')
    line, maxim = getLimits(dof, alignable)
    
    if 'T' in dof:
        if maxim < 1e-1:
            mult = 1000
            unit = '#mum'
        else:
            mult = 1
            unit = 'mm'
    elif 'R' in dof:
        if maxim < 1e-3:
            mult = 1000000
            unit = '#murad'
        elif maxim < 1e-1:
            mult = 1000
            unit = 'mrad'
        else:
            mult = 1
            unit = 'rad'
    line, maxim = line*mult, maxim*mult

    lines = [0,-1*line,line] if diffUpdate else [0]
    if isProjection:
        vlines, hlines = lines, []
        vlines_styles, hlines_styles = {i : (1 if i == 0 else 7) for i in lines}, {}
        rangeY = (None, None)
        xLabel = ('Variation' if diffUpdate else 'Value') + ' [{0}]'.format(unit)
        yLabel = 'Entries'
    else:
        vlines, hlines = [], lines
        vlines_styles, hlines_styles = {},  {i : (1 if i == 0 else 7) for i in lines}
        rangeY = (None, None) if freeY else (-1*maxim, maxim)
        xLabel = 'Alignment number [AU]' if labelAU else 'Run number'
        yLabel = ('Variation' if diffUpdate else 'Value') + ' [{0}]'.format(unit)
    

    mp = MultiPlot('VeloHalf', title = 'Trend plot {alignable} {dof};{xLabel};{yLabel}'.format(**locals()),
    kind='h', legMarker='p',
    hlines=hlines, vlines = vlines,
    vlines_styles = vlines_styles, hlines_styles = hlines_styles,
    rangeY = rangeY,
    drawLegend= len(aligndofs) != 1 or drawLegend41)

    for style, aligndof in enumerate(aligndofs, 1):
        alignable, dof = aligndof.split('.')
        if diffUpdate:
            values = diffWRTUpdate(getConstants(dof, alignable, allConstants), runsUpdated)
        else:
            values = subtractMean(getConstants(dof, alignable, allConstants))
        values = [(i,j*mult) for i,j in values]
        if onlyRuns2show != None:
            new_values = []
            for value in values:
                if value[0] in onlyRuns2show:
                    new_values.append(value)
            values = new_values
        valuesUpdated = [[i,j] for i,j in values]
        if not noUpdate:
            for value in valuesUpdated:
                if value[0] not in runsUpdated: value[1] = None
        if isProjection:
            histo = makeHistoProjection(dof, values, range=(-1*maxim, maxim))
            mp.Add(histo, '{0} {1}'.format(alignable, dof) ,style=style)
            mp.Draw()
        else:
            histo = makeHisto(dof, values)
            histoUpdated = makeHisto(dof, valuesUpdated)          
            mp.Add(histo,style=-1*style)
            mp.Add(histoUpdated, '{0} {1}'.format(alignable, dof) ,style=style)
            mp.Draw()
            if not labelAU:
                addXLabels(mp, maxim, [i[0] for i in values])
    return mp
    
  


if __name__ == '__main__':
        
    runs = getListRuns(args.activity, args.AligWork_dir)
    runs = [i for i in runs if i >= args.runs[0] and i <= args.runs[1]]
    runsUpdated = getListRunsUpdated(args.activity, args.alignment_dir)
    runsUpdated = [i for i in runsUpdated if i in runs]  

    outFile_name = args.outName+'.pdf'
    outDir = args.outName
    if not os.path.exists(outDir): os.mkdir(outDir)

    allConstants = getAllConstants(runs, args.activity, args.AligWork_dir)

    c = r.TCanvas('c', 'c', *args.canvasSize)
    c.Print(outFile_name+'[')

    if args.samePlot:
        mp=drawPlot(allConstants,runsUpdated, args.alignables, args.diffUpdate, args.noUpdate, args.labelAU, args.freeY, args.projection, references = args.references, drawLegend41=args.drawLegend)
        c.Print(outFile_name)
        c.Print(os.path.join(outDir,'{0}_{1}.pdf'.format(*args.alignables[0].split('.'))))

    else:
        for align in args.alignables:
            alignable, dof = align.split('.')
            dofs = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'] if dof == '*' else [dof]

            for dof in dofs:
                mp=drawPlot(allConstants, runsUpdated, '{0}.{1}'.format(alignable, dof), args.diffUpdate, args.noUpdate, args.labelAU, args.freeY, args.projection, references = args.references, drawLegend41=args.drawLegend)
                c.Print(outFile_name)
                c.Print(os.path.join(outDir,'{alignable}_{dof}.pdf'.format(**locals())))

    c.Print(outFile_name+']')

