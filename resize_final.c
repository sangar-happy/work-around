/**
 * resizes an image by factor of f
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int divisor(int ,int );

int main(int argc, char** argv)
{
	// ensure proper usage
	if (argc != 4)
	{
		fprintf(stderr, "Usage: ./resize f  infile outfile \n");
		return 1;
 	}

	//remember the factor
	float f;
	sscanf(argv[1], "%f ", &f);
	
	// n holds the number of times every pixel and scanline has to repeat 	
	int n = f;
	
	int nume = lroundf( ( f - (float)n ) * 100 );
	int denom = 0;
	
	if(nume != 0)
	{
		int div = divisor(nume, 100);
		nume = nume / div;
		denom = 100 / div; 
	}
	


 	// remember filenames
 	char* infile = argv[2];
 	char* outfile = argv[3];		

 	// open input file 
 	FILE* inptr = fopen(infile, "r");
 	if (inptr == NULL)
 	{
   	fprintf(stderr, "Could not open %s.\n", infile); 
   	return 2;
 	}

 	// open output file
 	FILE* outptr = fopen(outfile, "w");
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

	// allocate enough memory to store a pixel
	RGBTRIPLE* pixel = malloc(sizeof(RGBTRIPLE));		

	// if memory cannot be allocated in heap
	if(!pixel)
	{
		free(pixel);
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


	int count_scan = 0,temp_scan_counter = 0, x, count_pix, temp_pix_counter;
	
	// repeat for every scanline of infile
	for(int j = 0; j < abs(biHeight); j++)
	{
		
		count_scan++;
		x = n;
		
		if(count_scan <= nume && (temp_scan_counter + 1 + n * abs(biHeight)) <= abs(outBI.biHeight))
			{
				x++;
				temp_scan_counter++;
			}
		else 	if(count_scan == denom)
					count_scan = 0;
		
		if (x == 0)
			fseek(inptr, biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
		
		// write every scanline x times
		for(int k = 0; k < x; k++)
		{	
			
			count_pix = 0;
			temp_pix_counter = 0;
			// read scanline pixel-by-pixel			
			for(int i = 0; i < biWidth; i++)
			{
				
				count_pix++;
				
				fread(pixel, sizeof(RGBTRIPLE), 1, inptr);			

				// write every pixel n times
				for(int l = 0; l < n; l++)
				{
					fwrite(pixel, sizeof(RGBTRIPLE), 1, outptr);
					temp_pix_counter++;
				}
				
				if(count_pix <= nume && temp_pix_counter < outBI.biWidth)
					{
						fwrite(pixel, sizeof(RGBTRIPLE), 1, outptr);
						temp_pix_counter++;
					}
				
				else 	if(count_pix == denom)
								count_pix = 0;
				
			}
			
			// introduce padding after scanline in outfile
			for(int l = 0; l < outPadd; l++)
				fputc(0x00, outptr);
 
			// return the cursor to starting of infile's scanline
			if(k < x - 1)
				fseek(inptr, - ((long int) (biWidth * sizeof(RGBTRIPLE))), SEEK_CUR);
							
		}
			
		fseek(inptr, inPadd, SEEK_CUR);	
	}

	// free the allocated memory
	free(pixel);
    
	// close infile and outfile
	fclose(inptr);
	fclose(outptr);

	// success
	return 0;
}

//source: https://www.programiz.com/c-programming/examples/hcf-gcd
int divisor(int n1, int n2)
{
	while (n1 != n2)
	{
		if (n1 > n2)
			n1 -= n2;
		
		else
			n2 -= n1;
	}

	return(n1);
}

