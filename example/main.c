#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../seif.h"

#define printBinary(n)                           \
	for (int i = sizeof(n) * 8 - 1; i >= 0; i--) \
		printf("%d", (n >> i) & 1);
#define isBitSet(num, n) ((num) & (1 << (n)))

void put_pixel(int r, int g, int b)
{
// ANSI color escape codes
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BG(r, g, b) "\x1b[48;2;" #r ";" #g ";" #b "m"

	// Set background color
	printf(ANSI_COLOR_BG(% d, % d, % d), r, g, b);

	// Draw dot (represented by a single character)
	printf(".");

	// Reset colors
	printf(ANSI_COLOR_RESET);
}

int main(int argc, char *argv[])
{
	FILE *file_ptr;
	char *file_contents;
	long file_size;
	size_t result;

	if (argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	file_ptr = fopen(argv[1], "rb");

	if (file_ptr == NULL)
	{
		printf(
			"Error opening file. Please check if the file exists and try again.\n");
		return 1;
	}

	fseek(file_ptr, 0, SEEK_END);
	file_size = ftell(file_ptr);
	rewind(file_ptr);

	file_contents = (char *)malloc(file_size * sizeof(char));

	if (file_contents == NULL)
	{
		printf("Memory allocation failed.\n");
		fclose(file_ptr);
		return 1;
	}

	result = fread(file_contents, 1, file_size, file_ptr);
	if (result != file_size)
	{
		printf("Error reading file.\n");
		free(file_contents);
		fclose(file_ptr);
		return 1;
	}

	SEIF_Header *header = (SEIF_Header *)file_contents;

	if (strncmp((char *)header->magic, "SEIF", 4) != 0)
	{
		printf("Image is not a vaild SEIF image. ERROR: Image has invalid magic "
			   "\"%.4s\"!\n",
			   header->magic);
		return 1;
	}

	printf("- Magic: \"%.4s\"\n", header->magic);
	printf("- Flags: ");
	printBinary(header->flags);
	printf("\n");

	bool alpha = false;
	bool compressed = false;

	if (isBitSet(header->flags, 0))
		alpha = true;
	if (isBitSet(header->flags, 1))
		compressed = true;

	if (compressed)
	{
		printf("Compressed images arent supported yet!\n");
		return 1;
	}

	printf("- Alpha: %s, Compressed: %s\n", alpha ? "yes" : "no", compressed ? "yes" : "no");

	printf("- Chunk Count: %d\n", header->chunk_count);

	if (header->chunk_count < 1)
	{
		printf("Invalid chunk count \"%d\"!\n", header->chunk_count);
		return 1;
	}

	printf("- Signature: \"%s\"\n", header->meta.signature);
	printf("- Width: %d\n", header->meta.width);
	printf("- Height: %d\n", header->meta.height);

	for (int c = 0; c < header->chunk_count; c++)
	{
		printf("-------- Chunk %d --------\n", c + 1);
		SEIF_ChunkHeader *chunk_header = (SEIF_ChunkHeader *)(file_contents + sizeof(SEIF_Header) + (c * sizeof(SEIF_ChunkHeader)));

		if (chunk_header == NULL)
		{
			printf("Didnt find the chunk header for chunk %d!\n", c);
			return 1;
		}

		int encoding_size;

		if (header->encoding == SEIF_ENCODING_RGBA)
		{
			encoding_size = sizeof(uint32_t);
		}
		else if (header->encoding == SEIF_ENCODING_RGB)
		{
			encoding_size = 3; // 24bits
		}
		else if (header->encoding == SEIF_ENCODING_ARGB)
		{
			encoding_size = sizeof(uint32_t);
		}

		int chunk_size_full = header->chunk_size * encoding_size;
		size_t offset = sizeof(SEIF_Header) + sizeof(SEIF_ChunkHeader) * header->chunk_count + (chunk_size_full * c);

		uint8_t *chunk = (uint8_t *)(file_contents + offset);
		for (int i = 0; i < chunk_size_full; i += encoding_size)
		{
			int r, g, b, a = 0;
			if (header->encoding == SEIF_ENCODING_RGBA)
			{
				r = chunk[i + 0];
				g = chunk[i + 1];
				b = chunk[i + 2];
				a = chunk[i + 3];
			}
			else if (header->encoding == SEIF_ENCODING_RGB)
			{
				r = chunk[i + 0];
				g = chunk[i + 1];
				b = chunk[i + 2];
			}
			else if (header->encoding == SEIF_ENCODING_ARGB)
			{
				a = chunk[i + 0];
				r = chunk[i + 1];
				g = chunk[i + 2];
				b = chunk[i + 3];
			}

			printf("%d: R = %d, G = %d, B = %d, A = %d\n", i / encoding_size, r, g, b, a);
		}
	}

	fclose(file_ptr);
	free(file_contents);

	return 0;
}
