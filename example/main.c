#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../seif.h"

#define printBinary(n) for(int i=sizeof(n)*8-1;i>=0;i--)printf("%d", (n>>i)&1);
#define isBitSet(num, n) ((num) & (1 << (n)))

int main(int argc, char *argv[]) {
    FILE *file_ptr;
    char *file_contents;
    long file_size;
    size_t result;

    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    file_ptr = fopen(argv[1], "rb");

    if (file_ptr == NULL) {
        printf("Error opening file. Please check if the file exists and try again.\n");
        return 1;
    }

    fseek(file_ptr, 0, SEEK_END);
    file_size = ftell(file_ptr);
    rewind(file_ptr);

    file_contents = (char *)malloc(file_size * sizeof(char));

    if (file_contents == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file_ptr);
        return 1;
    }
    
    result = fread(file_contents, 1, file_size, file_ptr);
    if (result != file_size) {
        printf("Error reading file.\n");
        free(file_contents);
        fclose(file_ptr);
        return 1;
    }

    SEIF_Header *header = (SEIF_Header*)file_contents;
	
	if(strncmp((char*)header->magic, "SEIF", 4) != 0) {
		printf("Image is not a vaild SEIF image. ERROR: Image has invalid magic \"%.4s\"!\n", header->magic);
		return 1;
	}

	printf("Magic: \"%.4s\"\n", header->magic);
	printf("Flags: ");
	printBinary(header->flags);
	printf("\n");

	bool alpha = false;
	bool meta = false;
	bool compressed = false;

	if(isBitSet(header->flags, 0))
		alpha = true;
	if(isBitSet(header->flags, 1)) 
		meta = true;	
	if(isBitSet(header->flags, 2))
		compressed = true;
		
	if(compressed) {
		printf("Compressed images arent supported yet!\n");
		return 1;
	}

	printf("Alpha: %s, Meta: %s, Compressed: %s\n", alpha ? "yes" : "no", meta ? "yes" : "no", compressed ? "yes" : "no");

	if(meta)
		printf("Meta Signature: \"%.8s\"\n", header->meta.signature);

	fclose(file_ptr);
    free(file_contents);

    return 0;
}

