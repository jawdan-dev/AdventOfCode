#include <common/common.h>
#include <openssl/evp.h>

// Note: This one needs to link against '-lcrypto' and '-lssl'.

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	EVP_MD_CTX *context;
	context = EVP_MD_CTX_create();
    unsigned int digestSize = EVP_MD_size(EVP_md5());
	unsigned char* digest = (unsigned char*)OPENSSL_malloc(digestSize);
	//
	int value;
	char update[40] = "";
	char output[(digestSize * 2) + 1];
	for (value = 0; value < 10000000; value++) {
		strcpy(update, fileReader.readBuffer);
		sprintf(update + strlen(update), "%i", value);
		//
    	EVP_DigestInit_ex(context, EVP_md5(), NULL);
    	EVP_DigestUpdate(context, update, strlen(update));
		EVP_DigestFinal_ex(context, digest, &digestSize);
		//
		for (int i = 0; i < ARRAY_SIZE(digest); i++)
			sprintf(output + (i * 2), "%.2x", digest[i]);
		//
		if (strncmp(output, "000000", 6) == 0) break; 
	}
	//
    OPENSSL_free(digest);
	EVP_MD_CTX_free(context);
	//
	printf("Secret compliment: %lli\n", value);
}