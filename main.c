#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;
typedef unsigned char  BYTE;

#pragma pack(1)  

typedef struct _BITMAPFILEHEADER
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
}BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER
{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
}BITMAPINFOHEADER;

typedef struct _RGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
}RGBQUAD;

#pragma pack()  

int Gray_Raw2Bmp(char* pRawName, DWORD nWidth, DWORD nHeight, char* pBmpName)
{
    BITMAPFILEHEADER  file_h;
    BITMAPINFOHEADER  info_h;
    DWORD             dwBmpSize = 0;
    DWORD             dwRawSize = 0;
    DWORD             dwLine = 0;
    long              lCount, i;
    FILE* in, * out;
    char* pData = NULL;
    RGBQUAD           rgbPal[256];

    in = fopen(pRawName, "rb");
    if (in == NULL)
    {
        printf("File Open Error!\n");
        return 0;
    }

    out = fopen(pBmpName, "wb");
 
    file_h.bfType = 0x4D42;
    file_h.bfReserved1 = 0;
    file_h.bfReserved2 = 0;
    file_h.bfOffBits = sizeof(rgbPal) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    info_h.biSize = sizeof(BITMAPINFOHEADER);
    info_h.biWidth = (DWORD)nWidth;
    info_h.biHeight = (DWORD)nHeight;
    info_h.biPlanes = 1;
    info_h.biBitCount = 8;
    info_h.biCompression = 0;
    info_h.biXPelsPerMeter = 0;
    info_h.biYPelsPerMeter = 0;
    info_h.biClrUsed = 0;
    info_h.biClrImportant = 0;

    dwLine = ((((info_h.biWidth * info_h.biBitCount) + 31) & ~31) >> 3);
    dwBmpSize = dwLine * info_h.biHeight;
    info_h.biSizeImage = dwBmpSize;
    file_h.bfSize = dwBmpSize + file_h.bfOffBits + 2;

    dwRawSize = info_h.biWidth * info_h.biHeight;
    pData = (char*)malloc(sizeof(char) * dwRawSize + 16);

    if (pData)
    {
        fread(pData, 1, dwRawSize, in);

        for (i = 0; i < 256; i++)
        {
            rgbPal[i].rgbRed = (BYTE)(i % 256);
            rgbPal[i].rgbGreen = rgbPal[i].rgbRed;
            rgbPal[i].rgbBlue = rgbPal[i].rgbRed;
            rgbPal[i].rgbReserved = 0;
        }

        fwrite((char*)&file_h, 1, sizeof(BITMAPFILEHEADER), out);
        fwrite((char*)&info_h, 1, sizeof(BITMAPINFOHEADER), out);
        fwrite((char*)rgbPal, 1, sizeof(rgbPal), out);
        lCount = dwRawSize;

        for (lCount -= (long)info_h.biWidth; lCount >= 0; lCount -= (long)info_h.biWidth)
        {
            fwrite((pData + lCount), 1, (long)dwLine, out);
        }

        free(pData);
    }

    fclose(in);
    fclose(out);

    return 1;
}

int main(int argc, char** argv)
{
    char szRawName[80];
    char szBmpName[80];
    DWORD  nWidth = 256, nHeight = 256;

    printf("RAW File Name ? ");
    gets(szRawName);
    printf("BMP File Name ? ");
    gets(szBmpName);

    Gray_Raw2Bmp(szRawName, nWidth, nHeight, szBmpName);

    return 0;
}