	// allocate enough memory to store a pixel
	RGBTRIPLE* pixel = malloc(sizeof(RGBTRIPLE));	

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
		// skip reading the padding of infile
		fseek(inptr, inPadd, SEEK_CUR);	
	}



bmp.h:


#include <stdint.h>
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

typedef struct 
{ 
    WORD bfType; 
    DWORD bfSize; 
    WORD bfReserved1; 
    WORD bfReserved2; 
    DWORD bfOffBits; 
} __attribute__((__packed__)) 
BITMAPFILEHEADER; 

typedef struct
{
    DWORD biSize; 
    LONG biWidth; 
    LONG biHeight; 
    WORD biPlanes; 
    WORD biBitCount; 
    DWORD biCompression; 
    DWORD biSizeImage; 
    LONG biXPelsPerMeter; 
    LONG biYPelsPerMeter; 
    DWORD biClrUsed; 
    DWORD biClrImportant; 
} __attribute__((__packed__))
BITMAPINFOHEADER; 

typedef struct
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;
