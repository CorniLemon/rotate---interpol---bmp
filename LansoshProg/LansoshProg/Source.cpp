#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

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

int main(){
    FileWithDes f("LansoshProg.lans", "wb");
    double L = 0;
    int a = 3;

    for (double i = -3; i <0; i += 0.001) {
        L = a * sin(M_PI * i) * sin(M_PI * i / a) / (M_PI * M_PI * i * i);
        fwrite(&L, sizeof(L), 1, f.getF());
    }

    L = 1;
    fwrite(&L, sizeof(L), 1, f.getF());

    for (double i = 0.001; i <= 3; i += 0.001) {
        L = a * sin(M_PI * i) * sin(M_PI * i / a) / (M_PI * M_PI * i * i);
        fwrite(&L, sizeof(L), 1, f.getF());
    }

    return 0;
}
