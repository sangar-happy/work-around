/**
 * resizes an image by factor of f
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
	// ensure proper usage
	if (argc != 4)
	{
		fprintf(stderr, "Usage: ./resize n  infile outfile \n");
		return 1;
 	}

	//remember the factor
	float f;
	sscanf(argv[1], "%f ", &f);
	
	
 	// remember filenames
 	char *infile = argv[2];
 	char *outfile = argv[3];		

 	// open input file 
 	FILE *inptr = fopen(infile, "r");
 	if (inptr == NULL)
 	{
   	fprintf(stderr, "Could not open %s.\n", infile); 
   	return 2;
 	}

 	// open output file
 	FILE *outptr = fopen(outfile, "w");
 	if (outptr == NULL)
 	{
		fclose(inptr);
   		fprintf(stderr, "Could not create %s.\n", outfile);
 		return 3;
 	}

 	// read infile's BITMAPFILEHEADER and BITMAPINFOHEADER
 	BITMAPFILEHEADER outBF;
 	fread(&outBF, sizeof(BITMAPFILEHEADER), 1, inptr);
 	BITMAPINFOHEADER outBI;
 	fread(&outBI, sizeof(BITMAPINFOHEADER), 1, inptr);

 	// ensure infile is (likely) a 24-bit uncompressed BMP 4.0
 	if (outBF.bfType != 0x4d42 || outBF.bfOffBits != 54 || outBI.biSize != 40 || outBI.biBitCount != 24 || outBI.biCompression != 0)
 	{
   	fclose(outptr);
   	fclose(inptr);
   	fprintf(stderr, "Unsupported file format.\n");
   	return 4;
 	}

	// infile's dimensions
	int biWidth = outBI.biWidth;
	int biHeight = outBI.biHeight;
	
	// outfile's dimensions increase f folds
	outBI.biWidth *= f;
	outBI.biHeight *= f;

	// padding for both files
	int inPadd  = (4 - (biWidth * sizeof(RGBTRIPLE)) % 4) % 4;			
	int outPadd = (4 - (outBI.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

	// array to store scanline of outfile
	RGBTRIPLE* outLine = malloc(outBI.biWidth * sizeof(RGBTRIPLE));		

	// if memory cannot be allocated in heap
	if(!outLine)
	{
		free(outLine);
		fclose(outptr);
		fclose(inptr);
		fprintf(stderr, "Could not allocate memory in heap.\n");
		return 5;
	}
		
 	// outfile's updated header elements
 	outBI.biSizeImage = ((outBI.biWidth * sizeof(RGBTRIPLE) + outPadd) * abs(outBI.biHeight));
 	outBF.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + outBI.biSizeImage;
		
 	// write outfile's BITMAPFILEHEADER and BITMAPINFOHEADER
	fwrite(&outBF, sizeof(BITMAPFILEHEADER), 1, outptr);
	fwrite(&outBI, sizeof(BITMAPINFOHEADER), 1, outptr);
	
	// n hold the number of times every pixel and scanline has to repeat 
	// priv holds the number (starting from first) of pixels and scanlines which have to be repeated n+1 times
	int n = outBI.biWidth / biWidth, privScanLine = 0;
	const int priv = outBI.biWidth % biWidth;
	
	// repeat for every scanline of infile
	for(int j = 0; j < abs(biHeight); j++)
	{
		int o = 0;
		int privPixel = 0;
		
		// repeat for every pixel in scanline
		for (int i = 0; i < biWidth; i += 1)
		{
			
			// read every pixel n times
			for (int k = 0; k < n; k += 1)
			{
				fread(&outLine[o], sizeof(RGBTRIPLE), 1, inptr);
				o++;
				
				// take the cursor one pixel back if loop is to run again
				if(k+1 < n)
					fseek(inptr, -( (long int) (sizeof(RGBTRIPLE)) ), SEEK_CUR);
			}
			
			
			if(privPixel < priv)
			{
				
				// if this pixel has the privilege, take the cursor back and read it once more
				if(n != 0)
					fseek(inptr, -( (long int) (sizeof(RGBTRIPLE)) ), SEEK_CUR);
				fread(&outLine[o], sizeof(RGBTRIPLE), 1, inptr);
				o++;
				privPixel++;
				
				// if the picture is being scaled down, read the pixel and
				// then fseek forward, skipping pixels according to fraction
				if(n == 0)
					fseek(inptr, sizeof(RGBTRIPLE) * ((biWidth / priv) - 1), SEEK_CUR);
			}		
			
		}
		
		// write every scanline n times
		for (int k = 0; k < n; k += 1)
		{
			for (int i = 0; i < outBI.biWidth; i += 1)
			{
				fwrite(&outLine[i], sizeof(RGBTRIPLE), 1, outptr);
			}
			
			// add outfile's padding in end		
			for(int l = 0; l < outPadd; l++)
				fputc(0x00, outptr);
		}
		
		// if the scanline has privilege, write it once more
		if(privScanLine < priv)
		{
			for (int i = 0; i < outBI.biWidth; i += 1)
				fwrite(&outLine[i], sizeof(RGBTRIPLE), 1, outptr);
		
			for(int l = 0; l < outPadd; l++)
				fputc(0x00, outptr);
		}
		
		//skip reading the infile's padding
		fseek(inptr, inPadd, SEEK_CUR);
		
		// if picture is being scaled down, skip reading some scanlines
		// of infile according to the fraction
		if(n == 0)
			fseek(inptr, ( (biWidth * sizeof(RGBTRIPLE) + inPadd) * ((biWidth / privPixel) - 1) ), SEEK_CUR);
		privScanLine++;
	}

	// free the allocated memory
	free(outLine);
    
	// close infile and outfile
	fclose(inptr);
	fclose(outptr);

	// success
	return 0;
}
