 /**
 * Copies a BMP piece by piece, just because.
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

		//remembers the factor
		int n = atoi(argv[1]);

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

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
		
		// outfile's dimensions increase n folds
		bi.biWidth *= n;
		bi.biHeight *= n;
		
		// infile's dimensions
		int biWidth = bi.biWidth / n;
		int biHeight = bi.biHeight / n;

		// padding for both files
    int inPadd  = (4 - (biWidth * sizeof(RGBTRIPLE)) % 4) % 4;			
		int outPadd = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
		
    // outfile's updated header elements
    bi.biSizeImage = (bi.biWidth + outPadd) * bi.biHeight;
    bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
		
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
		
		// array to store scanline of both files
		RGBTRIPLE *inLine = malloc(sizeof(biWidth));
		RGBTRIPLE *outLine = malloc(sizeof(bi.biWidth));
		
		if(!inLine || !outLine)
		{
			free(inLine);
			free(outLine);
			return 5;
		}
		// repeat for every scanline of infile
		for(int j = 0; j < abs(biHeight); j++)
		{
			// read every pixel of scanline
			for(int i = 0; i < biWidth; i++)
			{
				fread(&inLine[i], sizeof(RGBTRIPLE), 1, inptr);
			}
			
			// repeat every pixel n times horizontally		
			for(int i = 0; i < biWidth; i++)
			{
				int count = 0;
				
				// store the scaled scanline in the array outLine
				for(int o = i * n; count < n; o++)
				{	
					outLine[o] = inLine[i];
					count++;
				}
				count = 0;
			}
			
			// repeat every scanline n times vertically
			for(int k = 0; k < n; k++)
			{		
				// write scanline into outfile
				for(int o = 0; o < bi.biWidth; o ++)
				{
					fwrite(&outLine[o], sizeof(RGBTRIPLE), 1, outptr);
				}
				
				// introduce padding in outfile
				for(int l = 0; l < outPadd; l++)
					fputc(0x00, outptr);
			}
			
			//skip the padding of infile
			fseek(inptr, inPadd, SEEK_CUR);	
		}
		
		// free the allocated memory
		free(inLine);
		free(outLine);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
