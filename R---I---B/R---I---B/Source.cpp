#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

static const double angle = -1 * M_PI / 12;//в радианах

static const double sinA = sin(angle);
static const double cosA = cos(angle);

static const char sIn[] = "in3.bmp";
static const char sOut[] = "out3_-1Pid12.bmp";

uint32_t trash = 0;

typedef unsigned __int16 WORD;//2 байта
//typedef unsigned int DWORD;//4 байта
typedef long LONG;//4 байта
typedef unsigned char BYTE;//1 байт

class FileWithDes {
    FILE* f = NULL;
public:
    FileWithDes(const char A[], const char B[]) {
        f = fopen(A, B);
        if (!f) {
            cout << "файл " << A << " не существует или не удалось создать\n";
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
        x = X1;//округление вниз
        y = Y1;
        RGBTRIPLE Out;

        Ave.rgbtBlue = matrIn[y][x].rgbtBlue * (X1 - x) * (Y1 - y) + matrIn[y + 1][x].rgbtBlue * (X1 - x) * (y + 1 - Y1) + matrIn[y][x + 1].rgbtBlue * (x + 1 - X1) * (Y1 - y) + matrIn[y + 1][x + 1].rgbtBlue * (x + 1 - X1) * (y + 1 - Y1);
        Ave.rgbtGreen = matrIn[y][x].rgbtGreen * (X1 - x) * (Y1 - y) + matrIn[y + 1][x].rgbtGreen * (X1 - x) * (y + 1 - Y1) + matrIn[y][x + 1].rgbtGreen * (x + 1 - X1) * (Y1 - y) + matrIn[y + 1][x + 1].rgbtGreen * (x + 1 - X1) * (y + 1 - Y1);
        Ave.rgbtRed = matrIn[y][x].rgbtRed * (X1 - x) * (Y1 - y) + matrIn[y + 1][x].rgbtRed * (X1 - x) * (y + 1 - Y1) + matrIn[y][x + 1].rgbtRed * (x + 1 - X1) * (Y1 - y) + matrIn[y + 1][x + 1].rgbtRed * (x + 1 - X1) * (y + 1 - Y1);

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
        x = X1;//округление вниз
        y = Y1;
        RGBTRIPLE Out;

        if (X1 - x == 0 && Y1 - y == 0) {
            Out = matrIn[y][x];
        }
        else
            if (X1 - x == 0) {
                Ave.rgbtBlue = (matrIn[y][x].rgbtBlue + matrIn[y + 1][x].rgbtBlue) / 2;
                Ave.rgbtGreen = (matrIn[y][x].rgbtGreen + matrIn[y + 1][x].rgbtGreen) / 2;
                Ave.rgbtRed = (matrIn[y][x].rgbtRed + matrIn[y + 1][x].rgbtRed) / 2;
            }
            else
                if (Y1 - y == 0) {
                    Ave.rgbtBlue = (matrIn[y][x].rgbtBlue + matrIn[y][x+1].rgbtBlue) / 2;
                    Ave.rgbtGreen = (matrIn[y][x].rgbtGreen + matrIn[y][x+1].rgbtGreen) / 2;
                    Ave.rgbtRed = (matrIn[y][x].rgbtRed + matrIn[y][x+1].rgbtRed) / 2;
                }
                else {
                    Ave.rgbtBlue = (matrIn[y][x].rgbtBlue + matrIn[y][x + 1].rgbtBlue + matrIn[y + 1][x].rgbtBlue + matrIn[y + 1][x + 1].rgbtBlue) / 4;
                    Ave.rgbtGreen = (matrIn[y][x].rgbtGreen + matrIn[y][x + 1].rgbtGreen + matrIn[y + 1][x].rgbtGreen + matrIn[y + 1][x + 1].rgbtGreen) / 4;
                    Ave.rgbtRed = (matrIn[y][x].rgbtRed + matrIn[y][x + 1].rgbtRed + matrIn[y + 1][x].rgbtRed + matrIn[y + 1][x + 1].rgbtRed) / 4;
                }
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

public:

    DoOutFile(size_t Hout, size_t Wout, int paddingOut) : Hout(Hout), Wout(Wout), paddingOut(paddingOut) {
        lineOut.resize(Wout);

        black.rgbtBlue = 0;
        black.rgbtGreen = 0;
        black.rgbtRed = 0;
    }

    void OutFile(size_t Hin, size_t Win, FILE* f, RGBTRIPLE** matr,MATR& matrOrigin) {
        int k1 = floor(angle / (M_PI / 2));//целая часть
        int k = k1 % 4;
        if (k < 0)
            k = 4 + k;
        double dx = 0;
        double dy = 0;
        int kx = 1;
        int ky = 1;
        switch (k)
        {
        case 3:
            dx = Hin * abs(sinA);
            dy = 0;
            break;
        case 2:
            dx = Hin * abs(sinA) + Win * (-cosA);
            dy = Hin * (-cosA);
            break;
        case 1:
            dx = Win * (-cosA);
            dy = Hin * (-cosA) + Win * abs(sinA);
            break;
        case 0:
            dx = 0;
            dy = Win * abs(sinA);
            break;
        default:
            cout << "что-то не то...\n\n";
            break;
        }

        /*++dx;
        ++dy;*/

        double Y1;
        double X1;

        for (int y = 0; y < Hout; ++y) {
            for (int x = 0; x < Wout; ++x) {

                Y1 = (x - dx) * sinA + (y - dy) * cosA + 0.5;
                X1 = (x - dx) * cosA - (y - dy) * sinA + 0.5;

                if (X1 >= 0 && X1 < Win - 1 && Y1 >= 0 && Y1 < Hin - 1 || X1 - Win == 0 || Y1 - Hin == 0) {//---------------------------------
                    lineOut[x] = matrOrigin.getAve2Per(Y1, X1);
                    //lineOut[x] = matrOrigin.getAve2(Y1, X1);
                    //lineOut[x] = matr[Y1][X1];
                }
                else {
                    lineOut[x] = black;
                }
            }
            fwrite(lineOut.data(), Wout * 3, 1, f);//запись в bmp
            fwrite(&trash, 1, paddingOut, f);
        }
    }


};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");//подключение русского языка6

    //int64_t t0 = GetTickCount();

    BITMAPFILEHEADER bfhIn, bfhOut;
    BITMAPINFOHEADER bihIn, bihOut;
    int paddingIn, paddingOut;

    FileWithDes f1(argc > 2 ? argv[1] : sIn, "rb");
    FileWithDes f2(argc > 2 ? argv[2] : sOut, "wb");

    cout << "обрабатываем " << sIn << "\nпишем в " << sOut;

    //считали шапку бмп
    fread(&bfhIn, sizeof(bfhIn), 1, f1.getF());
    fread(&bihIn, sizeof(bihIn), 1, f1.getF());

    bihOut.biSize = sizeof(bihOut);
    bihOut.biWidth = abs(bihIn.biWidth * cosA) + abs(bihIn.biHeight * sinA) + 2;
    bihOut.biHeight = abs(bihIn.biWidth * sinA) + abs(bihIn.biHeight * cosA) + 2;
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

    paddingIn = (4 - (bihIn.biWidth * 3) % 4) % 4;
    paddingOut = (4 - (bihOut.biWidth * 3) % 4) % 4;

    cout << "\n\nразмер исходного изображения: " << bihIn.biHeight << "*" << bihIn.biWidth << endl;
    cout << "\n\nразмер итогового изображения: " << bihOut.biHeight << "*" << bihOut.biWidth << endl;

    MATR matr(bihIn.biHeight, bihIn.biWidth);
    matr.FillAll(f1.getF(), paddingIn);

    DoOutFile DoFile2(bihOut.biHeight, bihOut.biWidth, paddingOut);
    DoFile2.OutFile(bihIn.biHeight, bihIn.biWidth, f2.getF(), matr.getM(), matr);

    return 0;
}