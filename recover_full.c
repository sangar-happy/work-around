/* recovers jpeg images */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

typedef uint8_t  BYTE;

/*		Objectives:
*		 1. open card.raw
*		 2. read 512 bytes from card.raw
*		 3. if consecutive bytes being read are:
*		 	  0xff, 0xd8, 0xff, (b4 & 0xf0) == 0xe0
*		 		open a new file while closing the previous one if any
*		 4. write 512 bytes that were read and repeat doing so till condition in step 3 occurs or if we reach EOF
*		 5. exit
*/

int main(int argc, char** argv)
{
	
	// ensure proper usage
	if(argc != 2)
	{
		fprintf(stderr, "Usage: ./recover image\n");
		return 1;
	}
	
	// pointer to card.raw
	FILE* card = fopen(argv[1], "r");
	if(card == NULL)
	{
		fclose(card);
		fprintf(stderr, "Could not open %s\n", argv[1]);
		return 2;
	}
	
	// buffer to hold memory block of size 512 bytes
	BYTE* buffer = malloc( 512 * sizeof(BYTE) );
	if(buffer == NULL)
	{
		fclose(card);
		free(buffer);
		fprintf(stderr, "Could not allocate memory in heap\n");
		return 3;
	}

	char name[7];
	int i = 0, check = 0;
	FILE* image;
	
	while( (feof(card)) == 0 )
	{
		fread(buffer, sizeof(BYTE), 512, card);
		
		for(int k = 0; k < 512; k++)
		{
			if(buffer[k] == 0xff &&
	  			buffer[k + 1] == 0xd8 &&
	  			buffer[k + 2] == 0xff &&
	  			(buffer[k + 3] & 0xf0) == 0xe0)
			{
				// if outfile already opened, close it
				if(check == 1)
				{	
					fclose(image);
					check = 0;
				}
				
				// open new outfile
				sprintf(name, "%03d.jpg", i);
				i++;
				image = fopen(name, "w");
				if(image == NULL)
					{
						fclose(card);
						free(buffer);
						fclose(image);
						fprintf(stderr, "Could not create image\n");
						return 4;
					}
				check = 1;
			}
				
			if(check == 1)
			{	
				for(int j = k; j < 512; j++)
					fwrite(&buffer[j], sizeof(BYTE), 1, image);
				break;
			}
		}
	}
	
	fclose(card);
	free(buffer);
	if(check == 1)
		fclose(image);
	return 0;
}
