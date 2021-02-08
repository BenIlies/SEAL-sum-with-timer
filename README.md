# SEAL-sum-with-timer

This C++ program allows the computation of operations over data using the SEAL library.
The version of SEAL used here is the version 3.5.

# Installation of Microsoft SEAL

You can download SEAL on https://www.microsoft.com/en-us/research/project/microsoft-seal.
I advice you to download the same version as me if you want to follow the same installation procedure.

After having downloaded SEAL from github (gitclone works better than downloading the git).
You have to open the file SEAL.sln with Microsoft Visual Studio 2017 or with a newer version.

Build SEAL with Release x64 mode.

# Adding SEAL with your own project

Create a new project in visual studio.
Open the properties of the project.
Select All Configuration.
In the C/C++ folder, General, Additional include directories add the path where the src folder was generated after having build SEAL.
In the Linker, General, Additional Library Directories add the path where the lib was generated followed by "$(Platform)\$(Configuration)".
In the Linker, Input, Additional Dependencies add "seal.lib;"
Finally in General, General Properties, C++ Language Standard replace the current version by ISO C++ 17.

Every steps are explicitly detailed on the video below.
https://www.youtube.com/watch?v=oZQ_c89HFU0

# The program itself

	The 5 different parameters for the simulation are listed below

		- param_poly_modulus_degree :
			The degree of the polynomial modulus, a positive number which must
			be a power of 2. The greater it is the more operation you can compute.
			Nevertheless, it will result in slower computations.

		- param_coeff_modulus :
			The coefficient modulus is a prime number up to 60 bits.
			In this example we use a unique prime number but they can be assembled
			in vector following the table.

			+----------------------------------------------------+
			| poly_modulus_degree | max coeff_modulus bit-length |
			+---------------------+------------------------------+
			| 1024                | 27                           |
			| 2048                | 54                           |
			| 4096                | 109                          |
			| 8192                | 218                          |
			| 16384               | 438                          |
			| 32768               | 881                          |
			+---------------------+------------------------------+

		- param_scale_power :
			The scale is basically the precision of the computation result.
			The scale is bounded to a certain value depending on the coeff_modulus
			length.

		- param_range_random_values :
			This paramater reprensents the range of the values that will be randomly
			selected. For example 10, will result in values going from 0 to 9.

		- param_number_random_values :
			The number of values that will be encrypted and on which we will compute the sum.