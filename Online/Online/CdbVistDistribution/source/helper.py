###################################################################################################################
# This module is a container class for functions that does not belong to any particular module. A collection of ###
# useful functions that can be used in any module.								###
###################################################################################################################

#from wxPython.wx import *

OC = __import__('objectclasses') # We have to import the objectclasses module this way (using the load module function)
				 # because or else we will get a recursive including/importing of objectclasses and helper


def flatten(my_list):
	""" Flattens a list. If we f.ex. have a list like [[[2],2],[[[3,3],4],4],5,6], 
	then this function will convert it to [2,2,3,3,4,4,5,5] recursively. Because 
	it is done recursively the list shouldn't have too many levels.
	"""

    	if type(my_list) != type([]): 
		return [my_list]
	if my_list == []: 
		return my_list

    	return flatten(my_list[0]) + flatten(my_list[1:])

def prepostquotes(thestring):
	""" prepends and appends a quote sign to a string, and returns it.

	@string - the string that will be quoted

	!return - the quoted string
	"""

	return '"' + thestring + '"'
'''
class TextDropTarget(wxTextDropTarget):
	""" Drag and drop text functionality class.
	
	    Here we define what that should be done on drop
	"""

	def __init__(self,obj,mainwin):
		""" Constructor.

		    Parameters:
		    @obj     - The object associated with the drop
		    @mainwin - The MainWindow instance (for communication)
		"""
		
		wxTextDropTarget.__init__(self)

		self.obj = obj
		self.appwin = mainwin

	def OnDropText(self,x,y,data):
		""" Method that is invoked on the drop of an object.

		    Parameters:
		    @x - x position (coord) of the dropped object
		    @y - y position (coord) of the dropped object
		    @data - data (string) set to the dropped object
		"""
		
		tmp_info = data.split("|") # We have defined a string to be associated with every drag and drop object
					   # First part is the systemname, second part the name of the object type dropped
					   # They are separated by a '|'.

		if tmp_info[2] == "Device":
			tmp_devices = self.appwin.CreateDeviceClick(None) # Open Create Device Window

			if tmp_devices != False and len(tmp_devices) > 0: # If we created one or more devices...
				i = 0
				for tmp_device in tmp_devices: # Lay them out nicely in the visual window
					x_offset,y_offset = self.appwin.GetVisWindow().CalcUnscrolledPosition(x,y+i)
					self.appwin.GetVisWindow().CreateNode(x_offset,y_offset,tmp_device)
					i += 5

		else: #So far, only support for Devices...
			print "Dropped an unknown object type"
				
'''