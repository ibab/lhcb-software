#!/usr/bin/env python
import os, sys, optparse
from XMLSummaryBase import summary

def find_counters( summary ):
    d = dict()
    counters = summary.find( "counter", "name" )
    for counter in counters:
        name = counter.attrib()[ 'name' ]
        d[ name ] = counter
    return d

# Setup the option parser
usage = "usage: %prog directory"
parser = optparse.OptionParser( usage = usage )

( options, args ) = parser.parse_args()

d = os.path.expandvars( args[ 0 ] )

files = os.listdir( d )

# Get all the summary files from the directory
summary_files = []
for filename in files:
    if filename.find( "summary.xml" ) != -1:
        summary_files.append( filename )

if not "allLines_summary.xml" in summary_files:
    print "No allLines summary in " + d
    sys.exit( 1 )
else:
    summary_files.remove( "allLines_summary.xml" )

# Create and parse the allLines summary
all_sum = summary.Summary()
all_sum.parse( d + "/allLines_summary.xml" )

# Put the counters in a dict
all_counters = find_counters( all_sum )
    
# Create the summary objects and parse the single line summaries
summaries = dict()
for single_filename in summary_files:
    s = summary.Summary()
    name = single_filename.split( "_" )[ 0 ]
    s.parse( d + "/" + single_filename )
    summaries[ name ] = s

for name, s in summaries.items():
    single_counters = find_counters( s )
    for counter_name, single_counter in single_counters.items():
        all_counter = all_counters[ counter_name ]
        all_value = all_counter.value()
        single_value = single_counter.value()
        if all_value != single_value and counter_name.find( "#accept" ) != -1:
            print name + " " + counter_name + " " + str( all_value ) + " " + str( single_value )
