// 25310023 - Nguyen Huu Thien Nhan
// 25310057 - Bui Thi Minh Hang
// 25310043 - Pham Ngoc Tram
#include <cstdio>
#include <cstdlib>
#include <ctime>

int docSoNguyen(const char *loiNhac) {
    char buf[64];
    printf("%s", loiNhac);
    scanf("%63s", buf);
    char *end;
    long val = strtol(buf, &end, 10);
    if (end == buf) {
        printf("Nhap khong hop le!\n");
        return 0;
    }
    return static_cast<int>(val);
}

long long docSoLon(const char *loiNhac) {
    char buf[64];
    printf("%s", loiNhac);
    scanf("%63s", buf);
    char *end;
    long long val = strtoll(buf, &end, 10);
    if (end == buf) {
        printf("Nhap khong hop le!\n");
        return 0;
    }
    return val;
}

int kiemTraNamNhuan(int nam) {
    if (nam < 1) return 0;
    if ((nam % 4 == 0 && nam % 100 != 0) || (nam % 400 == 0)) return 1;
    return 0;
}

int soNgayTrongThang(int thang, int nam) {
    switch (thang) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return kiemTraNamNhuan(nam) ? 29 : 28;
        default:
            return -1;
    }
}

void tinhTuoi() {
    int ngay = docSoNguyen("Nhap ngay: ");
    int thang = docSoNguyen("Nhap thang: ");
    int nam = docSoNguyen("Nhap nam sinh: ");

    if (thang < 1 || thang > 12) {
        printf("Thang khong hop le!\n\n");
        return;
    }
    int soNgayMax = soNgayTrongThang(thang, nam);
    if (ngay < 1 || ngay > soNgayMax) {
        printf("Ngay khong hop le! Thang %d nam %d co %d ngay.\n\n",
               thang, nam, soNgayMax);
        return;
    }

    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    int namHienTai = 1900 + ltm->tm_year;
    int thangHienTai = 1 + ltm->tm_mon;
    int ngayHienTai = ltm->tm_mday;

    int tuoi = namHienTai - nam;
    if (thangHienTai < thang ||
        (thangHienTai == thang && ngayHienTai < ngay))
        tuoi--;

    if (tuoi < 0) {
        printf("Ngay sinh o tuong lai!\n\n");
        return;
    }
    printf("Tuoi cua ban: %d\n\n", tuoi);
}

void inHoa() {
    char ch;
    printf("Nhap mot ky tu: ");
    scanf(" %c", &ch);

    if (ch >= 'a' && ch <= 'z') {
        ch = static_cast<char>(ch - 'a' + 'A');
        printf("Ky tu in hoa: %c\n\n", ch);
    } else if (ch >= 'A' && ch <= 'Z') {
        printf("Da la chu in hoa: %c\n\n", ch);
    } else {
        printf("Khong phai ky tu chu cai: %c\n\n", ch);
    }
}

void tachMenhGia() {
    long long soTien = docSoLon("Nhap so tien (VND): ");

    if (soTien <= 0) {
        printf("So tien phai duong!\n");
        return;
    }
    if (soTien % 10000 != 0) {
        printf("So tien phai la boi cua 10000!\n");
        return;
    }

    printf("%lld = ", soTien);
    for (int i = 0; i < 6; i++) {
        const long long menhGia[] = {500000, 200000, 100000, 50000, 20000, 10000};
        if (i > 0) printf(" + ");
        printf("%lld * %lld", soTien / menhGia[i], menhGia[i]);
        soTien = soTien % menhGia[i];
    }
    printf("\n");
}

int main() {
    tinhTuoi();
    inHoa();
    tachMenhGia();
    return 0;
}
