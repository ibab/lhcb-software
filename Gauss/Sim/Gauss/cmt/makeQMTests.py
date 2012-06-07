#!/usr/bin/env python
#
#  makeQMTests.py
#  Author   Emmanouil Kiagias, 6 June 2012
#
#  Make copy of qmtests starting from a template and changing the
#  event type for which they are made.
#
#  By default the template and the tests are in the $GAUSSROOT/tests/qmtest
#  directory.
#  The list of event types for which to make the test is taken from a file
#  that by default is in the $DECFILESROOT directory
#
#  For more information, makeQMTests.py -h
#
import re, os, sys, logging
from optparse import OptionParser
#default values
INPUT_FILE=os.environ['DECFILESROOT']+'/doc/newdecfiles.txt'
INPUT_TEMPLATE=os.environ['GAUSSROOT']+'/tests/qmtest/templates/gauss-gen-EVTTYPE.qmt'
OUTPUT_PATH=os.environ['GAUSSROOT']+'/tests/qmtest/gauss.qms'

#regex of words given as input
input_regex = r'[0-9]{8}'
#regex to word which will be changed
change_regex = r'@\[eventtype\]'

def main():
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)
    parser.add_option( "-i" , "--input" , type="string" , 
                    dest="input_file" , default=INPUT_FILE, 
                    help="Path to input data file [ default: %default ]") 
    parser.add_option( "-t" , "--template-input" , type="string" ,
                    dest="input_template" , default=INPUT_TEMPLATE, 
                    help="Path to general template model [ default: %default ]") 
    parser.add_option( "-o" , "--output-path" , type="string" , 
                    dest="output_path" , default=OUTPUT_PATH ,
                    help="Path to save the output files [ default: %default ]") 
    parser.add_option("-q", "--quiet", action="store_true",
                      dest="ssss", default=True,
                      help="Just be quiet (do not print info from logger)")
    
    (options, args) = parser.parse_args()
        
    #set the logger either to print info or not
    if not options.ssss:
        logging.root.setLevel(logging.DEBUG)
        
    logging.info('Using INPUT_FILE: '+options.input_file)
    logging.info('Using INPUT_TEMPLATE: '+options.input_template)
    logging.info('Using OUTPUT_PATH: '+options.output_path+'\n')
    
    #check the validity of the arguments
    if not os.path.isdir(options.output_path):
        logging.error("Invalid --output-path(-o) , give an existing directory!\n")
        return

    #check if the input files exist and if they are empty
    try:
       filesize = os.path.getsize(options.input_file)
    except OSError:
        logging.error("Input data file doesn't exist!\n")
        return
    else:
        if not filesize > 0:
            logging.error("Input file data is empty\n")
            return            
    try:
       filesize2 = os.path.getsize(options.input_template)
    except OSError:
        logging.error("Input template file doesn't exist!\n")
        return
    else:
        if not filesize2 > 0:
            logging.error("Input template file is empty\n")
            return
    
    #read the input data (find regex) and in the input template(model)    
    input_data = open(options.input_file,'r').read()
    model = open(options.input_template,'r').read()

    matches = re.finditer(input_regex,input_data)
    
    #check if matches where found
    temp_list = list(matches)
    if len(temp_list) == 0:
        logging.error("No regex matches found in input data file: "+options.input_file+" .\n")
        return
    else:
        matches = iter(temp_list)
    
    for match in matches:
        #final path and also fix the name of the output file
        final_path = options.output_path+os.sep+'gauss-evt-'+match.group(0)+'.qmt'  
        out = open(final_path, "w")
        out.write( re.sub(change_regex, match.group(0), model) )
        out.close()
        logging.info('File created :'+options.output_path+os.sep+'gauss-evt-'+match.group(0)+'.qmt')
        
if __name__ == "__main__":
    main()
