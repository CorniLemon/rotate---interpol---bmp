#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

static const double angle = 1 * M_PI / 4;//в радианах

static const double sinA = sin(angle);
static const double cosA = cos(angle);

static const char sIn[] = "in3.bmp";
static const char sOut[] = "out3_1Pid4WIKI.bmp";

uint32_t trash = 0;

const int extend = 0;

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

        rgbtBlue = max(0, rgbtBlue);
        rgbtGreen = max(0, rgbtGreen);
        rgbtRed = max(0, rgbtRed);
    }
};

class MATR {
    RGBTRIPLE** matrIn;
    size_t Hin;
    size_t Win;
    size_t del;

public:

    MATR(size_t Hin, size_t Win) : Hin(Hin), Win(Win)
    {
        del = Hin + 2 * extend;
        matrIn = new RGBTRIPLE * [del];
        for (size_t i = 0; i < del; ++i) {
            matrIn[i] = new RGBTRIPLE[Win + 2 * extend];
        }
    }

    ~MATR() {
        for (size_t i = 0; i < del; ++i) {
            delete[] matrIn[i];
        }
        delete[] matrIn;
    }

    RGBTRIPLE get(size_t i, size_t j) {
        return matrIn[i][j];
    }

    void FillAll(FILE* f, int padding) {

        for (size_t i = extend; i < Hin + extend; ++i) {
            fread(matrIn[i] + extend, Win * 3, 1, f);
            fseek(f, padding, SEEK_CUR);
            for (size_t j = 0; j < extend; ++j) {//расширение по бокам
                matrIn[i][j] = matrIn[i][2 * extend - 1 - j];
                matrIn[i][Win + 2 * extend - 1 - j] = matrIn[i][Win + j];
            }

        }

        for (size_t i = 0; i < extend; ++i) {//расширение вверх и вниз
            memcpy(matrIn[i], matrIn[2 * extend - 1 - i], (Win + 2 * extend) * sizeof(RGBTRIPLE));
            memcpy(matrIn[Hin + 2 * extend - 1 - i], matrIn[Hin + i], (Win + 2 * extend) * sizeof(RGBTRIPLE));
        }
    }

    RGBTRIPLE getBilineWiki(double Y1, double X1) {
        PIX Ave;
        Ave.clc();
        RGBTRIPLE Out;

        size_t X0, Y0;
        X0 = X1;//округление вниз//целая часть
        Y0 = Y1;

        double x = X1 - X0;
        double y = Y1 - Y0;
        
        int b[4][4] = {
            {
                1 / 4 * (x - 1) * (x - 2) * (x + 1) * (y - 1) * (y - 2) * (y + 1),
                -1 / 4 * x * (x + 1) * (x - 2) * (y - 1) * (y - 2) * (y + 1),
            },
            {
                1 / 4 * (x - 1) * (x - 2) * (x + 1) * (y - 1) * (y - 2) * (y + 1),
                -1 / 4 * x * (x + 1) * (x - 2) * (y - 1) * (y - 2) * (y + 1),
            }
        };
        b[0] = 1 / 4 * (x - 1) * (x - 2) * (x + 1) * (y - 1) * (y - 2) * (y + 1);
        b[1] = -1 / 4 * x * (x + 1) * (x - 2) * (y - 1) * (y - 2) * (y + 1);
        b[2] = -1 / 4 * y * (x - 1) * (x - 2) * (x + 1) * (y + 1) * (y - 2);
        b[3] = 1 / 4 * x * y * (x + 1) * (x - 2) * (y + 1) * (y - 2);
        b[4] = -1 / 12 * x * (x - 1) * (x - 2) * (y - 1) * (y - 2) * (y + 1);
        b[5] = -1 / 12 * y * (x - 1) * (x - 2) * (x + 1) * (y - 1) * (y - 2);
        b[6] = 1 / 12 * x * y * (x - 1) * (x - 2) * (y + 1) * (y - 2);
        b[7] = 1 / 12 * x * y * (x + 1) * (x - 2) * (y - 1) * (y - 2);
        b[8] = 1 / 12 * x * (x - 1) * (x + 1) * (y - 1) * (y - 2) * (y + 1);
        b[9] = 1 / 12 * y * (x - 1) * (x - 2) * (x + 1) * (y - 1) * (y + 1);
        b[10] = 1 / 36 * x * y * (x - 1) * (x - 2) * (y - 1) * (y - 2);
        b[11] = -1 / 12 * x * y * (x - 1) * (x + 1) * (y + 1) * (y - 2);
        b[12] = -1 / 12 * x * y * (x + 1) * (x - 2) * (y - 1) * (y + 1);
        b[13] = -1 / 36 * x * y * (x - 1) * (x + 1) * (y - 1) * (y - 2);
        b[14] = -1 / 36 * x * y * (x - 1) * (x - 2) * (y - 1) * (y + 1);
        b[15] = 1 / 36 * x * y * (x - 1) * (x + 1) * (y - 1) * (y + 1);

        Ave.rgbtBlue = b[0] * matrIn[Y0][X0].rgbtBlue + b[1] * matrIn[Y0 + 1][X0].rgbtBlue + b[2] * matrIn[Y0][X0 + 1].rgbtBlue + b[3] * matrIn[Y0 + 1][X0 + 1].rgbtBlue + b[4] * matrIn[Y0 - 1][X0].rgbtBlue + b[5] * matrIn[Y0][X0 - 1].rgbtBlue + b[6] * matrIn[Y0 - 1][X0 + 1].rgbtBlue + b[7] * matrIn[Y0 + 1][X0 - 1].rgbtBlue + b[8] * matrIn[Y0 + 2][X0].rgbtBlue + b[9] * matrIn[Y0][X0 + 2].rgbtBlue + b[10] * matrIn[Y0 - 1][X0 - 1].rgbtBlue + b[11] * matrIn[Y0 + 2][X0 + 1].rgbtBlue + b[12] * matrIn[Y0 + 1][X0 + 2].rgbtBlue + b[13] * matrIn[Y0 + 2][X0 - 1].rgbtBlue + b[14] * matrIn[Y0 - 1][X0 + 2].rgbtBlue + b[15] * matrIn[Y0 + 2][X0 + 2].rgbtBlue;
        Ave.rgbtGreen = b[0] * matrIn[Y0][X0].rgbtGreen + b[1] * matrIn[Y0 + 1][X0].rgbtGreen + b[2] * matrIn[Y0][X0 + 1].rgbtGreen + b[3] * matrIn[Y0 + 1][X0 + 1].rgbtGreen + b[4] * matrIn[Y0 - 1][X0].rgbtGreen + b[5] * matrIn[Y0][X0 - 1].rgbtGreen + b[6] * matrIn[Y0 - 1][X0 + 1].rgbtGreen + b[7] * matrIn[Y0 + 1][X0 - 1].rgbtGreen + b[8] * matrIn[Y0 + 2][X0].rgbtGreen + b[9] * matrIn[Y0][X0 + 2].rgbtGreen + b[10] * matrIn[Y0 - 1][X0 - 1].rgbtGreen + b[11] * matrIn[Y0 + 2][X0 + 1].rgbtGreen + b[12] * matrIn[Y0 + 1][X0 + 2].rgbtGreen + b[13] * matrIn[Y0 + 2][X0 - 1].rgbtGreen + b[14] * matrIn[Y0 - 1][X0 + 2].rgbtGreen + b[15] * matrIn[Y0 + 2][X0 + 2].rgbtGreen;
        Ave.rgbtRed = b[0] * matrIn[Y0][X0].rgbtRed + b[1] * matrIn[Y0 + 1][X0].rgbtRed + b[2] * matrIn[Y0][X0 + 1].rgbtRed + b[3] * matrIn[Y0 + 1][X0 + 1].rgbtRed + b[4] * matrIn[Y0 - 1][X0].rgbtRed + b[5] * matrIn[Y0][X0 - 1].rgbtRed + b[6] * matrIn[Y0 - 1][X0 + 1].rgbtRed + b[7] * matrIn[Y0 + 1][X0 - 1].rgbtRed + b[8] * matrIn[Y0 + 2][X0].rgbtRed + b[9] * matrIn[Y0][X0 + 2].rgbtRed + b[10] * matrIn[Y0 - 1][X0 - 1].rgbtRed + b[11] * matrIn[Y0 + 2][X0 + 1].rgbtRed + b[12] * matrIn[Y0 + 1][X0 + 2].rgbtRed + b[13] * matrIn[Y0 + 2][X0 - 1].rgbtRed + b[14] * matrIn[Y0 - 1][X0 + 2].rgbtRed + b[15] * matrIn[Y0 + 2][X0 + 2].rgbtRed;

        double sum = 0;
        for (int k = 0; k < 4; k++)
            for (int l = 0; l < 4; l++)
                sum += b[k][l] * matrIn[Y0 + k - 1][X0 + l - 1].rgbtBlue;

        Ave.norm();
        Out.rgbtBlue = Ave.rgbtBlue;
        Out.rgbtGreen = Ave.rgbtGreen;
        Out.rgbtRed = Ave.rgbtRed;

        return Out;
    }

    RGBTRIPLE getAve2Per(double Y1, double X1) {
        PIX Ave;
        Ave.clc();
        size_t x, y;
        x = X1;//округление вниз
        y = Y1;
        RGBTRIPLE Out;

        Ave.rgbtBlue = matrIn[y][x].rgbtBlue * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtBlue * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtBlue * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtBlue * (X1 - x) * (Y1 - y);
        Ave.rgbtGreen = matrIn[y][x].rgbtGreen * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtGreen * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtGreen * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtGreen * (X1 - x) * (Y1 - y);
        Ave.rgbtRed = matrIn[y][x].rgbtRed * (x + 1 - X1) * (y + 1 - Y1) + matrIn[y + 1][x].rgbtRed * (x + 1 - X1) * (Y1 - y) + matrIn[y][x + 1].rgbtRed * (X1 - x) * (y + 1 - Y1) + matrIn[y + 1][x + 1].rgbtRed * (X1 - x) * (Y1 - y);

        //sAve.norm();
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

        Ave.rgbtBlue = (matrIn[y][x].rgbtBlue + matrIn[y][x + 1].rgbtBlue + matrIn[y + 1][x].rgbtBlue + matrIn[y + 1][x + 1].rgbtBlue) / 4;
        Ave.rgbtGreen = (matrIn[y][x].rgbtGreen + matrIn[y][x + 1].rgbtGreen + matrIn[y + 1][x].rgbtGreen + matrIn[y + 1][x + 1].rgbtGreen) / 4;
        Ave.rgbtRed = (matrIn[y][x].rgbtRed + matrIn[y][x + 1].rgbtRed + matrIn[y + 1][x].rgbtRed + matrIn[y + 1][x + 1].rgbtRed) / 4;

        //Ave.norm();
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

        double HoutD, WoutD;

        Bx = Hin * sinA;
        By = Hin * cosA;

        Cx = Hin * sinA + Win * cosA;
        Cy = Hin * cosA -1*( Win * sinA);

        Dx = Win * cosA;
        Dy = -1 * (Win * sinA);

        dx = min(Ax, Bx);
        dx = min(dx, Cx);
        dx = min(dx, Dx);


        dy = min(Ay, By);
        dy = min(dy, Cy);
        dy = min(dy, Dy);


        WoutD = max(Ax, Bx);
        WoutD = max(WoutD, Cx);
        WoutD = max(WoutD, Dx)+1;
        WoutD -= dx;
        Wout = ceil(WoutD);
        Wout1 = Wout;

        HoutD = max(Ay, By);
        HoutD = max(HoutD, Cy);
        HoutD= max(HoutD, Dy)+1;
        HoutD -= dy;
        Hout = ceil(HoutD);
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

                Y1 = (x + dx) * sinA + (y + dy) * cosA + extend;
                X1 = (x + dx) * cosA - (y + dy) * sinA + extend;

                if (X1 >= extend && X1 < Win-1+extend && Y1 >= extend && Y1 < Hin-1+extend) {//---------------------------------
                    lineOut[x]= matrOrigin.getBilineWiki(Y1, X1);
                    //lineOut[x] = matrOrigin.getAve2Per(Y1, X1);
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

    cout << "\n\nразмер исходного изображения: " << bihIn.biHeight << "*" << bihIn.biWidth << endl;
    cout << "\n\nразмер итогового изображения: " << bihOut.biHeight << "*" << bihOut.biWidth << endl;

    
    DoFile2.OutFile(bihIn.biHeight, bihIn.biWidth, f2.getF(), matr.getM(), matr);

    return 0;
}