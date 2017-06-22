	// repeat for every scanline of infile
	for(int j = 0; j < abs(biHeight); j++)
	{
		
		// write every scanline n times
		for(int k = 0; k < n; k++)
		{	
			
			// read scanline pixel-by-pixel			
			for(int i = 0; i < biWidth; i++)
			{
				
				fread(&pixel, sizeof(RGBTRIPLE), 1, inptr);			
				
				// write every pixel n times
				for(int l = 0; l < n; l++)
				{
					fwrite(&pixel, sizeof(RGBTRIPLE), 1, outptr);
				}
			}
			
			// introduce padding after scanline in outfile
			for(int l = 0; l < outPadd; l++)
				fputc(0x00, outptr);
				
			// return the cursor to starting of infile's scanline
			if(k < n-1)
				fseek(inptr, - ((long int) (biWidth * sizeof(RGBTRIPLE))), SEEK_CUR);	
		}	
		fseek(inptr, inPadd, SEEK_CUR);	
	}
