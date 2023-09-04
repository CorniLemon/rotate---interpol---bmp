#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

static const double angle = 1 * M_PI / 4;//? ????????

static const double sinA = sin(angle);
static const double cosA = cos(angle);

static const char sIn[] = "in3.bmp";
static const char sOut[] = "out3_1Pid4.bmp";

uint32_t trash = 0;

typedef unsigned __int16 WORD;//2 ?????
//typedef unsigned int DWORD;//4 ?????
typedef long LONG;//4 ?????
typedef unsigned char BYTE;//1 ????

class FileWithDes {
    FILE* f = NULL;
public:
    FileWithDes(const char A[], const char B[]) {
        f = fopen(A, B);
        if (!f) {
            cout << "???? " << A << " ?? ?????????? ??? ?? ??????? ???????\n";
            throw;
        }
    };

    FILE* getF() {
        return f;
    };

    ~FileWithDes() {
        fclose(f);
        f = NULL;
    }
};

class PIX {
public:
    double rgbtBlue = 0;
    double rgbtGreen = 0;
    double rgbtRed = 0;

    void clc() {
        rgbtBlue = 0;
        rgbtGreen = 0;
        rgbtRed = 0;
    }

    void norm() {
        rgbtBlue = min(255, rgbtBlue);
        rgbtGreen = min(255, rgbtGreen);
        rgbtRed = min(255, rgbtRed);
    }
};

class MATR {
    RGBTRIPLE** matrIn;
    size_t Hin;
    size_t Win;

public:

    MATR(size_t Hin, size_t Win) : Hin(Hin), Win(Win)
    {
        matrIn = new RGBTRIPLE * [Hin];
        for (size_t i = 0; i < Hin; ++i) {
            matrIn[i] = new RGBTRIPLE[Win];
        }
    }

    ~MATR() {
        for (size_t i = 0; i < Hin; ++i) {
            delete[] matrIn[i];
        }
        delete[] matrIn;
    }

    RGBTRIPLE get(size_t i, size_t j) {
        return matrIn[i][j];
    }

    void FillAll(FILE* f, int padding) {
        for (size_t i = 0; i < Hin; ++i) {
            fread(matrIn[i], Win * 3, 1, f);
            fseek(f, padding, SEEK_CUR);
        }
    }

    RGBTRIPLE getAve2Per(double Y1, double X1) {
        PIX Ave;
        Ave.clc();
        size_t x, y;
        x = X1;//?????????? ????
        y = Y1;
        RGBTRIPLE Out;

        Ave.rgbtBlue = matrIn[y][x].rgbtBlue * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtBlue * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtBlue * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtBlue * (X1 - x) * (Y1 - y);
        Ave.rgbtGreen = matrIn[y][x].rgbtGreen * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtGreen * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtGreen * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtGreen * (X1 - x) * (Y1 - y);
        Ave.rgbtRed = matrIn[y][x].rgbtRed * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtRed * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtRed * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtRed * (X1 - x) * (Y1 - y);

        Ave.norm();
        Out.rgbtBlue = Ave.rgbtBlue;
        Out.rgbtGreen = Ave.rgbtGreen;
        Out.rgbtRed = Ave.rgbtRed;

        return Out;
    }

    RGBTRIPLE getAve2(double Y1, double X1) {
        PIX Ave;
        Ave.clc();
        size_t x, y;
        x = X1;//?????????? ????
        y = Y1;
        RGBTRIPLE Out;

        Ave.rgbtBlue = (matrIn[y][x].rgbtBlue + matrIn[y][x + 1].rgbtBlue + matrIn[y + 1][x].rgbtBlue + matrIn[y + 1][x + 1].rgbtBlue) / 4;
        Ave.rgbtGreen = (matrIn[y][x].rgbtGreen + matrIn[y][x + 1].rgbtGreen + matrIn[y + 1][x].rgbtGreen + matrIn[y + 1][x + 1].rgbtGreen) / 4;
        Ave.rgbtRed = (matrIn[y][x].rgbtRed + matrIn[y][x + 1].rgbtRed + matrIn[y + 1][x].rgbtRed + matrIn[y + 1][x + 1].rgbtRed) / 4;

        Ave.norm();
        Out.rgbtBlue = Ave.rgbtBlue;
        Out.rgbtGreen = Ave.rgbtGreen;
        Out.rgbtRed = Ave.rgbtRed;

        return Out;
    }

    RGBTRIPLE** getM() {
        return matrIn;
    };

};

class DoOutFile {
    size_t Hout;
    size_t Wout;
    int paddingOut;

    vector<RGBTRIPLE> lineOut;

    RGBTRIPLE black;

    double dx = 0;
    double dy = 0;

public:

    DoOutFile(size_t Hin, size_t Win, LONG& Wout1, LONG& Hout1, int& paddingOut1){

        double Ax = 0, Ay = 0;
        double Bx, By;
        double Cx, Cy;
        double Dx, Dy;

        Bx = Hin * sinA;
        By = Hin * cosA;

        Cx = Hin * sinA + Win * cosA;
        Cy = Hin * cosA -1*( Win * sinA);

        Dx = Win * cosA;
        Dy = -1 * (Win * sinA);

        dx = min(Ax, Bx);
        dx = min(dx, Cx);
        dx = min(dx, Dx);
        dx = abs(dx);

        dy = min(Ay, By);
        dy = min(dy, Cy);
        dy = min(dy, Dy);
        dy = abs(dy);

        Wout = max(Ax, Bx);
        Wout = max(Wout, Cx);
        Wout = max(Wout, Dx);
        Wout += dx;
        Wout1 = Wout;

        Hout = max(Ay, By);
        Hout = max(Hout, Cy);
        Hout = max(Hout, Dy);
        Hout += dy;
        Hout1 = Hout;

        paddingOut = (4 - (Wout * 3) % 4) % 4;
        paddingOut1 = paddingOut;

        lineOut.resize(Wout);

        black.rgbtBlue = 0;
        black.rgbtGreen = 0;
        black.rgbtRed = 0;
    }

    void OutFile(size_t Hin, size_t Win, FILE* f, RGBTRIPLE** matr,MATR& matrOrigin){

        double Y1;
        double X1;

        for (int y = 0; y < Hout; ++y) {
            for (int x = 0; x < Wout; ++x) {

                Y1 = (x - dx) * sinA + (y - dy) * cosA + 0.5;
                X1 = (x - dx) * cosA - (y - dy) * sinA + 0.5;

                if (X1 >= 0 && X1 < Win-1 && Y1 >= 0 && Y1 < Hin-1) {//---------------------------------
                    //lineOut[x] = matrOrigin.getAve2Per(Y1, X1);
                    lineOut[x] = matrOrigin.getAve2(Y1, X1);
                    //lineOut[x] = matr[Y1][X1];
                }
                else {
                    lineOut[x] = black;
                }
            }
            fwrite(lineOut.data(), Wout * 3, 1, f);//?????? ? bmp
            fwrite(&trash, 1, paddingOut, f);
        }
    }
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");//??????????? ???????? ?????6

    //int64_t t0 = GetTickCount();

    BITMAPFILEHEADER bfhIn, bfhOut;
    BITMAPINFOHEADER bihIn, bihOut;
    int paddingIn, paddingOut;

    FileWithDes f1(argc > 2 ? argv[1] : sIn, "rb");
    FileWithDes f2(argc > 2 ? argv[2] : sOut, "wb");

    cout << "???????????? " << sIn << "\n????? ? " << sOut;

    //??????? ????? ???
    fread(&bfhIn, sizeof(bfhIn), 1, f1.getF());
    fread(&bihIn, sizeof(bihIn), 1, f1.getF());

    paddingIn = (4 - (bihIn.biWidth * 3) % 4) % 4;

    MATR matr(bihIn.biHeight, bihIn.biWidth);
    matr.FillAll(f1.getF(), paddingIn);

    DoOutFile DoFile2(bihIn.biHeight, bihIn.biWidth, bihOut.biWidth, bihOut.biHeight, paddingOut);


    bihOut.biSize = sizeof(bihOut);
    //bihOut.biWidth = abs(bihIn.biWidth * cosA) + abs(bihIn.biHeight * sinA) + 2;
    //bihOut.biHeight = abs(bihIn.biWidth * sinA) + abs(bihIn.biHeight * cosA) + 2;
    bihOut.biPlanes = 1;
    bihOut.biBitCount = 24;
    bihOut.biCompression = 0;
    bihOut.biSizeImage = bihOut.biWidth * bihOut.biHeight * 3;
    bihOut.biXPelsPerMeter = 2835;
    bihOut.biYPelsPerMeter = 2835;
    bihOut.biClrUsed = 0;
    bihOut.biClrImportant = 0;

    bfhOut.bfType = 19778;
    bfhOut.bfReserved1 = 0;
    bfhOut.bfReserved2 = 0;
    bfhOut.bfOffBits = sizeof(bfhOut) + sizeof(bihOut);
    bfhOut.bfSize = sizeof(bfhOut) + sizeof(bihOut) + bihOut.biSizeImage;

    fwrite(&bfhOut, sizeof(bfhOut), 1, f2.getF());
    fwrite(&bihOut, sizeof(bihOut), 1, f2.getF());

    cout << "\n\n?????? ????????? ???????????: " << bihIn.biHeight << "*" << bihIn.biWidth << endl;
    cout << "\n\n?????? ????????? ???????????: " << bihOut.biHeight << "*" << bihOut.biWidth << endl;

    
    DoFile2.OutFile(bihIn.biHeight, bihIn.biWidth, f2.getF(), matr.getM(), matr);

    return 0;
}
