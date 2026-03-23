#include "../header/EquationIO.h"
#include "../header/Equation.h"
#include <cstdio>

// ============================================================
// Doc he so tu ban phim
// ============================================================
void docPTBac1(double &a, double &b) {
    printf("Nhap a, b: ");
    scanf("%lf %lf", &a, &b);
}

void docPTBac2(double &a, double &b, double &c) {
    printf("Nhap a, b, c: ");
    scanf("%lf %lf %lf", &a, &b, &c);
}

void docPTBac3(double &a, double &b, double &c, double &d) {
    printf("Nhap a, b, c, d: ");
    scanf("%lf %lf %lf %lf", &a, &b, &c, &d);
}

void docPTBac4TrungPhuong(double &a, double &b, double &c) {
    printf("Nhap a, b, c: ");
    scanf("%lf %lf %lf", &a, &b, &c);
}

// ============================================================
// In dang phuong trinh + goi ham giai de in ket qua
// ============================================================
void xuatPTBac1(double a, double b) {
    printf("Phuong trinh bac 1: %.4gx + %.4g = 0\n", a, b);
    giaiPTBac1(a, b);
    printf("\n");
}

void xuatPTBac2(double a, double b, double c) {
    printf("Phuong trinh bac 2: %.4gx^2 + %.4gx + %.4g = 0\n", a, b, c);
    giaiPTBac2(a, b, c);
    printf("\n");
}

void xuatPTBac3(double a, double b, double c, double d) {
    printf("Phuong trinh bac 3: %.4gx^3 + %.4gx^2 + %.4gx + %.4g = 0\n",
           a, b, c, d);
    giaiPTBac3(a, b, c, d);
    printf("\n");
}

void xuatPTBac4TrungPhuong(double a, double b, double c) {
    printf("Phuong trinh trung phuong: %.4gx^4 + %.4gx^2 + %.4g = 0\n",
           a, b, c);
    giaiPTBac4TrungPhuong(a, b, c);
    printf("\n");
}

// ============================================================
// Menu chinh
// ============================================================
void chayMenu() {
    int lua_chon;

    do {
        printf("========================================\n");
        printf("  GIAI PHUONG TRINH\n");
        printf("========================================\n");
        printf("  1. Phuong trinh bac 1 (ax + b = 0)\n");
        printf("  2. Phuong trinh bac 2 (ax^2+bx+c = 0)\n");
        printf("  3. Phuong trinh bac 3 (ax^3+bx^2+cx+d = 0)\n");
        printf("  4. PT bac 4 trung phuong (ax^4+bx^2+c = 0)\n");
        printf("  0. Thoat\n");
        printf("Lua chon: ");
        scanf("%d", &lua_chon);

        switch (lua_chon) {
        case 1: {
            double a, b;
            docPTBac1(a, b);
            xuatPTBac1(a, b);
            break;
        }
        case 2: {
            double a, b, c;
            docPTBac2(a, b, c);
            xuatPTBac2(a, b, c);
            break;
        }
        case 3: {
            double a, b, c, d;
            docPTBac3(a, b, c, d);
            xuatPTBac3(a, b, c, d);
            break;
        }
        case 4: {
            double a, b, c;
            docPTBac4TrungPhuong(a, b, c);
            xuatPTBac4TrungPhuong(a, b, c);
            break;
        }
        case 0:
            printf("Ket thuc chuong trinh.\n");
            break;
        default:
            printf("Lua chon khong hop le!\n\n");
            break;
        }
    } while (lua_chon != 0);
}
