typedef uint8_t  BYTE;
BYTE* buffer = malloc( 512 * sizeof(BYTE) );

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
                                // ensures that the image starts from ffd8ff
/*This is not working for the first image*/				
				for(int j = k; j < 512; j++)
					fwrite(&buffer[j], sizeof(BYTE), 1, image);
				break;
			}
		}
	}
