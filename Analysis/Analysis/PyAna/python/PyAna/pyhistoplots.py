'''Helper functions for plotting histograms, sequences, and ntuple columns

Copyright (c) 2010 Juan Palacios juan.palacios.puyana@gmail.com
Subject to the Lesser GNU Public License - see < http://www.gnu.org/licenses/lgpl.html>
'''

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

__all__ = ('histo_plot','sequence_histo_plot','ntuple_plot', 'ntuple_column_histo', 'default_stats')

from matplotlib import pyplot
from PyAna.pyhistogram.histostats import poissonSigma

default_stats =  ('entries','mean', 'sigma', 'integral')

def histo_plot(histogram, errorfunction = poissonSigma, stats = default_stats, show = True, **kwargs) :
    '''
    Plot the contents of a histogram.Histogram
    '''
    axis = histogram.axis
    bins = histogram.filledBins()
    x = [bin.centre for bin in bins]
    w = [bin.height for bin in bins]

    e = None
    if errorfunction :
        e = [errorfunction(bin) for bin in bins]

    stat_text = None
    if stats :
        stat_text = 'Statistics'
        for stat in stats :
            stat_text += '\n%(st)s  %(#)g' % { 'st' : stat, '#' : histogram.__getattribute__(stat)() }

    return sequence_histo_plot(x,
                               weights=w,
                               errors = e,
                               bins=axis.nbins, 
                               range = axis.range,
                               xlabel=axis.label,
                               stats = stat_text,
                               show = show,
                               **kwargs)

def sequence_histo_plot(x, weights=None, errors = None, bins=100, range = None, xlabel='', stats = None, show = True, histtype = 'step', **kwargs) :
    '''
    Plot the contents of a sequence
    '''
    fig = pyplot.figure()
    ax = fig.add_subplot(1,1,1)

    if errors == None :
        ax.hist(x, weights = weights, bins=bins, range = range, histtype=histtype, **kwargs)
    else :
        ax.errorbar(x, weights, errors, fmt = 'o', **kwargs)
        
    ax.set_xlim(range[0], range[1])
    ax.set_ylabel('Entries')
    ax.set_xlabel(xlabel)

    if stats :
        ax.text(0.975, 0.975, stats,
                horizontalalignment='right',
                verticalalignment='top',
                multialignment = 'left',
                transform = ax.transAxes,
                bbox=dict(edgecolor = 'black', facecolor='none', alpha=1.))
    if show :
        fig.show()
    return fig

def ntuple_plot(ntuple, tag, cut=None, **kwargs) :
    '''
    Plot the contents of an ntuple.Ntuple column
    '''

    column_histo = ntuple_column_histo(ntuple, tag, cut)

    return histo_plot(column_histo, **kwargs)

def ntuple_column_histo(ntuple, tag, cut=None, bins=100) :
    '''
    Create and return a histogram.Histogram object constructed from the 
    contents of an ntuple.NTuple column.
    '''
    from PyAna.pyhistogram.histogram import Histogram, Axis
    col = ntuple.column(tag,cut)
    min_range = min(col)
    max_range = max(col)
    width = (max_range-min_range)/bins
    max_range += width/2.
    max_range -= width/2.
    histo =  Histogram(axis=Axis(bins, min_range, max_range, label=tag))
    for x in col :
        histo.fill(x)
    return histo
