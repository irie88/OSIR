#include "common.h"
#include "weakcrypt.h"
#include "thirdparty/b64.h"
#include "web.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int decryptRemoteBytes(char **decrypted_msg, const char *url)
{
	//get encrypted message
	char *encrypted_msg = getHypertext(url);

	//Decode message
	char *decoded_msg;
	unsigned int raw_msg_size = b64decode(&decoded_msg, encrypted_msg);

	//Deallocate and allocate

	*decrypted_msg =
	    (char *) malloc(sizeof(char) * (raw_msg_size - XOR_KEY_SIZE));
	char key[XOR_KEY_SIZE];

	//Decrypt message
	for (unsigned int i = 0; i < raw_msg_size; i++) {
		if (i < XOR_KEY_SIZE) {
			key[i] = decoded_msg[i];
		} else {
			//XOR with key
			(*decrypted_msg)[i - XOR_KEY_SIZE] =
			    decoded_msg[i] ^ key[(i - XOR_KEY_SIZE) %
						 XOR_KEY_SIZE];
		}
	}

	//Perform one-byte Integrity check
	if ((*decrypted_msg)[raw_msg_size - XOR_KEY_SIZE - 1] !=
	    ((*decrypted_msg)[0] ^
	     (*decrypted_msg)[raw_msg_size - XOR_KEY_SIZE - 2])) {
		fprintf(stderr,
			"Error: getRemoteBytes: integrity check failed(%s)\n",
			encrypted_msg);
		free(encrypted_msg);
		return -1;
	}
	//Deallocate memory
	free(decoded_msg);
	free(encrypted_msg);
	return raw_msg_size - XOR_KEY_SIZE - 1;
}

void hash16(char *hash, const char *input, const int length)
{
	const int hash_length = 2;
	memset(hash, 0, hash_length);
	for (int i = 0; i < length; i++) {
		hash[i % hash_length] ^= input[i];
	}
}
