#!/bin/python

def saneHex(string):
	#cut away leading 0x
	hex = string.replace('0x','')
	return '0x%(h)08d'% {'h':int(hex)}

import sys

# guess that the only given argument is "--help"
if (1<len(sys.argv)) :
   print "USAGE:"
   print "  SetupProject Moore"
   print "  python makeZooList.py "
   print ""
   print "NB: * only use this if you make a trigger list for a new release."
   print "    * this is a script for maintainers, not for users"
   sys.exit()


from TCKUtils.utils import *

tcklist = [0x2e002a, 0x2a002a, 0x2e002c, 0x2b002a, 0x25002c, 0x1f0029, 0x1e0030, 0x190024, 0x19001f]
hlt1list = []
hlt2list = []

for tck in tcklist:
	tmphlt1 = getHlt1Lines(tck)
	for line in tmphlt1:
		if not hlt1list.count(line):
			hlt1list += [line]
	tmphlt2 = getHlt2Lines(tck)
	for line in tmphlt2:
		if not hlt2list.count(line):
			hlt2list += [line]

word = 1
byte = 0
byte_count = 0
print '//HLT1 Lines\n\n'
for i in hlt1list:
	byte = pow(2, byte_count)
	byte_count += 1
	name = i.replace('Hlt::Line/','')
	if name.find('Global') == -1:  
		#string = 'MakeTriggerDec(' + name + '\t\t\t' +repr(word) + ',' + hex(byte) + ',"' + name + 'Decision);'
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)sDecision");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	else:
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)s");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	

	print string
	
	if byte_count == 32:
		byte_count = 0
		word +=1
		print '\n'

print '\n//HLT2 Lines\n\n'
word += 1
byte = 0
byte_count = 0
for i in hlt2list:
	byte = pow(2, byte_count)
	byte_count += 1
	name = i.replace('Hlt::Line/','')
	if name.find('Global') == -1:  
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)sDecision");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	else:
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)s");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	print string
	
	if byte_count == 32:
		byte_count = 0
		word +=1
		print '\n'

