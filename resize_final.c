/**
 * resizes an image by factor of f
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int divisor(int ,int );
void store_scanLine(FILE* , RGBTRIPLE* , int , int );
void write_scanLine(FILE* , RGBTRIPLE* , int , int , int , int , int );


int main(int argc, char** argv)
{
	// ensure proper usage
	if (argc != 4)
	{
		fprintf(stderr, "Usage: ./resize f  infile outfile \n");
		return 1;
 	}

	// remember the factor
	float f;
	sscanf(argv[1], "%f ", &f);
	
	// repeat every pixel and scanline n times
	int n = f;
	
	// every 'nume' out of 'denom' pixels and scanlines are repeated n+1 times
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

	// allocate memory to store every pixel of inLine n times
	RGBTRIPLE* scanLine = malloc(sizeof(RGBTRIPLE) * biWidth);		

	// if memory cannot be allocated in heap
	if(!scanLine)
	{
		free(scanLine);
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
	
	int counter = 0, ex = 0;
	
	// repeat for every scanline of infile
	for(int j = 0; j < abs(biHeight); j++)
	{
		counter++;

		store_scanLine(inptr, scanLine, biWidth, inPadd);
		
		for(int i = 0; i < n; i++)
			write_scanLine(outptr, scanLine, n, biWidth, nume, denom, outPadd);		

		if(counter <= nume && ( ex + 1 + n * abs(biHeight)) <= abs(outBI.biHeight))
		{	
			write_scanLine(outptr, scanLine, n, biWidth, nume, denom, outPadd);		
			ex++;
		}
			
		else 	if(counter == denom)
						counter = 0;		
		
	}
	// free the allocated memory
	free(scanLine);
    
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

// store the inLine in array scanLine
void store_scanLine(FILE* inptr, RGBTRIPLE* scanLine, int biWidth, int inPadd)
{
	for(int i = 0; i < biWidth; i++)
		fread(&scanLine[i], sizeof(RGBTRIPLE), 1, inptr);

	fseek(inptr, inPadd, SEEK_CUR);
}


void write_scanLine(FILE* outptr, RGBTRIPLE* scanLine, int n, int biWidth, int nume, int denom, int outPadd)
{
	int counter = 0;
	
	for(int i = 0; i < biWidth; i++)
	{
		for(int j = 0; j < n; j++)
		{
			
			fwrite(&scanLine[i], sizeof(RGBTRIPLE), 1, outptr);
		}
		counter++;
		
		if(counter <= nume && (i+1) < biWidth)
			fwrite(&scanLine[i], sizeof(RGBTRIPLE), 1, outptr);
			
		else 	if(counter == denom)
						counter = 0;
		
	}
	
	// introduce padding after scanline in outfile
	for(int l = 0; l < outPadd; l++)
		fputc(0x00, outptr);
}
