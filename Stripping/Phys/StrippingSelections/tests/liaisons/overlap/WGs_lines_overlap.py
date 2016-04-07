#!/usr/bin/env python3
__authors__ = 'Pablo Ruiz Valls, Carlos Vazquez Sierra, Nikita Kazeev'
__date__ = '2016/03/23'

'''
Module for producing overlap tables of lines.

WARNING : The DaVinci version must coincide with the one used for
          the stripping production in the file, as retrocompatibility
          is not guaranteed in all cases.
          
After the environment is set run with:

python WGs_lines_overlap.py -wg <WGs> -min_overlap <threshold> -input_file <file>

   <WGs> can be any single WG or combination of WGs, separated by spaces
   it also admits ALL as a WG (gets all the WGs)

   <threshold> determines the minimum overlap percentage between two different
   lines for them to be printed in the tables

   <file> is the intersections file, formatted as
      strippingversion.line1.line2 overlap

It will produce two files,
<WG>_Stripping_Lines_overlaps_min_<threshold>_percent.log
   A TWiki formatted table of the overlaps
<WG>_Stripping_Lines_overlaps_min_<threshold>_percent.pdf
   A 2D histogram of the overlaps
'''

from itertools import chain
from collections import defaultdict
from StrippingArchive.Utils import buildStreams
from ROOT import TH2F, TCanvas
import argparse

def getLinelist(my_wg, strippingVersion):
    """Lists all the lines with the <my_wg> specified in its
    default condif dictionary ['WG'] key"""
    linelist = []
    for myWG in my_wg:
        # CVS: Defining aliases for certain WGs.
        if   myWG=='BnoC':    myWG='Charmless'
        elif myWG=='Calib':   myWG='Calibration'
        elif myWG=='SL':      myWG='Semileptonic'
        if myWG not in ['B2CC','B2OC','BandQ','Charmless','Calibration','Charm','QEE','RD','Semileptonic']: raise ValueError(myWG+' WG not defined')
        # CVS: Removed 'MiniBias', does not exist as a WG tag itself.        
        myStreams = buildStreams('stripping'+strippingVersion,WGs = myWG)
        for stream in myStreams:
            for line in stream.lines: linelist.append(line.name().strip('Stripping'))
    return linelist

def parse_intersection(text_line):
    """Parses the line in format <stripping_version>.<line1>.<line2>\t<overlap>\n,
    returns (stripping_version, (tuple((line_one, line_two)),
    int(overlap)))"""
    intersection_id, overlap = text_line.strip().split("\t")
    stripping_version, line_one, line_two = intersection_id.split(".")
    return (stripping_version, (tuple((line_one, line_two)), int(overlap)))

def calc_percent(overlaps):
    percent_dict = defaultdict(lambda : defaultdict(dict))
    for stripping_version, overlaps_for_version in overlaps.items():
        lines = tuple(chain(*overlaps_for_version.keys()))
        lines_counts = dict(((line, overlaps_for_version[
            tuple((line, line))]) for line in lines))
        reduced_list = list(set(lines)) # removes duplicates
        for mother_line in reduced_list:
            for line_pair, overlap_for_pair in overlaps_for_version.items():
                if mother_line in line_pair:
                    for line in line_pair:
                        if line is not mother_line:
                            percent_dict[stripping_version][mother_line][line] = 100*float(overlap_for_pair)/float(lines_counts[mother_line])
            percent_dict[stripping_version][mother_line][mother_line] = 100.
    print 'Percentages correctly calculated'
    return percent_dict

def print_table(percents,threshold,fname):
    t = []
    with open(fname, 'w') as fout:
        for stripping_version,mother_lines in percents.items():
            for mother,lines in mother_lines.items():
                for line in lines.keys():
                    if line != mother and lines[line] >= float(threshold):
                        print mother,line,'above 50%=',lines[line]
                        t.append(line)
                        t.append(mother)
        reduced_lines = list(set(t)) # removes duplicates
        reduced_lines.sort()
        for stripping_version,mother_lines in percents.items():
            fout.write('\n|*Stripping'+stripping_version+'*|\n')
            fout.write('||!'+'|!'.join(reduced_lines)+'|\n')
            for row_line in reduced_lines:
                valuelist = []
                for column_line in reduced_lines:
                    if column_line not in mother_lines[row_line].keys():
                        valuelist.append('0.00%')
                    else:
                        valuelist.append(str(format(mother_lines[row_line][column_line],'.2f'))+'%')
                fout.write('|!'+row_line+'|'+'|'.join(valuelist)+'|\n')

def print_matrix(percents,threshold,fname):
  t = []
  for stripping_version,mother_lines in percents.items():
    for mother,lines in mother_lines.items():
      for line in lines.keys():
        if line != mother and lines[line] >= float(threshold):
          print mother,line,'above '+str(threshold)+'%=',lines[line]
          t.append(line)
          t.append(mother)
  reduced_lines = list(set(t)) # removes duplicates
  reduced_lines.sort(reverse=True)
  for stripping_version,mother_lines in percents.items():
    nBins = len(reduced_lines)
    mCorr = TH2F('mCorr','',nBins,0.,nBins,nBins,0.,nBins)
    rowIndex = nBins
    for row_line in reduced_lines:
      colIndex = 1
      for column_line in reduced_lines:
        if column_line not in mother_lines[row_line].keys(): mCorr.SetBinContent(rowIndex,colIndex,0.00)
        else: mCorr.SetBinContent(rowIndex,colIndex,round(float(mother_lines[column_line][row_line]),2)) # inverted row <-> column so that it matches TWiki table, ROOT 2D histograms are build from the bottom up
        colIndex+=1
      rowIndex-=1
  mCorr.SetStats(0)
  for nBin in range(nBins):
    mCorr.GetXaxis().SetBinLabel(nBin+1,reduced_lines[nBins-nBin-1])
    mCorr.GetYaxis().SetBinLabel(nBin+1,reduced_lines[nBin])
  mCorr.GetXaxis().SetLabelSize(.021)
  mCorr.GetYaxis().SetLabelSize(.021)
  fCanvas = TCanvas('fCanvas','')
  fCanvas.SetGrid()
  fCanvas.SetBottomMargin(0.15)
  fCanvas.SetLeftMargin(0.185)
  mCorr.Draw('text,col1')
  fCanvas.Print(fname)

def read_lines_intersections(io_object,wg = None):
    """Reads the intersections file into a dictionary, filtering by WG
    File format:
    <stripping_version>.<line1>.<line2>\t<overlap>\n
    Must contain entries <line1>.<line1> with the total counts.
    Missing pairs are assumed to have zero overlap.
    Args:
        io_object: iterator to read the text lines
        wg: Only lines in this WG
    Returns:
        {<stripping_version>: {tuple(<line1>, <line2>): overlap, ...}, ...}
    """
    overlaps = defaultdict(dict)
    default_version = ''

    for stripping_version, record in map(parse_intersection, io_object):
        if stripping_version != default_version:
            default_version = stripping_version
            linelist = getLinelist(wg,default_version)
        if record[0][0] in linelist and record[0][1] in linelist:
            overlaps[stripping_version][record[0]] = record[1]

    # Do the sanity checks
    # All the counts should be non-zero
    min_overlap = min(map(
        lambda overlaps_for_version: min(overlaps_for_version.values()),
        overlaps.values()))
    if min_overlap < 0:
        raise ValueError("Negative overlap encountered")
    elif min_overlap == 0:
        print("Warning: overlap 0 encountered. In "
              "principle it's valid, but MapReduce "
              "shouldn't have produced it")
    # Check that every line intersects with itself no less than
    # with the others
    for stripping_version, overlaps_for_version in overlaps.items():
        lines = tuple(chain(*overlaps_for_version.keys()))
        lines_counts = dict(((line, overlaps_for_version[
            tuple((line, line))]) for line in lines))
        for line_pair, overlap_for_pair in overlaps_for_version.items():
            for line in line_pair:
                if lines_counts[line] < overlap_for_pair:
                    raise ValueError("Total count for line "+line+" is less than the total count for pair "+str(line_pair[0])+str(line_pair[0])+", stripping version "+stripping_version)
    return overlaps

def main():
    parser = argparse.ArgumentParser(description="Verify lines intersection file")
    parser.add_argument("-wg","--WG",nargs='+',help='Any of [B2CC,B2OC,BandQ,BnoC,Calib,Charm,MiniBias,QEE,RD,SL], any combination or ALL')
    parser.add_argument("-min_overlap","--minOverlap",type=int,default=75,help='Minimum overlap to consider as int, 75% by default')
    parser.add_argument("-input_file","--inputFile",type=str,help='Input file formatted as version.line1.line2 overlap',required=True)
    args = parser.parse_args()
    if args.WG==['ALL']: args.WG = ['B2CC','B2OC','BandQ','BnoC','Calib','Charm','QEE','RD','SL']
    with open(args.inputFile) as input_io: overlaps = read_lines_intersections(input_io,args.WG)
    print("Valid file %s" % args.inputFile)
    args_WG = ''
    if len(args.WG)>1:
        for argWG in args.WG: args_WG += argWG+'_'
        args_WG = args_WG[:-1]
    else:
        for argWG in args.WG: args_WG += argWG
    for stripping_version, overlaps_for_version in overlaps.items():
        print("%s Stripping version %s: %d records" % (str(args.WG), stripping_version, len(overlaps_for_version)))
    pcent = calc_percent(overlaps)
    print_table(pcent,args.minOverlap,args_WG+'_Stripping_Lines_overlaps_min_'+str(args.minOverlap)+'_percent.log')
    print_matrix(pcent,args.minOverlap,args_WG+'_Stripping_Lines_overlaps_min_'+str(args.minOverlap)+'_percent.pdf')

if __name__ == "__main__":
    main()
