#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t UINT8;
typedef uint32_t UINT32;

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

#define CIPHER_KEY "./Key.bin"
#define CIPHER_FILE "./EncryptedMessage.bin"

int main()
{

	int key_fd, cipher_fd;
	int retval;
	int size;
	struct EncryptionStepDescriptor cur_desc;
	char * buffer;
	int pos = 0;

	bool up = true;

	key_fd = open(CIPHER_KEY, O_RDONLY);
	cipher_fd = open(CIPHER_FILE, O_RDONLY);

	if ((!key_fd) || (!cipher_fd))
		return -1;

	struct stat st;
	stat(CIPHER_FILE, &st);
	size = st.st_size;
	buffer = malloc(size);

	if (!buffer) {
		perror("No memeory");
		return -1;
	}

	retval = read(cipher_fd, buffer, size);
	if (retval != (int) size) {
		printf("Problem reading cipher to memory\r\n");
		return -1;
	}
	while (1) {

		retval = read(key_fd, &cur_desc, sizeof(cur_desc));
		if (retval != sizeof(cur_desc))
			break;

		printf("Descriptor: OPCODE: %u PARAM: %u LENGTH: %u\r\n", cur_desc.operationCode, cur_desc.operationParameter, cur_desc.lengthToOperateOn);

		switch (cur_desc.operationCode)
		{
			case OPCODE_ADD:
				for (uint32_t i=0; i< cur_desc.lengthToOperateOn; i++) {
					buffer[pos]+= cur_desc.operationParameter;

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
				for (uint32_t i=0; i< cur_desc.lengthToOperateOn; i++) {
					buffer[pos]-= cur_desc.operationParameter;
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
				for (uint32_t i=0; i< cur_desc.lengthToOperateOn; i++) {
					buffer[pos] ^= cur_desc.operationParameter;
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


	for (int i = 0; i < size; i++)
		printf("%c", buffer[i]);
	return 0;

}


