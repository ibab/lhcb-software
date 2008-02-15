#!/bin/bash
python <<EE_EOF
args = '$*'.split(' ')
i=0
print 'terminal_type=xterm;help=0;dns=;subfarm=;'
while(i<len(args)):
    a = args[i][:args[i].find('=')].upper()
    #print 'echo "ARG='+a+'";'
    if a=='-DISPLAY':
	print 'export DISPLAY='+args[i][args[i].find('=')+1:]+';'
	print 'echo "export DISPLAY to '+args[i][args[i].find('=')+1:]+'";'
	print 'shift 1;'
	i = i + 1
    elif a=='-SUBFARM':
	sf = args[i][args[i].find('=')+1:]
	print 'subfarm='+sf+';echo "Chosen subfarm is: '+sf+'";shift 1;'
	i = i + 1
    elif a=='-DNS':
	dn = args[i][args[i].find('=')+1:]
	print 'dns='+dn+';echo "DIM DNS Node: '+dn+'";shift 1;'
	i = i + 1
    elif a=='-GNOM':
	print 'terminal_type=gnome; shift 1;'
	i = i + 1
    elif a=='-XTER':
	print 'terminal_type=xterm; shift 1;'
	i = i + 1
    elif a=='-HEL' or a=='-he':
	print 'help=1;'
	print 'echo " Generic options  (case insensitive, no abbreviation, must be first):";'
	print 'echo "     -DISPLAY=<display-name>   set DISPLAY environment     ";'
	print 'echo "     -SUBFARM=<sub-farm-name>  set subfarm=<sub-farm-name> ";'
	print 'echo "     -DNS=<dim-dns-server>     set dns=<dim-dns-server>    ";'
	print 'echo "     -GNOME                    set terminal_type=gnome     ";'
	print 'echo "     -XTERM                    set terminal_type=xterm     ";'
	print 'echo "     -HELP                     set help=1                  ";'
	print 'echo "  -------------------------------------------------------  ";'
        i = i + 1
    else:
	i = i + 1
EE_EOF
