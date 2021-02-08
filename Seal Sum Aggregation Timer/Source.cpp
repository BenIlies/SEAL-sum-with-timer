#include "seal/seal.h"
#include <math.h>
#include <vector>


using namespace seal;

int main()
{
	/*
	-------------------------------------------------------------------------------
	The different parameters for the simulation are listed below

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
			This paramater reprensent the range of the values that will be randomly
			selected. For example 10, will result in values going from 0 to 9.

		- param_number_random_values :
			The number of values that will be encrypted and on which we will compute the sum.
	-------------------------------------------------------------------------------
	*/
	int param_poly_modulus_degree = 2048;
	int param_coeff_modulus = 50;
	int param_scale_power = 25;
	int param_range_random_values = 3;
	int param_number_random_values = 10000;
	std::cout << "> Starting simulation" << std::endl << std::endl;
	std::cout << "> Poly_modulus_degree : " << param_poly_modulus_degree << " bits" << std::endl;
	std::cout << "> Coeff_modulus : " << param_coeff_modulus << " bits" << std::endl;
	std::cout << "> Scale power : " << param_scale_power << std::endl;
	std::cout << "> Range values : 0 - " << param_range_random_values - 1 << std::endl;
	std::cout << "> Number of values to add : " << param_number_random_values << std::endl << std::endl;


	/*
		We start the capture of time for all the values.
		For every step we also calculate the time.
	*/
	std::chrono::high_resolution_clock::time_point time_start, time_end;
	time_start = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds time_diff;

	/*
		First we select random values and put it into a vector.
	*/
	std::chrono::high_resolution_clock::time_point time_random_selection_start, time_random_selection_end;
	time_random_selection_start = std::chrono::high_resolution_clock::now();
	std::vector <double> age;
	for (int i = 0; i < param_number_random_values; i++)
	{
		age.push_back(rand() % param_range_random_values);
	}
	time_random_selection_end = std::chrono::high_resolution_clock::now();
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_random_selection_end - time_random_selection_start);
	std::cout << "Time to select all the random values [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	/*
		We create a CKKS context with all the parameters.
		The public key can be available to everybody and will allow encryption,
		whereas the private one will decrypt the sum that was encrypted.
		In SEAL we have to select an evaluator and an encoder which will allow us to
		make operations over encrypted values.
	*/
	std::chrono::high_resolution_clock::time_point time_select_context_start, time_select_context_end;
	time_select_context_start = std::chrono::high_resolution_clock::now();
	EncryptionParameters parms(scheme_type::CKKS);
	size_t poly_modulus_degree = size_t(param_poly_modulus_degree);
	parms.set_poly_modulus_degree(poly_modulus_degree);
	parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { param_coeff_modulus }));
	double scale = pow(2.0, param_scale_power);
	auto context = SEALContext::Create(parms);
	KeyGenerator keygen(context);
	auto public_key = keygen.public_key();
	auto secret_key = keygen.secret_key();
	Encryptor encryptor(context, public_key);
	Evaluator evaluator(context);
	Decryptor decryptor(context, secret_key);
	CKKSEncoder encoder(context);
	time_select_context_end = std::chrono::high_resolution_clock::now();
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_select_context_end - time_select_context_start);
	std::cout << "Time to select the SEAL context and the keys [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	age.reserve(encoder.slot_count());

	/*
		SEAL will encode and then encrypt all the random values that were stored
		in the vector previously. After this, we store those values into another vector.
	*/
	std::chrono::high_resolution_clock::time_point time_encryption_start, time_encryption_end;
	time_encryption_start = std::chrono::high_resolution_clock::now();
	Plaintext age_result;
	Ciphertext age_encrypted_sum;
	std::vector <Ciphertext> all_age_encrypted;
	for (double i : age)
	{
		Plaintext age_plain;
		Ciphertext encryption_age;
		encoder.encode(i, scale, age_plain);
		encryptor.encrypt(age_plain, encryption_age);
		all_age_encrypted.push_back(encryption_age);
	}
	time_encryption_end = std::chrono::high_resolution_clock::now();
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_encryption_end - time_encryption_start);
	std::cout << "Time to encode and encrypt [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	/*
		Here with the evalutor we compute the sum of all the encrypted values.
	*/
	std::chrono::high_resolution_clock::time_point time_sum_start, time_sum_end;
	time_sum_start = std::chrono::high_resolution_clock::now();
	evaluator.add_many(all_age_encrypted, age_encrypted_sum);
	time_sum_end = std::chrono::high_resolution_clock::now();
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_sum_end - time_sum_start);
	std::cout << "Time to do the sum operation [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	/*
		After having done the sum of the values, we can decrypt with de private key.
	*/
	std::chrono::high_resolution_clock::time_point time_decryption_start, time_decryption_end;
	time_decryption_start = std::chrono::high_resolution_clock::now();
	decryptor.decrypt(age_encrypted_sum, age_result);
	std::vector<double> result;
	encoder.decode(age_result, result);
	double sum_expected = 0;
	for (double i : age)
	{
		sum_expected += i;
	}
	time_decryption_end = std::chrono::high_resolution_clock::now();
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_decryption_end - time_decryption_start);
	std::cout << "Time to decode and decrypt [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	/*
		Finally, we verify the correct of the CKKS scheme with the chosen parameters.
		We also display the time it took to compute all the previous operations.
	*/
	time_end = std::chrono::high_resolution_clock::now();
	std::cout << "Sum expected is : " << sum_expected << std::endl;
	std::cout << "Result is : " << result[0] << std::endl;
	time_diff = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
	std::cout << "Time to do all the computation [" << time_diff.count() << " microseconds]" << std::endl << std::endl;

	return 0;


}