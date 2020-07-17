..
.. NB:  This file is machine generated, DO NOT EDIT!
..
.. Edit vmod.vcc and run make instead
..

.. role:: ref(emphasis)

.. _vmod_fiftyonedegrees(4):

====================
vmod_fiftyonedegrees
====================

-------------------------------
51Degrees Device Detection VMOD
-------------------------------

:Manual section: 4

SYNOPSIS
========


::

   import fiftyonedegrees [from "path"] ;
   
   VOID start(STRING filePath)
  
   VOID reload([STRING filePath])
  
   STRING match_all(STRING properties)
  
   STRING match_single(STRING userAgent, STRING properties)
  
   VOID set_properties(STRING properties)
  
   VOID set_delimiter(STRING delimiter)
  
   VOID set_performance_profile(STRING performanceProfile)
  
   VOID set_drift(INT drift)
  
   VOID set_difference(INT difference)
  
   VOID set_max_concurrency(INT concurrency)
  
   VOID set_allow_unmatched(STRING allowUnmatched)
  
   VOID set_use_performance_graph(STRING usePerformanceGraph)
  
   VOID set_use_predictive_graph(STRING usePredictiveGraph)
  
   STRING get_dataset_name()
  
   STRING get_dataset_format()
  
   STRING get_dataset_published_date()
  


DESCRIPTION
===========

Prototype 51Degrees device detection module for Varnish 6.0

Implements the 51Degrees match functionality as a VMOD.
Initialise the data set with
fiftyonedegrees.start(datafilepath),
then get a match property from a requesting device with
fiftyonedegrees.match_*(propertyname).

CONTENTS
========

* :ref:`func_get_dataset_format`
* :ref:`func_get_dataset_name`
* :ref:`func_get_dataset_published_date`
* :ref:`func_match_all`
* :ref:`func_match_single`
* :ref:`func_reload`
* :ref:`func_set_allow_unmatched`
* :ref:`func_set_delimiter`
* :ref:`func_set_difference`
* :ref:`func_set_drift`
* :ref:`func_set_max_concurrency`
* :ref:`func_set_performance_profile`
* :ref:`func_set_properties`
* :ref:`func_set_use_performance_graph`
* :ref:`func_set_use_predictive_graph`
* :ref:`func_start`


.. _func_start:

VOID start(STRING filePath)
---------------------------

Return value
	VOID
Description
	Initialises the 51Degrees resource manager with the data file path
	provided.
Example
        ::

                fiftyonedegrees.start("/path/to/51Degrees.dat");


.. _func_reload:

VOID reload([STRING filePath])
------------------------------

Return value
	VOID
Description
	Reload the data set being used by the resource manager by using
	the given data file path. If no data file path is given, the original
	file path will be used. The data set is reloaded with the configuration
	when the resource manager was first initialised.


.. _func_match_all:

STRING match_all(STRING properties)
-----------------------------------

Return value
	STRING The value of the requested property or properties for the
	matched device. Multiple values for the name property are separated
	with "|", and values for different properties are separated by the
	delimiter set with set_delimiter or "," by default.
Description
	Returns the values(s) of property provided for the requesting device
	using all important HTTP headers. Multiple properties should be
	separated with a comma.
Example
	::

		set req.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");


.. _func_match_single:

STRING match_single(STRING userAgent, STRING properties)
--------------------------------------------------------

Return value
	STRING The value of the requested property or properties for the
	matched device. Multiple values for the same property are separated
	with "|", and values for different properties are separated by the
	delimiter set with set_delimiter or "," by default.
Description
	Returns the value of property provided for the requesting device
	using the User-Agent provided. Multiple properties should be
	separated with a comma.
Example
	::

		set req.http.X-IsMobile = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");


.. _func_set_properties:

VOID set_properties(STRING properties)
--------------------------------------

Description
	Limits the properties which are initialised to the ones set
	in this function. The function takes a comma separted lisst of
	properties.
Example
	::

		fiftyonedegrees.set_properties("IsMobile,BrowserName");


.. _func_set_delimiter:

VOID set_delimiter(STRING delimiter)
------------------------------------

Description
	Sets the delimiter to use when returning values for multiple properties.
	If not set, then a comma is used by default.
Example
	::

		fiftyonedegrees.set_delimiter("/");


.. _func_set_performance_profile:

VOID set_performance_profile(STRING performanceProfile)
-------------------------------------------------------

Descripton
	Dictates how device detection allocates memory and manage data.
	The possible profiles are "DEFAULT", "IN_MEMORY", "HIGH_PERFORMANCE",
	"LOW_MEMORY", "BALANCED". By default the "DEFAULT" profile is used.
Example
	::

		fiftyonedegrees.set_performance_profile("IN_MEMORY")


.. _func_set_drift:

VOID set_drift(INT drift)
-------------------------

Description
	Set the drift value to allow the sub-strings to be matched in
	a wider range of character positions. By default the value is 0.
Example
	::

		fiftyonedegrees.set_drift(10)


.. _func_set_difference:

VOID set_difference(INT difference)
-----------------------------------

Description
	Set the difference value to allow User-Agents where characters are
	slightly different from what is expected. By default the value is 0.
Example
	::

		fiftyonedegrees.set_difference(10)


.. _func_set_max_concurrency:

VOID set_max_concurrency(INT concurrency)
-----------------------------------------

Description
	Set the expected concurrent detection requests. This will allow
	the internal caches, used by the detection process to be constructed
	in a way that will allow this level of concurrency. This should
	be the expected frequency of number of worker threads being deployed.
	This will override what being set by the performance profile.
Example
	::

		fiftyeonedegrees.set_max_concurrency(10)


.. _func_set_allow_unmatched:

VOID set_allow_unmatched(STRING allowUnmatched)
-----------------------------------------------

Description
	Set whether unmatched node should be allowed. Potential values are
	"YES" and "NO". If this value is set to "YES", which meansthe
	unmatched node will be used and a default value is returned in
	the case no match is found. If "NO" is set, an empty string will
	returned. By default "NO" is used.
Example
	::

		fiftyonedegrees.set_allow_unmatched("NO")


.. _func_set_use_performance_graph:

VOID set_use_performance_graph(STRING usePerformanceGraph)
----------------------------------------------------------

Description
	Set whether performance optimized graph should be used. The
	potential values are "YES" and "NO". The default value is determined
	by the performance profile being used.
Example
	::

		fiftyonedegrees.set_use_performance_graph("NO")


.. _func_set_use_predictive_graph:

VOID set_use_predictive_graph(STRING usePredictiveGraph)
--------------------------------------------------------

Description
	Set whether predictive optimized graph should be used. The potentinal
	values are "YES" and "NO". The default value is determinedby the
	performance profile being used.
Example
	::

		fiftyonedegrees.set_use_predictive_graph("NO")
	


.. _func_get_dataset_name:

STRING get_dataset_name()
-------------------------

Description
	Returns the name of the dataset being used e.g. Premium.


.. _func_get_dataset_format:

STRING get_dataset_format()
---------------------------

Description
	Returns the format of the dataset being used e.g. HashV41.


.. _func_get_dataset_published_date:

STRING get_dataset_published_date()
-----------------------------------

Description
	Returns the date the dataset was published in the format YYYY-MM-DD


COPYRIGHT
=========

::

  This Original Work is copyright of 51 Degrees Mobile Experts Limited.
  Copyright 2020 51 Degrees Mobile Experts Limited, 5 Charlotte Close,
  Caversham, Reading, Berkshire, United Kingdom RG4 7BY.
 
  This Original Work is licensed under the European Union Public Licence (EUPL) 
  v.1.2 and is subject to its terms as set out below.
 
  If a copy of the EUPL was not distributed with this file, You can obtain
  one at https://opensource.org/licenses/EUPL-1.2.
 
  The 'Compatible Licences' set out in the Appendix to the EUPL (as may be
  amended by the European Commission) shall be deemed incompatible for
  the purposes of the Work and the provisions of the compatibility
  clause in Article 5 of the EUPL shall not apply.
  
  If using the Work as, or as part of, a network application, by 
  including the attribution notice(s) required under Article 5 of the EUPL
  in the end user terms of the application under an appropriate heading, 
  such notice(s) shall fulfill the requirements of that article.
 
