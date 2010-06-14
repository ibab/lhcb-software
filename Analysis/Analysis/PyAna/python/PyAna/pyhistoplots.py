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

default_stats =  ('entries','mean', 'sigma', 'integral')

def histo_plot(histogram, histtype='step', color='red', linewidth='1.5',  stats = default_stats, show = True, **kwargs) :
    '''
    Plot the contents of a histogram.Histogram
    '''
    fig = pyplot.figure()
    axis = histogram.axis
    bins = histogram.filledBins()
    x = [bin.centre for bin in bins]
    w = [bin.height for bin in bins]
    stat_text = None
    if stats :
        stat_text = 'Statistics'
        for stat in stats :
            stat_text += '\n'+stat + '   ' + str(histogram.__getattribute__(stat)())
    return sequence_histo_plot(x,
                               weights=w,
                               bins=axis.nbins, 
                               range = axis.range,
                               xlabel=axis.label,
                               stats = stat_text,
                               show = show)

def sequence_histo_plot(x, bins=100, xlabel='',histtype='step', color='red', linewidth='1.5', stats = None, show = True, **kwargs) :
    '''
    Plot the contents of a sequence
    '''
    fig = pyplot.figure()
    ax = fig.add_subplot(1,1,1)
    ax.hist(x, bins=bins, histtype=histtype, color=color, linewidth=linewidth, **kwargs)
    ax.set_ylabel('Entries')
    ax.set_xlabel(xlabel)
    if stats :
        ax.text(0.75, 0.95, stats,
                horizontalalignment='left',
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
    return sequence_histo_plot(ntuple.column(tag, cut), xlabel = tag, **kwargs) 

def ntuple_column_histo(ntuple, tag, cut=None, bins=100, **kwargs) :
    '''
    Create and return a histogram.Histogram object constructed from the 
    contents of an ntuple.NTuple column.
    '''
    from PyAna.pythistogram.histogram import Histogram, Axis
    col = ntuple.column(tag,cut)
    min_range = min(col)
    max_range = max(col)
    width = (max_range-min_range)/bins
    max_range += width/2.
    max_range -= width/2.
    histo =  Histogram(axis=Axis(bins, max_range, min_range, label=tag))
    for x in col :
        histo.fill(x)
    return histo
