import string 	#string module
import re	#regular expressions module

from cdbVisCore import *
from MyExceptions import *

""" This module is a control/validation module with all the different validations on both user 
input and database input for all different data types. To prevent from trying to add "illegal"
data to the database.
"""

def ValidateString(name,thestring,maxlength=-1,allowzerolength=False,regexprmask=""):
    """ Validates string input to the given conditions.
    
	Parameters:
	@name - to be used in exception handling, identification that will be used to the output
	        to the user so he/she will understand what/where went wrong.
	@thestring - string to be validated
    	@maxlength - the max length of the string, -1 means no limit
    	@allowzerolength - if the string can have length of zero; boolean true or false
    	@regexprmask - a python regular expression mask, if the string needs to have a special syntax 
    	
	!Return:
	If the string satisfy the given conditions it is returned, else and exception is thrown.
    """

    thestring = str(thestring).strip() #strip leading and trailing white space

    #Length test
    if maxlength != -1:
        if len(thestring) > maxlength:
	    raise StringError, str(name) + " : The length of the text " + str(len(thestring)) + " is longer than the defined maximum size (" + str(maxlength) + ")"

    #Zero length test
    if allowzerolength==False:
        if len(thestring) == 0:
	    raise StringError, str(name) + ": Zero length text not allowed here."

    #Mask test
    if regexprmask != "":
	rematch = re.compile(regexprmask)
	if rematch.match(thestring) != True:
	    raise StringError, str(name) + ": The input given is not syntactically correct."

    #Our string went through all given tests
    return thestring

def ValidateNumber(name,thenumber,minvalue=None,maxvalue=None,allowdecimal=False):
    """ Validates a number with the given constraints.
    
	Input:
	@minvalue - the smallest value allowed
	@maxvalue - the biggest value allowed
	@allowdecimal - whether decimal (float) numbers are allowed or not
	
	!Return:
	True is returned if validation was successful, else we throw an exception.
    """


    #Check if we have a number and not a string, boolean etc.
    #if allowdecimal == False:
    #matchstring = "r'^\-?\d+$'" #negative or positive integer
    #else:
    #matchstring = "r'^\-?\d+(\.\d+)?$'" #negative or positive decimal or integer

    #rematch = re.compile(matchstring)
    #if rematch.match(thenumber) != True:
    #return False

    if allowdecimal:
        if type(thenumber) != type(1) and type(thenumber) != type(1.0):
	    raise NumberError, str(name) + ": Illegal input. Not a legal number. Integers and floats allowed only."
    else:
	if type(thenumber) != type(1):
	    raise NumberError, str(name) + ": Illegal input. Not a integer. Integers allowed only."

    if minvalue != None:
	if thenumber < minvalue:
	    raise NumberError, str(name) + ": Illegal input. Smallest value allowed is " + str(minvalue) + "."
    if maxvalue != None:
	if thenumber > maxvalue:
	    raise NumberError, str(name) + ": Illegal input. Biggest value allowed is " + str(maxvalue) + "."

    #Yeah, this is the number we want
    return True

def ValidateSystem(name,thesystem):
    """ Test if the systemname is valid, we have 12 different names to test against;
        set in the cdbVisCore. Case-sensitive.
	
	Parameters:
	@thesystem - name of the system
	
	!Return:
	The name of the system itself if legal, or an exception is thrown otherwise
    """

    validsystems = allvalidsystems #allvalidsystems is set in cdbVisCore module

    if thesystem in validsystems:
	return thesystem
    else:
	if string.find(thesystem,",",0) != -1:
		thesystem = thesystem.split(",")
		for system in thesystem:
			if system not in validsystems:
				raise SystemNameError, str(name) + ": The given input is not a valid list of subsystems."
				
	else:
		raise SystemNameError, str(name) + ": The given input is not a valid system name."
