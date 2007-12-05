###################################################################################
"""
PyTree - 	TTree derived class with simplified booking and setting methods. 

			TODO: extend for 2 dim arrays/vectors


Gabriel Guerrer - 27/06/07
"""
###################################################################################	

from ROOT import TTree, std
from array import array

ARRAYMAXSIZE = 1000 # used in arrays of variable lenght

#---------------------------------------------------
# PyTree
#---------------------------------------------------
class PyTree(TTree):
	"""
	TTree derived class with simplified booking and setting methods.
	PyTree is stored as TTree in a ROOT file.
	
	Usage example:
	
	from PyTree import *
	t = PyTree('t', '')
	tree_vars = ['n/I', 'a/F', 'x[2]/F', 'y[n]/F', 'z<>/F']

	# book your vars
	for var in tree_vars:
		t.book(var) 

	# loop over your events
	for ...:	

		t.setAll0()	 # in case you dont fill any of the tree vars

		t.set('n', 3)
		t.set('a', 3.141592)
		t.set('x', [0, 1])
		t.set('y', [0, 1, 2])
		
		# for setting the std::vector, one could pass a Python list
		t.set('z', [0, 1, 2, 3, 4, 5])
		
		# or a std::vector as well
		z = std.vector(float)()
		z.push_back(0) # ...
		t.set('z', z)

		t.Fill() # always fill in the end of the event loop
	
	"""

	varnames = []	# internal branch names

	#---------------------------------------------------
	def book(self, label): 			#general label = "varname[size]/type"
		"""
		Simplified booking. Possible uses:
		
		- label = "varname/type"
		- label = "varname[100]/type"
		- label = "varname[othervarname]/type"
		- label = "varname<>/type"
		
		Where type = I, F.
		The last example shows the possibility of working with std::vector<type>.
		
		A internal class member is created with name varname to hold the var values.
		
		"""		

		# If a list of labels is given as input, call book n times
		if isinstance(label, list):
			for iLabel in label:
				self.book(iLabel)
		else:
	
			# Start parsing
			i0 = label.find("/")
			i1 = label.find("[")
			i2 = label.find("]")
			i3 = label.find("<>") # vector

			# type check, I or F
			typ = label[i0+1:]		
			if not (typ == 'I' or typ == 'F'):
				print 'PyTree ERROR - var type ' + typ + ' not defined. Please use I or F.'		
				return
			typ = typ.lower()	# compatibility between array and TTree:Branch		

			# label = "varname<>/type"
			if i3 > -1:
				varname = label[:i3]
				if len(varname) == 0:
					print 'PyTree ERROR - you must supply a varname.'		
					return						
				if		typ == 'i': typ = 'int'
				elif 	typ == 'f': typ = 'float'			
				var = std.vector(typ)()				
				setattr(self, varname, var) # create internal varname to hold var
				self.Branch(varname, 'vector<'+typ+'>', var)				

			# label = "varname/type"
			elif i1 == -1 and i2 == -1:
				varname = label[:i0]
				if len(varname) == 0:
					print 'PyTree ERROR - you must supply a varname.'		
					return						
				var = array(typ, [0])			
				setattr(self, varname, var)
				self.Branch(varname, var, label)

			else:	
				size = label[i1+1:i2]
				varname = label[:i1]
				if len(varname) == 0 or len(size) == 0:
					print 'PyTree ERROR - you must supply varname and size.'		
					return						

				#label = "varname[100]/type"
				try:
					sizeval = int(size)
					var = array(typ, sizeval*[0])

				#label = "varname[othervarname]/type"
				except ValueError: 
					var = array(typ, ARRAYMAXSIZE*[0])

				setattr(self, varname, var)
				self.Branch(varname, var, label)		

			# append to internal branch names
			self.varnames.append(varname)		

	#---------------------------------------------------
	def get(self, varname):
		"""
		Return content of varname.
		"""
	
		return getattr(self, varname)	
		
	#---------------------------------------------------
	def set(self, varname, values):
		"""
		Set values in varname. An extra Fill() call is necessary to write values in the Tree.
		
		For setting arrays and vectors, one can pass python lists.		
		
		"""
	
		# check if varname exists
		if not hasattr(self, varname):
			print 'PyTree ERROR - setting an inexistent var ', varname
			return 

		var = getattr(self, varname)			
		#leafsize = self.GetLeaf(varname).GetLen()			
		
		# label = "varname<>/type"
		if str(type(var)).find('vector') != -1:		
			# values == int, float			
			if isinstance(values,int) or isinstance(values,float):
				var.clear()
				var.push_back(values)
			# values == list, vector			
			else:			
				var.clear()
				for value in values:
					var.push_back(value)
		
		# label = "varname[]/type"
		elif isinstance(values,list) or isinstance(values,tuple):		
			#if leafsize != len(values) and values != 0:
			#	print 'PyTree - ERROR: trying to set ', len(values), ' dim value in ',  varname,  ', leafsize == ',  leafsize			
			n = min(len(values),ARRAYMAXSIZE)
			for i in range(n):
				var[i] = values[i]
				
		# label = "varname/type"
		else:
			#if leafsize != 1:
			#	print 'PyTree - ERROR: trying to set 1 dim value in ', varname, ', leafsize == ', leafsize
			for i in range(len(var)):
				var[i]= values	
			
	#---------------------------------------------------
	def setAll0(self):
		"""
		Must be used in the beginning of event loop, otherwise non setted vars will
		fill the previous event content.
		"""
	
		for var in self.varnames:
			self.set(var, 0)
