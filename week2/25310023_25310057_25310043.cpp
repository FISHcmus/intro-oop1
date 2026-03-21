#include <cmath>
#include <cstdio>

// Loai bo -0 (negative zero) de in dep hon
double fix0(double x) { return x == 0 ? 0 : x; }

// ============================================================
// 1. Giai phuong trinh bac 1: ax + b = 0
// ============================================================
void giaiPTBac1(double a, double b) {
    printf("Phuong trinh bac 1: %.4gx + %.4g = 0\n", a, b);
    if (a == 0) {
        if (b == 0)
            printf("Phuong trinh vo so nghiem.\n");
        else
            printf("Phuong trinh vo nghiem.\n");
    } else {
        double x = fix0(-b / a);
        printf("Nghiem: x = %.6g\n", x);
    }
    printf("\n");
}

// ============================================================
// 2. Giai phuong trinh bac 2: ax^2 + bx + c = 0
// ============================================================
void giaiPTBac2(double a, double b, double c) {
    printf("Phuong trinh bac 2: %.4gx^2 + %.4gx + %.4g = 0\n", a, b, c);
    if (a == 0) {
        giaiPTBac1(b, c);
        return;
    }
    double delta = b * b - 4 * a * c;
    if (delta > 0) {
        double x1 = (-b + sqrt(delta)) / (2 * a);
        double x2 = (-b - sqrt(delta)) / (2 * a);
        printf("Hai nghiem phan biet:\n");
        printf("  x1 = %.6g\n", x1);
        printf("  x2 = %.6g\n", x2);
    } else if (delta == 0) {
        double x = fix0(-b / (2 * a));
        printf("Nghiem kep: x = %.6g\n", x);
    } else {
        double real = fix0(-b / (2 * a));
        double imag = fabs(sqrt(-delta) / (2 * a));
        printf("Hai nghiem phuc:\n");
        printf("  x1 = %.6g + %.6gi\n", real, imag);
        printf("  x2 = %.6g - %.6gi\n", real, imag);
    }
    printf("\n");
}

// ============================================================
// 3. Giai phuong trinh bac 3: ax^3 + bx^2 + cx + d = 0
//    Su dung phuong phap Cardano
// ============================================================
void giaiPTBac3(double a, double b, double c, double d) {
    printf("Phuong trinh bac 3: %.4gx^3 + %.4gx^2 + %.4gx + %.4g = 0\n",
           a, b, c, d);
    if (a == 0) {
        giaiPTBac2(b, c, d);
        return;
    }

    // Chuyen ve dang t^3 + pt + q = 0 (dat x = t - b/(3a))
    double p = (3 * a * c - b * b) / (3 * a * a);
    double q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) /
               (27 * a * a * a);
    double discriminant = -4 * p * p * p - 27 * q * q;
    double offset = -b / (3 * a);

    if (discriminant > 0) {
        // 3 nghiem thuc phan biet (dung cong thuc luong giac)
        double m = 2 * sqrt(-p / 3);
        double theta = acos(3 * q / (p * m)) / 3;
        double x1 = m * cos(theta) + offset;
        double x2 = m * cos(theta - 2 * M_PI / 3) + offset;
        double x3 = m * cos(theta - 4 * M_PI / 3) + offset;
        printf("Ba nghiem thuc phan biet:\n");
        printf("  x1 = %.6g\n", x1);
        printf("  x2 = %.6g\n", x2);
        printf("  x3 = %.6g\n", x3);
    } else if (discriminant == 0) {
        // Nghiem boi
        if (p == 0 && q == 0) {
            printf("Nghiem ba: x = %.6g\n", offset);
        } else {
            double x1 = 3 * q / p + offset;
            double x2 = -3 * q / (2 * p) + offset;
            printf("Mot nghiem don va mot nghiem kep:\n");
            printf("  x1 = %.6g\n", x1);
            printf("  x2 = x3 = %.6g\n", x2);
        }
    } else {
        // 1 nghiem thuc, 2 nghiem phuc (Cardano)
        double D = q * q / 4 + p * p * p / 27;
        double sqrtD = sqrt(D);
        double u = cbrt(-q / 2 + sqrtD);
        double v = cbrt(-q / 2 - sqrtD);
        double x1 = u + v + offset;
        double realPart = -(u + v) / 2 + offset;
        double imagPart = (u - v) * sqrt(3.0) / 2;
        printf("Mot nghiem thuc, hai nghiem phuc:\n");
        printf("  x1 = %.6g\n", x1);
        printf("  x2 = %.6g + %.6gi\n", realPart, fabs(imagPart));
        printf("  x3 = %.6g - %.6gi\n", realPart, fabs(imagPart));
    }
    printf("\n");
}

// ============================================================
// 4. Giai phuong trinh bac 4 trung phuong: ax^4 + bx^2 + c = 0
//    Dat t = x^2, giai ax^2 + bx + c = 0 (theo t), roi tinh x
// ============================================================
void giaiPTBac4TrungPhuong(double a, double b, double c) {
    printf("Phuong trinh trung phuong: %.4gx^4 + %.4gx^2 + %.4g = 0\n",
           a, b, c);
    if (a == 0) {
        giaiPTBac2(b, 0, c); // suy bien thanh bx^2 + c = 0
        return;
    }

    // Giai phuong trinh theo t = x^2: at^2 + bt + c = 0
    double delta = b * b - 4 * a * c;
    printf("Dat t = x^2, giai: %.4gt^2 + %.4gt + %.4g = 0\n", a, b, c);

    if (delta < 0) {
        printf("Delta < 0 => Phuong trinh vo nghiem thuc.\n");
    } else if (delta == 0) {
        double t = -b / (2 * a);
        printf("  t = %.6g\n", t);
        if (t < 0) {
            printf("t < 0 => Phuong trinh vo nghiem thuc.\n");
        } else if (t == 0) {
            printf("Nghiem: x = 0\n");
        } else {
            printf("Hai nghiem:\n");
            printf("  x1 = %.6g\n", sqrt(t));
            printf("  x2 = %.6g\n", -sqrt(t));
        }
    } else {
        double t1 = (-b + sqrt(delta)) / (2 * a);
        double t2 = (-b - sqrt(delta)) / (2 * a);
        printf("  t1 = %.6g, t2 = %.6g\n", t1, t2);

        int soNghiem = 0;
        double nghiem[4];

        if (t1 > 0) {
            nghiem[soNghiem++] = sqrt(t1);
            nghiem[soNghiem++] = -sqrt(t1);
        } else if (t1 == 0) {
            nghiem[soNghiem++] = 0;
        }

        if (t2 > 0) {
            nghiem[soNghiem++] = sqrt(t2);
            nghiem[soNghiem++] = -sqrt(t2);
        } else if (t2 == 0) {
            // Tranh trung nghiem 0 neu t1 da = 0
            bool da_co_0 = false;
            for (int i = 0; i < soNghiem; i++) {
                if (nghiem[i] == 0) { da_co_0 = true; break; }
            }
            if (!da_co_0) nghiem[soNghiem++] = 0;
        }

        if (soNghiem == 0) {
            printf("Ca hai t < 0 => Phuong trinh vo nghiem thuc.\n");
        } else {
            printf("Cac nghiem (%d nghiem):\n", soNghiem);
            for (int i = 0; i < soNghiem; i++) {
                printf("  x%d = %.6g\n", i + 1, nghiem[i]);
            }
        }
    }
    printf("\n");
}

// ============================================================
// Chuong trinh chinh
// ============================================================
int main() {
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
            printf("Nhap a, b: ");
            scanf("%lf %lf", &a, &b);
            giaiPTBac1(a, b);
            break;
        }
        case 2: {
            double a, b, c;
            printf("Nhap a, b, c: ");
            scanf("%lf %lf %lf", &a, &b, &c);
            giaiPTBac2(a, b, c);
            break;
        }
        case 3: {
            double a, b, c, d;
            printf("Nhap a, b, c, d: ");
            scanf("%lf %lf %lf %lf", &a, &b, &c, &d);
            giaiPTBac3(a, b, c, d);
            break;
        }
        case 4: {
            double a, b, c;
            printf("Nhap a, b, c: ");
            scanf("%lf %lf %lf", &a, &b, &c);
            giaiPTBac4TrungPhuong(a, b, c);
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

    return 0;
}
