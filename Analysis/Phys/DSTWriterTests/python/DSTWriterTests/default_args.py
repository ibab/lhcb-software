"""
Standard set of command line options for scripts analysing DSTs.
Options:

-v, --verbose
-o, --output  : Output file name to which summary is written [default 'dst_summary.txt
-r, --root    : TES root where scan of contents begins [default '/Event']

Usage:
from DSTWriterTests.default_args import parser
# add extre options
parser.add_option(...)
# parse
(options, args) = parser.parse_args()

author: Juan Palacios palacios@physik.uzh.ch

"""

__all__ = ('parser')

from optparse import OptionParser

parser = OptionParser()

parser.add_option('-o', '--output', type='string', dest='output_file', default='dst_summary.txt', help='Name of output file with contents summary [default: %default]')

parser.add_option('-r', '--root', type='string', dest='tes_root', default='/Event', help='TES root to explore [default: %default]')

parser.add_option('-v', '--verbose', action='store_true', dest='verbose')

