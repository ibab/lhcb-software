##################################################
#   Custom exception classes for CdbVis		##
#						##
##################################################

class DBError(Exception):
	""" Exception class for ConfDB query and db errors.
	"""

	def __init__(self,*args):
		self.args = args
		self._type = "database"

	def __str__(self):
		return "A " + str(self._type) + " error occured. The returned message from the ConfDB was:  " + str(self.args[0])

class DBConnectionError(DBError):
	""" Exception class for ConfDB connection errors.
	"""

	def __init__(self,*args):
		DBError.__init__(self,*args)
		self._type = "connection"


class ValidationError(Exception):
	""" Exception class for all Validation errors.
	"""
	
	def __init__(self,*args):
		self.args = args
		self._type = "validation"

	def __str__(self):
		return "A " + str(self._type) + " error occured: " + str(self.args[0])

class StringError(ValidationError):
	""" Exception class for String Validation errors.
	"""
	
	def __init__(self,*args):
		self.args = args
		self._type = "string validation"

class NumberError(ValidationError):
	""" Exception class for Number (Integer and Float) validation errors.
	"""
	
	def __init__(self,*args):
		self.args = args
		self._type = "number validation"

class SystemNameError(ValidationError):
	""" Exception clas for Subsystem name validation errors.
	"""
	
	def __init__(self,*args):
		self.args = args
		self._type = "system name validation"
