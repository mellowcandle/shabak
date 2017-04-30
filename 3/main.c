#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t UINT8;
typedef uint32_t UINT32;
void print(const char * msg, int size);

struct __attribute__((packed)) EncryptionStepDescriptor
{
	UINT8 operationCode;
	UINT8 operationParameter;
	UINT32 lengthToOperateOn;
};

typedef enum {
	OPCODE_XOR = 0x00,
	OPCODE_ADD,
	OPCODE_SUB,
} eOpcode;


bool valid(const char * msg, int length)
{
	for (int i=0; i < length; i++)
	{
		if (!isalpha(msg[i]) && msg[i] != ' ' && msg[i] != '.' && msg[i] != '?')
			return false;
	}

	return true;

}


#define CIPHER_KEY "./Key.bin"
#define CIPHER_FILE "./EncryptedMessage.bin"

int main()
{


	int a,b,c,d,e,f,g,h,j;

	int cipher_fd;
	int retval;
	int size;
	struct EncryptionStepDescriptor key[] =
	{
		{OPCODE_XOR,1,1},
		{OPCODE_SUB,1,1},
		{OPCODE_XOR,1,1},
	};
	char * buffer;
	char * orig_buffer;
	int pos = 0;

	bool up = true;

	cipher_fd = open(CIPHER_FILE, O_RDONLY);

	if (!cipher_fd)
		return -1;

	struct stat st;
	stat(CIPHER_FILE, &st);
	size = 5;//st.st_size;
	buffer = malloc(size);
	orig_buffer = malloc(size);

	if (!buffer) {
		perror("No memeory");
		return -1;
	}

	retval = read(cipher_fd, orig_buffer, size);
	if (retval != (int) size) {
		printf("Problem reading cipher to memory\r\n");
		return -1;
	}

	memcpy(buffer, orig_buffer, size);

	for (int i=0;i<256;i++) {
	for (int s=0; s < 256;s++) {
		for(int j=0;j < size;j++) {
			buffer[j] +=s;
			buffer[j] ^=i;
			}

		if (valid(buffer,size))
			print(buffer, size) ;
		memcpy(buffer, orig_buffer, size); 
	}
	}

	return 0;


	for (a = 0; a < 3; a++)
		for (b= 0; b < 3; b++)
			for (c = 0; c < 3; c++)
				for (d = 1; d < 255;d++) {
						printf("d = %u\n", d);
					for (e = 1; e < 255; e++) 
						for (f = 1; f < 255; f++)
							for (g=1; g < 5; g++)
								for (h=1;h<5;h++)
								for (j=1;j<5;j++)
									{
#if 1
	key[0].operationCode=a;
	key[1].operationCode=b;
	key[2].operationCode=c;
#endif
	key[0].operationParameter = d;
	key[1].operationParameter = e;
	key[2].operationParameter = f;

	key[0].lengthToOperateOn = g;
	key[1].lengthToOperateOn = h;
	key[2].lengthToOperateOn = j;
	/* Start with a clean buffer */
	memcpy(buffer, orig_buffer, size);

	for (int i = 0; i < 3; i++)
	{
//		printf("Descriptor: OPCODE: %u PARAM: %u LENGTH: %u\r\n", key[i].operationCode, key[i].operationParameter, key[i].lengthToOperateOn);

		switch (key[i].operationCode)
		{
			case OPCODE_ADD:
				for (uint32_t i=0; i< key[i].lengthToOperateOn; i++) {
					buffer[pos]+= key[i].operationParameter;

					if (up)	pos++;
						else
							pos--;

						if (pos == -1) {
							pos += 1;
							up = true;
						}
						if (pos == size) {
							pos-=1;
							up = false;
						}

		}
				break;
			case OPCODE_SUB:
				for (uint32_t i=0; i< key[i].lengthToOperateOn; i++) {
					buffer[pos]-= key[i].operationParameter;
					if (up)	pos++;
						else
							pos--;

						if (pos == -1) {
							pos += 1;
							up = true;
						}
						if (pos == size) {
							pos-=1;
							up = false;
						}
					}
					break;
			case OPCODE_XOR:
				for (uint32_t i=0; i< key[i].lengthToOperateOn; i++) {
					buffer[pos] ^= key[i].operationParameter;
					if (up)	pos++;
						else
						pos--;

						if (pos == -1) {
							pos += 1;
							up = true;
						}
						if (pos == size) {
							pos-=1;
							up = false;
						}
				}
					break;

			default:
				printf("Unknown opcode\r\n");
				return -1;
		}
	}

	if (valid(buffer, size)) {
		print(buffer,size);
		return 0;

	}
									}
				}

}

void print(const char * msg, int size)
{
	for (int i = 0; i < size; i++)
		printf("%c", msg[i]);

	printf("\n");
}
