#!/usr/bin/env python

import os
AligWork_dir = '/group/online/AligWork/'
alignment_dir = '/group/online/alignment/'
this_files_dir = os.path.dirname(os.path.realpath(__file__))
references = os.path.join(this_files_dir, '../files/ConstantsReferences.txt')


##########################
###   Options parser   ###
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description ="Macro to make trend plots alignments")
    parser.add_argument('-a','--alignables', help='eg. VeloLeft.Tx Module01.Rz, for having all 6 dof for an alignable: VeloLeft.*', default= ['VeloLeft.*'],nargs='+')
    parser.add_argument('-r','--runs', help='run numbers, default is the last one', nargs=2, type=int,default=[0, 1e20])
    parser.add_argument('--activity', help='eg. Velo, Tracker ...', default= 'Velo')
    parser.add_argument('-o','--outName',help='output file name without extension, make both pdf and directory', default='trendPlots')
    args = parser.parse_args()

##########################

import  re
import ROOT as r
from MultiPlot import MultiPlot


#r.gROOT.ProcessLine('.L LHCbStyle.C')
r.gStyle.SetTitleOffset(0.7,"y")
r.gStyle.SetTitleOffset(-0.5,"x")
r.gStyle.SetLabelSize(0,"x")
r.gStyle.SetTitleSize(0.06,"x")
r.gStyle.SetTitleSize(0.06,"y")
r.gStyle.SetNdivisions(505,"x");
r.gStyle.SetNdivisions(510,"y");

r.gROOT.SetBatch(True)

def getListRuns(activity):
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

    
def getListRunsUpdated(activity):
    runs = []
    if activity == 'Tracker': activity = 'IT'
    directory = os.path.join(alignment_dir, '{0}/{0}Global'.format(activity))
    for ff in os.listdir(directory):
        run = getRunFromXml(os.path.join(directory, ff))
        if run: runs.append(run)
    return sorted(runs)

    
def getLimits(dof, alignable):
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


def getAllConstants(runs, activity):
    '''
    return list of pairs (runNumber, all_aligConstants)
    '''
    all_constants = []
    if activity == 'Velo':
        detectors = ['Velo']
    elif activity == 'Tracker':
        detectors = ['TT', 'IT', 'OT']
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
    given all_constatns prodced with previous function
    return list of pairs (runNumber, constant)
    '''
    dofs = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz']
    return [(run, dictConstants[alignable][dofs.index(dof)]) for run, dictConstants in all_constants]


def subtractMean(values):
    '''
    Subtract to every value the mean of the serie
    '''
    runs, constants = [i[0] for i in values],  [i[1] for i in values]
    mean_const = float(sum(constants))/len(constants)
    return [(run, const-mean_const) for run, const in zip(runs, constants)]


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

               
def drawPlot(dof, alignable):
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
    values = subtractMean(getConstants(dof, alignable, allConstants))
    values = [(i,j*mult) for i,j in values]
    valuesUpdated = [[i,j] for i,j in values]
    for value in valuesUpdated:
        if value[0] not in runsUpdated: value[1] = None
    histo = makeHisto(dof, values)
    histoUpdated = makeHisto(dof, valuesUpdated)          
    mp = MultiPlot('VeloHalf', title = 'Trend plot {alignable} {dof};Run number;Variation [{unit}]'.format(**locals()),
    kind='h', legMarker='p',
    hlines=[0,-1*line,line],
    rangeY = (-1*maxim, maxim),
    drawLegend=False)
    mp.Add(histo,style=-1)
    mp.Add(histoUpdated,style=1)
    mp.Draw()
    addXLabels(mp, maxim, [i[0] for i in values])
    return mp
    
  


if __name__ == '__main__':
        
    runs = getListRuns(args.activity)
    runs = [i for i in runs if i >= args.runs[0] and i <= args.runs[1]]
    runsUpdated = getListRunsUpdated(args.activity)
    runsUpdated = [i for i in runsUpdated if i in runs]

    outFile_name = args.outName+'.pdf'
    outDir = args.outName
    if not os.path.exists(outDir): os.mkdir(outDir)

    allConstants = getAllConstants(runs, args.activity)

    c = r.TCanvas('c', 'c')
    c.Print(outFile_name+'[')

    for align in args.alignables:
        alignable, dof = align.split('.')
        dofs = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'] if dof == '*' else [dof]

        for dof in dofs:
            mp=drawPlot(dof, alignable)
            c.Print(outFile_name)
            c.Print(os.path.join(outDir,'{alignable}_{dof}.pdf'.format(**locals())))

    c.Print(outFile_name+']')

