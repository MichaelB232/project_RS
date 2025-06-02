#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cctype> // buat tolower
using namespace std;

const int MAX_DATA = 100;
const int Durasi_Janjitemu = 45;

struct Pasien_t
{
    string nama;
    string no_pendaftaran;
    string gol_darah;
};
typedef struct Pasien_t Pasien;

struct JadwalPraktikHarian_t
{
    string hari;
    string jam_mulai;
    string jam_selesai;
    bool aktif;

    JadwalPraktikHarian_t()
    {
        aktif = false;
    }
};
typedef struct JadwalPraktikHarian_t JadwalPraktikHarian;

const int MAX_JADWAL_DOKTER = 7;

struct Dokter_t
{
    string id_dokter;
    string nama_dokter;
    string spesialisasi;
    JadwalPraktikHarian daftar_jadwal[MAX_JADWAL_DOKTER];
    int jumlah_jadwal_aktif;

    Dokter_t()
    {
        jumlah_jadwal_aktif = 0;
    }
};
typedef struct Dokter_t Dokter;

struct JanjiTemu_t
{
    string no_pendaftaran_pasien;
    string id_dokter;
    string tanggal; // Format YYYY-MM-DD
    string jam;     // Format HH:MM
    string keluhan;
};
typedef struct JanjiTemu_t JanjiTemu;

struct Waktu
{
    int jam;
    int menit;
    int totalMenit;
    Waktu() : jam(0), menit(0), totalMenit(0) {}
    Waktu(const string &time_str)
    {
        if (time_str.length() == 5 && time_str[2] == ':')
        {
            try
            {
                int h = stoi(time_str.substr(0, 2));
                int m = stoi(time_str.substr(3, 2));
                if (h >= 0 && h <= 23 && m >= 0 && m <= 59)
                {
                    jam = h;
                    menit = m;
                    totalMenit = h * 60 + m;
                }
                else
                {
                    jam = -1;
                    menit = -1;
                    totalMenit = -1;
                }
            }
            catch (const std::exception &)
            {
                // Jika ada input yang tidak valid misal AB:15.
                jam = -1;
                menit = -1;
                totalMenit = -1;
            }
        }
        else
        {
            jam = -1;
            menit = -1;
            totalMenit = -1;
        }
    }

    Waktu(int h, int m)
    { // Konstruktor overload
        if (h >= 0 && h <= 23 && m >= 0 && m <= 59)
        {
            jam = h;
            menit = m;
            totalMenit = h * 60 + m;
        }
        else
        {
            jam = -1;
            menit = -1;
            totalMenit = -1;
        }
    }

    bool isValid() const
    {
        return jam != -1 && menit != -1;
    }

    string waktuToString() const
    {
        if (!isValid())
            return "InvalidTime";
        stringstream ss_time;
        ss_time << setfill('0') << setw(2) << jam << ":" << setw(2) << menit;
        return ss_time.str();
    }
};

JanjiTemu jadwal[MAX_DATA];
Dokter listDokter[MAX_DATA];
Pasien p[MAX_DATA];

int total_janji = 0;
bool Login = false;

void TampilkanHeader(const string &judul);
void ClearScreen();
void PauseSystem();
void MainMenu();

void DataDokter(Dokter dokter_arr[], int &total_dokter);
void InputDokter(Dokter dokter_arr[], int &total_dokter);
void ListDokter(const Dokter dokter_arr[], int total_dokter);
void SearchDokter(const Dokter dokter_arr[], int total_dokter);
void SimpanDataDokterKeFile(const Dokter dokter_arr[], int total_dokter);
int LoadDataDokterDariFile(Dokter dokter_arr[]);
bool DeleteDataDokter(Dokter dokter_arr[], int &total_dokter);

void DataPasien(Pasien pasien_arr[], int &total_pasien);
void InputPasien(Pasien pasien_arr[], int &total_pasien);
void ListPasien(const Pasien pasien_arr[], int total_pasien);
void SearchPasien(const Pasien pasien_arr[], int total_pasien, string search_no_pasien, int cari_pasien_idx);
void SortPasienByNoPendaftaran(Pasien pasien_arr[], int total_pasien);
void SimpanDataPasienKeFile(const Pasien pasien_arr[], int total_pasien);
int LoadDataPasienDariFile(Pasien pasien_arr[]);
bool DeleteDataPasien(Pasien pasien_arr[], int &total_pasien);

void MenuJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter);
bool CekJadwalTabrakan(const string &id_dokter_check, const string &tanggal_check, const string &jam_check);
void BuatJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter);
void LihatJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter);
void SimpanJanjiKeFile();
void LoadJanjiDariFile();
bool CekJamPraktikDokter(const string &id_dokter_check, const string &nama_hari_janji, const string &jam_janji_str, const Dokter dokter_arr[], int total_dokter);

int main()
{
    system("cls");
    int total_dokter = LoadDataDokterDariFile(listDokter);
    int total_pasien = LoadDataPasienDariFile(p);
    LoadJanjiDariFile();
    MainMenu();
    return 0;
}

void TampilkanHeader(const string &judul)
{
    int lebarkolom = 65;
    int panjangjudul = judul.length();
    int center = (lebarkolom - panjangjudul) / 2;

    cout << "===============================================================\n";
    cout << "|                 SISTEM MANAJEMEN RUMAH SAKIT                |\n";
    cout << "===============================================================\n";
    cout << string(center, ' ') << judul << "\n";
    cout << "===============================================================\n";
}

void ClearScreen()
{
    system("cls");
}

void PauseSystem()
{
    cout << "\nTekan Enter untuk melanjutkan...";
    cin.ignore();
    cin.get();
}

void MainMenu()
{
    int menu_choice;
    string username_input;
    string password_input;

    if (!Login)
    {
        TampilkanHeader("HALAMAN LOGIN");
        do
        {
            cout << "Username : ";
            cin >> username_input;
            cout << "Password : ";
            cin >> password_input;
            if (username_input != "Admin" || password_input != "Admin123")
            {
                cout << "\nUsername atau Password Salah! Silakan coba lagi.\n\n";
            }
            else
            {
                cout << "\nAnda berhasil Login!" << endl;
                Login = true;
                cin.ignore();
                PauseSystem();
            }
        } while (!Login);
    }

    do
    {
        ClearScreen();
        int total_pasien = LoadDataPasienDariFile(p);
        int total_dokter = LoadDataDokterDariFile(listDokter);
        LoadJanjiDariFile();

        TampilkanHeader("MENU UTAMA");
        cout << "| 1. Data Pasien                                              |\n";
        cout << "| 2. Data Dokter                                              |\n";
        cout << "| 3. Jadwal Janji Temu                                        |\n";
        cout << "| 0. Keluar                                                   |\n";
        cout << "===============================================================\n";
        cout << "Pilih Menu : ";
        cin >> menu_choice;

        while (menu_choice < 0 || menu_choice > 3)
        {
            cout << "Input tidak valid. Masukkan angka menu (0-3): ";
            cin.clear();
            cin.ignore();
            cin >> menu_choice;
        }

        switch (menu_choice)
        {
        case 0:
            SimpanDataDokterKeFile(listDokter, total_dokter);
            SimpanDataPasienKeFile(p, total_pasien);
            SimpanJanjiKeFile();
            ClearScreen();
            TampilkanHeader("KELUAR PROGRAM");
            cout << "Menyimpan semua data...\n";
            cout << "Terima kasih telah menggunakan sistem ini.\n\n";
            cin.ignore();
            PauseSystem();
            exit(0);
        case 1:
            DataPasien(p, total_pasien);
            break;
        case 2:
            DataDokter(listDokter, total_dokter);
            break;
        case 3:
            MenuJanjiTemu(p, total_pasien, listDokter, total_dokter);
            break;
        default:
            cout << "Pilihan tidak valid." << endl;
            cin.ignore();
            PauseSystem();
            break;
        }
    } while (true);
}

void DataDokter(Dokter dokter_arr[], int &total_dokter)
{
    int menu_dokter;
    bool delete_dokter;
    do
    {
        ClearScreen();
        total_dokter = LoadDataDokterDariFile(dokter_arr);
        TampilkanHeader("MENU DATA DOKTER (Total: " + to_string(total_dokter) + ")");
        cout << "| 1. Input Data Dokter Baru                                   |\n";
        cout << "| 2. Lihat Data Dokter                                        |\n";
        cout << "| 3. Cari Data Dokter (berdasarkan ID)                        |\n";
        cout << "| 4. Hapus Data Dokter                                        |\n";
        cout << "| 0. Kembali ke Menu Utama                                    |\n";
        cout << "===============================================================\n";
        cout << "Pilih menu: ";
        cin >> menu_dokter;

        while (menu_dokter < 0 || menu_dokter > 4)
        {
            cout << "Input tidak valid. Masukkan angka menu (0-3): ";
            cin.clear();
            cin.ignore();
            cin >> menu_dokter;
        }

        switch (menu_dokter)
        {
        case 1:
            ClearScreen();
            TampilkanHeader("INPUT DATA DOKTER BARU");
            InputDokter(dokter_arr, total_dokter);
            SimpanDataDokterKeFile(dokter_arr, total_dokter);
            PauseSystem();
            break;
        case 2:
            ClearScreen();
            TampilkanHeader("Lihat DATA DOKTER");
            ListDokter(dokter_arr, total_dokter);
            PauseSystem();
            break;
        case 3:
            ClearScreen();
            TampilkanHeader("CARI DATA DOKTER");
            SearchDokter(dokter_arr, total_dokter);
            PauseSystem();
            break;
        case 4:
            ClearScreen();
            TampilkanHeader("HAPUS DATA DOKTER");
            ListDokter(dokter_arr, total_dokter);
            delete_dokter = DeleteDataDokter(dokter_arr, total_dokter);
            if (delete_dokter == true)
            {
                cout << "Data Dokter Berhasil Dihapus!!\n";
                SimpanDataDokterKeFile(dokter_arr, total_dokter);
            }
            else
            {
                cout << "ID Dokter Tidak Ditemukan!!";
            }
            PauseSystem();
            break;
        case 0:
            return;
        }
    } while (true);
}

void InputDokter(Dokter dokter_arr[], int &total_dokter)
{
    if (total_dokter >= MAX_DATA)
    {
        cout << "Kapasitas data dokter penuh! Tidak dapat menambah data baru." << endl;
        return;
    }
    string id_input;
    bool id_exists;

    cout << "Data Dokter Ke-" << total_dokter + 1 << " (dari maks " << MAX_DATA << "):\n";
    do
    {
        id_exists = false;
        cout << "ID Dokter (Unique)       : ";
        cin >> id_input;
        cin.ignore();
        for (int j = 0; j < total_dokter; ++j)
        {
            if (dokter_arr[j].id_dokter == id_input)
            {
                id_exists = true;
                cout << "ID Dokter '" << id_input << "' sudah digunakan! Masukkan ID yang lain." << endl;
                break;
            }
        }
    } while (id_exists);

    Dokter &new_dokter = dokter_arr[total_dokter];
    new_dokter.id_dokter = id_input;
    cout << "Nama Dokter              : ";
    getline(cin >> ws, new_dokter.nama_dokter);
    cout << "Spesialisasi             : ";
    getline(cin >> ws, new_dokter.spesialisasi);

    cout << "Input Jadwal Praktik Dokter:" << endl;
    new_dokter.jumlah_jadwal_aktif = 0;
    char tambah_jadwal_lagi;
    for (int k = 0; k < MAX_JADWAL_DOKTER; ++k)
    {
        cout << "--- Jadwal Praktik Ke-" << new_dokter.jumlah_jadwal_aktif + 1 << " ---" << endl;
        JadwalPraktikHarian &jadwal_current = new_dokter.daftar_jadwal[new_dokter.jumlah_jadwal_aktif];

        cout << "Hari                        : ";
        getline(cin >> ws, jadwal_current.hari);

        bool waktu_valid;
        string jam_str_input;
        do
        {
            waktu_valid = true;
            cout << "Jam Mulai (HH:MM)           : ";
            cin >> jam_str_input;
            cin.ignore();
            jadwal_current.jam_mulai = jam_str_input;
            Waktu temp_mulai(jadwal_current.jam_mulai);
            if (!temp_mulai.isValid())
            {
                cout << "Format jam mulai tidak valid atau nilai di luar rentang. Ulangi." << endl;
                waktu_valid = false;
            }
        } while (!waktu_valid);

        do
        {
            waktu_valid = true;
            cout << "Jam Selesai (HH:MM)         : ";
            cin >> jam_str_input;
            cin.ignore();
            jadwal_current.jam_selesai = jam_str_input;
            Waktu temp_selesai(jadwal_current.jam_selesai);
            Waktu temp_mulai(jadwal_current.jam_mulai);
            if (!temp_selesai.isValid())
            {
                cout << "Format jam selesai tidak valid atau nilai di luar rentang. Ulangi." << endl;
                waktu_valid = false;
            }
            else if (temp_mulai.isValid() && temp_selesai.totalMenit <= temp_mulai.totalMenit)
            {
                cout << "Jam selesai harus setelah jam mulai. Ulangi." << endl;
                waktu_valid = false;
            }
        } while (!waktu_valid);

        jadwal_current.aktif = true;
        new_dokter.jumlah_jadwal_aktif++;

        if (new_dokter.jumlah_jadwal_aktif >= MAX_JADWAL_DOKTER)
        {
            cout << "Jumlah maksimum jadwal per dokter (" << MAX_JADWAL_DOKTER << ") telah tercapai." << endl;
            break;
        }

        cout << "Tambah jadwal praktik lain untuk dokter ini? (y/n): ";
        cin >> tambah_jadwal_lagi;
        cin.ignore();
        if (tolower(tambah_jadwal_lagi) != 'y')
        {
            break;
        }
    }

    total_dokter++;
    cout << "\nData dokter berhasil ditambahkan." << endl;
}

void ListDokter(const Dokter dokter_arr[], int total_dokter)
{
    if (total_dokter == 0)
    {
        cout << "Belum ada data dokter yang tersimpan." << endl;
        return;
    }
    cout << "\n--- Daftar Semua Dokter ---\n";
    cout << "----------------------------------------------------------------------------------------------------------------------\n";
    cout << left << setw(5) << "No." << setw(15) << "ID Dokter" << setw(25) << "Nama Dokter" << setw(20) << "Spesialisasi" << "Jadwal Praktik" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------\n";
    for (int i = 0; i < total_dokter; ++i)
    {
        cout << left << setw(5) << (i + 1);
        cout << setw(15) << dokter_arr[i].id_dokter
             << setw(25) << dokter_arr[i].nama_dokter
             << setw(20) << dokter_arr[i].spesialisasi;

        stringstream jadwal_ss;
        if (dokter_arr[i].jumlah_jadwal_aktif == 0)
        {
            jadwal_ss << "Belum ada jadwal";
        }
        else
        {
            for (int j = 0; j < dokter_arr[i].jumlah_jadwal_aktif; j++)
            {
                const auto &jd = dokter_arr[i].daftar_jadwal[j];
                if (jd.aktif)
                {
                    jadwal_ss << jd.hari << " (" << jd.jam_mulai << "-" << jd.jam_selesai << ")";
                    if (j < dokter_arr[i].jumlah_jadwal_aktif - 1)
                    {
                        jadwal_ss << ", ";
                    }
                }
            }
        }
        cout << jadwal_ss.str() << endl;
    }
    cout << "----------------------------------------------------------------------------------------------------------------------\n";
}

bool DeleteDataDokter(Dokter dokter_arr[], int &total_dokter)
{
    string dokter_id;
    cout << "Masukkan ID Dokter yang akan dihapus : ";
    cin >> dokter_id;
    bool found = false;
    int search_dokter = 0;
    while (search_dokter < total_dokter && !found)
    {
        if (dokter_arr[search_dokter].id_dokter == dokter_id)
        {
            found = true;
        }
        else
        {
            search_dokter++;
        }
    }
    if (found)
    {
        for (int i = search_dokter; i < total_dokter - 1; i++)
        {
            dokter_arr[i] = dokter_arr[i + 1];
        }
        total_dokter--;
        return true;
    }
    else
    {
        return false;
    }
}

void SearchDokter(const Dokter dokter_arr[], int total_dokter)
{
    if (total_dokter == 0)
    {
        cout << "Belum ada data dokter untuk dicari." << endl;
        return;
    }
    string search_id;
    cout << "Masukkan ID Dokter yang dicari: ";
    cin >> search_id;
    cin.ignore();
    bool found = false;
    int idx = 0;
    while (!found && idx < total_dokter)
    {
        if (dokter_arr[idx].id_dokter == search_id)
        {
            found = true;
        }
        else
        {
            idx++;
        }
    }
    if (!found)
    {
        cout << "\nData Dokter dengan ID '" << search_id << "' tidak ditemukan." << endl;
    }
    else
    {
        cout << "\n--- Data Dokter Ditemukan ---\n";
        cout << "ID Dokter      : " << dokter_arr[idx].id_dokter << endl;
        cout << "Nama Dokter    : " << dokter_arr[idx].nama_dokter << endl;
        cout << "Spesialisasi   : " << dokter_arr[idx].spesialisasi << endl;
        cout << "Jadwal Praktik : ";
        stringstream jadwal_ss_search;
        if (dokter_arr[idx].jumlah_jadwal_aktif == 0)
        {
            jadwal_ss_search << "Belum ada jadwal";
        }
        else
        {
            for (int j = 0; j < dokter_arr[idx].jumlah_jadwal_aktif; ++j)
            {
                const auto &jd = dokter_arr[idx].daftar_jadwal[j];
                if (jd.aktif)
                {
                    jadwal_ss_search << jd.hari << " (" << jd.jam_mulai << "-" << jd.jam_selesai << ")";
                    if (j < dokter_arr[idx].jumlah_jadwal_aktif - 1)
                    {
                        jadwal_ss_search << ",";
                    }
                }
            }
        }
        cout << jadwal_ss_search.str() << endl;
        cout << "-----------------------------\n";
    }
}

void SimpanDataDokterKeFile(const Dokter dokter_arr[], int total_dokter)
{
    ofstream file("data_dokter.csv");
    if (file.is_open())
    {
        for (int i = 0; i < total_dokter; i++)
        {
            file << dokter_arr[i].id_dokter << ","
                 << dokter_arr[i].nama_dokter << ","
                 << dokter_arr[i].spesialisasi << ",";

            stringstream jadwal_gabung_ss;
            for (int j = 0; j < dokter_arr[i].jumlah_jadwal_aktif; ++j)
            {
                const auto &jd = dokter_arr[i].daftar_jadwal[j];
                if (jd.aktif)
                {
                    jadwal_gabung_ss << jd.hari << ":" << jd.jam_mulai << "-" << jd.jam_selesai;
                    if (j < dokter_arr[i].jumlah_jadwal_aktif - 1)
                    {
                        jadwal_gabung_ss << "|";
                    }
                }
            }
            file << jadwal_gabung_ss.str() << endl;
        }
        file.close();
    }
    else
    {
        cout << "ERROR: Gagal membuka file untuk menyimpan data dokter!" << endl;
    }
}

int LoadDataDokterDariFile(Dokter dokter_arr[])
{
    ifstream file("data_dokter.csv");
    string line;
    int count = 0;
    if (!file.is_open())
        return 0;

    while (count < MAX_DATA && getline(file, line))
    {
        stringstream ss(line);
        if (line.empty())
            continue;

        Dokter &current_doc = dokter_arr[count];
        current_doc.jumlah_jadwal_aktif = 0;

        getline(ss, current_doc.id_dokter, ',');
        getline(ss, current_doc.nama_dokter, ',');
        getline(ss, current_doc.spesialisasi, ',');

        string semua_jadwal_str;
        getline(ss, semua_jadwal_str);

        if (!current_doc.id_dokter.empty())
        {
            stringstream jadwal_parser_ss(semua_jadwal_str);
            string satu_jadwal_str;

            while (current_doc.jumlah_jadwal_aktif < MAX_JADWAL_DOKTER &&
                   getline(jadwal_parser_ss, satu_jadwal_str, '|'))
            {

                if (satu_jadwal_str.empty())
                    continue;

                JadwalPraktikHarian &jd_load = current_doc.daftar_jadwal[current_doc.jumlah_jadwal_aktif];

                size_t pos_titikdua = satu_jadwal_str.find(':');
                size_t pos_strip = satu_jadwal_str.find('-');

                if (pos_titikdua != string::npos && pos_strip != string::npos && pos_strip > pos_titikdua + 1 && pos_titikdua > 0)
                {
                    jd_load.hari = satu_jadwal_str.substr(0, pos_titikdua);
                    jd_load.jam_mulai = satu_jadwal_str.substr(pos_titikdua + 1, pos_strip - (pos_titikdua + 1));
                    jd_load.jam_selesai = satu_jadwal_str.substr(pos_strip + 1);

                    Waktu cek_mulai(jd_load.jam_mulai);
                    Waktu cek_selesai(jd_load.jam_selesai);
                    if (cek_mulai.isValid() && cek_selesai.isValid())
                    {
                        jd_load.aktif = true;
                        current_doc.jumlah_jadwal_aktif++;
                    }
                }
            }
            count++;
        }
    }
    file.close();
    return count;
}

void DataPasien(Pasien pasien_arr[], int &total_pasien)
{
    int menu_pasien;
    bool delete_pasien;
    do
    {
        ClearScreen();
        total_pasien = LoadDataPasienDariFile(pasien_arr);
        TampilkanHeader("MENU DATA PASIEN (Total: " + to_string(total_pasien) + ")");
        cout << "| 1. Input Data Pasien Baru                                   |\n";
        cout << "| 2. Lihat Data Pasien                                        |\n";
        cout << "| 3. Delete Data Pasien                                       |\n";
        cout << "| 4. Search Data Pasien                                       |\n";
        cout << "| 0. Kembali ke Menu Utama                                    |\n";
        cout << "===============================================================\n";
        cout << "Pilih Menu : ";
        cin >> menu_pasien;
        while (menu_pasien < 0 || menu_pasien > 4)
        {
            cout << "Input tidak valid. Masukkan angka menu (0-4): ";
            cin.clear();
            cin.ignore();
            cin >> menu_pasien;
        }

        switch (menu_pasien)
        {
        case 1:
            ClearScreen();
            TampilkanHeader("INPUT DATA PASIEN BARU");
            InputPasien(pasien_arr, total_pasien);
            SimpanDataPasienKeFile(pasien_arr, total_pasien);
            PauseSystem();
            break;
        case 2:
            ClearScreen();
            TampilkanHeader("Lihat DATA PASIEN");
            SortPasienByNoPendaftaran(pasien_arr, total_pasien);
            SimpanDataPasienKeFile(pasien_arr, total_pasien);
            ListPasien(pasien_arr, total_pasien);
            PauseSystem();
            break;
        case 3:
            ClearScreen();
            TampilkanHeader("DELETE DATA PASIEN");
            ListPasien(pasien_arr, total_pasien);
            delete_pasien = DeleteDataPasien(pasien_arr, total_pasien);
            if (delete_pasien == true)
            {
                cout << "Data Pasien berhasil dihapus!!\n";
                SimpanDataPasienKeFile(pasien_arr, total_pasien);
            }
            else
            {
                cout << "No Registrasi Pasien Tidak Ditemukan!!";
            }
            PauseSystem();
            break;
        case 4:
            ClearScreen();
            TampilkanHeader("SEARCH DATA PASIEN");
            SearchPasien(pasien_arr,total_pasien,"",0);
            PauseSystem();
        case 0:
            return;
        }
    } while (true);
}

void InputPasien(Pasien pasien_arr[], int &total_pasien)
{
    int jumlah_input_pasien;
    cout << "Masukkan Jumlah Pasien yang ingin diinput: ";
    cin >> jumlah_input_pasien;

    while (jumlah_input_pasien < 0)
    {
        cout << "Input jumlah tidak valid. Masukkan angka positif: ";
        cin.clear();
        cin.ignore();
        cin >> jumlah_input_pasien;
    }

    if (jumlah_input_pasien == 0)
        return;

    int jumlah_input_berhasil = 0;
    while (jumlah_input_berhasil < jumlah_input_pasien)
    {
        if (total_pasien >= MAX_DATA)
        {
            cout << "\nKapasitas data pasien penuh! Hanya " << jumlah_input_berhasil << " data yang berhasil ditambahkan." << endl;
            break;
        }
        string no_reg;
        bool no_reg_exists;
        cout << "\n--- Input Data Pasien Ke-" << total_pasien + 1 << " (dari maks " << MAX_DATA << ") ---" << endl;
        do
        {
            no_reg_exists = false;
            cout << "No. Pendaftaran (unik) : ";
            cin >> no_reg;

            for (int j = 0; j < total_pasien; j++)
            {
                if (pasien_arr[j].no_pendaftaran == no_reg)
                {
                    no_reg_exists = true;
                    cout << "No. Pendaftaran '" << no_reg << "' sudah digunakan! Masukkan No. yang lain." << endl;
                    break;
                }
            }
        } while (no_reg_exists);

        cin.ignore();
        pasien_arr[total_pasien].no_pendaftaran = no_reg;

        cout << "Nama Pasien              : ";
        getline(cin >> ws, pasien_arr[total_pasien].nama);
        cout << "Golongan Darah           : ";
        getline(cin >> ws, pasien_arr[total_pasien].gol_darah);

        total_pasien++;
        jumlah_input_berhasil++;
        cout << "Data pasien berhasil ditambahkan." << endl;
    }
}

void ListPasien(const Pasien pasien_arr[], int total_pasien)
{
    if (total_pasien == 0)
    {
        cout << "Belum ada data pasien yang tersimpan." << endl;
        return;
    }
    cout << "\n--- Daftar Semua Pasien ---\n";
    cout << "------------------------------------------------------------------\n";
    cout << left << setw(5) << "No." << setw(20) << "No. Pendaftaran" << setw(30) << "Nama Pasien" << "Gol. Darah" << endl;
    cout << "------------------------------------------------------------------\n";
    for (int i = 0; i < total_pasien; ++i)
    {
        cout << left << setw(5) << (i + 1);
        cout << setw(20) << pasien_arr[i].no_pendaftaran
             << setw(30) << pasien_arr[i].nama
             << pasien_arr[i].gol_darah << endl;
    }
    cout << "------------------------------------------------------------------\n";
}
bool DeleteDataPasien(Pasien pasien_arr[], int &total_pasien)
{
    string pasien_id;
    cout << "Pilih No Registrasi Pasien yang akan dihapus : ";
    cin >> pasien_id;
    bool found = false;
    int search_pasien = 0;
    while (search_pasien < total_pasien && !found)
    {
        if (pasien_arr[search_pasien].no_pendaftaran == pasien_id)
        {
            found = true;
        }
        else
        {
            search_pasien++;
        }
    }
    if (found)
    {
        for (int i = search_pasien; i < total_pasien - 1; i++)
        {
            pasien_arr[i] = pasien_arr[i + 1];
        }
        total_pasien--;
        return true;
    }
    else
    {
        return false;
    }
}
void SearchPasien(const Pasien pasien_arr[], int total_pasien, string search_no_pasien = "", int cari_pasien_idx = 0)
{
    if (total_pasien == 0)
    {
        cout << "Belum ada data pasien untuk dicari." << endl;
        return;
    }
    if (search_no_pasien == "" && cari_pasien_idx == 0)
    {
        cout << "Masukkan No. Pendaftaran Pasien yang dicari: ";
        cin >> search_no_pasien;
        cin.ignore();
    }
    if (cari_pasien_idx >= total_pasien)
    {
        cout << "\nData pasien dengan No. Pendaftaran '" << search_no_pasien << "' tidak ditemukan." << endl;
        return;
    }

    if (pasien_arr[cari_pasien_idx].no_pendaftaran == search_no_pasien)
    {
        cout << "\n--- Data Pasien Ditemukan ---\n";
        cout << "No. Pendaftaran  : " << pasien_arr[cari_pasien_idx].no_pendaftaran << endl;
        cout << "Nama Pasien      : " << pasien_arr[cari_pasien_idx].nama << endl;
        cout << "Golongan Darah   : " << pasien_arr[cari_pasien_idx].gol_darah << endl;
        cout << "---------------------------\n";
        return;
    }
    SearchPasien(pasien_arr, total_pasien, search_no_pasien, cari_pasien_idx + 1);
}

void SortPasienByNoPendaftaran(Pasien pasien_arr[], int total_pasien) // Selection Sort
{
    if (total_pasien < 2)
        return;
    for (int a = 0; a < total_pasien - 1; a++)
    {
        int k = a;
        for (int b = a + 1; b < total_pasien; b++)
        {
            if (pasien_arr[b].no_pendaftaran < pasien_arr[k].no_pendaftaran)
            {
                k = b;
            }
        }
        swap(pasien_arr[a], pasien_arr[k]);
    }
}

void SimpanDataPasienKeFile(const Pasien pasien_arr[], int total_pasien)
{
    ofstream file("data_pasien.csv");
    if (file.is_open())
    {
        for (int i = 0; i < total_pasien; i++)
        {
            if (!pasien_arr[i].no_pendaftaran.empty())
            {
                file << pasien_arr[i].no_pendaftaran << ","
                     << pasien_arr[i].nama << ","
                     << pasien_arr[i].gol_darah << endl;
            }
        }
        file.close();
    }
    else
    {
        cout << "ERROR: Gagal membuka file untuk menyimpan data pasien!" << endl;
    }
}

int LoadDataPasienDariFile(Pasien pasien_arr[])
{
    ifstream file("data_pasien.csv");
    string line;
    int count_pasien = 0;
    if (!file.is_open())
        return 0;

    while (count_pasien < MAX_DATA && getline(file, line))
    {
        stringstream ss(line);
        getline(ss, pasien_arr[count_pasien].no_pendaftaran, ',');
        getline(ss, pasien_arr[count_pasien].nama, ',');
        getline(ss, pasien_arr[count_pasien].gol_darah);
        if (!pasien_arr[count_pasien].no_pendaftaran.empty())
        {
            count_pasien++;
        }
    }
    file.close();
    return count_pasien;
}

// fungsi CekJamPraktikDokter
bool CekJamPraktikDokter(const string &id_dokter_check, const string &nama_hari_janji, const string &jam_janji_str, const Dokter dokter_arr[], int total_dokter)
{
    const Dokter *pDokter = nullptr;
    for (int i = 0; i < total_dokter; ++i)
    {
        if (dokter_arr[i].id_dokter == id_dokter_check)
        {
            pDokter = &dokter_arr[i];
            break;
        }
    }
    if (!pDokter)
    {
        cout << "Error internal: Data Dokter dengan ID '" << id_dokter_check << "' tidak ditemukan untuk cek jam praktik." << endl;
        return false;
    }

    for (int i = 0; i < pDokter->jumlah_jadwal_aktif; i++)
    {
        const JadwalPraktikHarian &jadwal_dokter_hari_ini = pDokter->daftar_jadwal[i];
        if (jadwal_dokter_hari_ini.aktif && jadwal_dokter_hari_ini.hari == nama_hari_janji)
        {
            Waktu waktu_janji_mulai(jam_janji_str);
            Waktu dokter_jam_mulai(jadwal_dokter_hari_ini.jam_mulai);
            Waktu dokter_jam_selesai(jadwal_dokter_hari_ini.jam_selesai);

            if (!waktu_janji_mulai.isValid() || !dokter_jam_mulai.isValid() || !dokter_jam_selesai.isValid())
            {
                cout << "Format waktu pada jadwal dokter atau janji tidak valid untuk hari " << nama_hari_janji << endl;
                return false;
            }

            int janji_mulai_total_menit = waktu_janji_mulai.totalMenit;
            int janji_selesai_total_menit = janji_mulai_total_menit + Durasi_Janjitemu;
            int dokter_mulai_total_menit = dokter_jam_mulai.totalMenit;
            int dokter_selesai_total_menit = dokter_jam_selesai.totalMenit;

            if (janji_mulai_total_menit >= dokter_mulai_total_menit && janji_selesai_total_menit <= dokter_selesai_total_menit)
            {
                return true;
            }
            else
            {
                Waktu waktu_janji_selesai_obj(janji_selesai_total_menit / 60, janji_selesai_total_menit % 60);
                cout << "\nJam janji temu (" << waktu_janji_mulai.waktuToString() << " - " << waktu_janji_selesai_obj.waktuToString()
                     << ") pada hari " << nama_hari_janji << " di luar jam praktik Dokter " << pDokter->nama_dokter
                     << " (" << dokter_jam_mulai.waktuToString() << " - " << dokter_jam_selesai.waktuToString() << ")." << endl;
                return false;
            }
        }
    }
    cout << "Dokter " << pDokter->nama_dokter << " tidak memiliki jadwal praktik pada hari " << nama_hari_janji << "." << endl;
    return false;
}

void MenuJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter)
{
    int pilih_janji;
    do
    {
        ClearScreen();
        LoadJanjiDariFile();
        TampilkanHeader("MENU JANJI TEMU (Total Janji: " + to_string(total_janji) + ")");
        cout << "| 1. Buat Janji Temu Baru                                     |\n";
        cout << "| 2. Lihat Janji Temu                                         |\n";
        cout << "| 0. Kembali ke Menu Utama                                    |\n";
        cout << "===============================================================\n";
        cout << "Pilih menu: ";
        cin >> pilih_janji;

        while (pilih_janji < 0 || pilih_janji > 2)
        {
            cout << "Input tidak valid. Masukkan angka menu (0-2): ";
            cin.clear();
            cin.ignore();
            cin >> pilih_janji;
        }

        switch (pilih_janji)
        {
        case 1:
            ClearScreen();
            TampilkanHeader("BUAT JANJI TEMU BARU");
            BuatJanjiTemu(pasien_arr, total_pasien, dokter_arr, total_dokter);
            SimpanJanjiKeFile();
            PauseSystem();
            break;
        case 2:
            ClearScreen();
            TampilkanHeader("LIHAT JANJI TEMU");
            LihatJanjiTemu(pasien_arr, total_pasien, dokter_arr, total_dokter);
            PauseSystem();
            break;
        case 0:
            return;
        }
    } while (true);
}

bool CekJadwalTabrakan(const string &id_dokter_check, const string &tanggal_check, const string &jam_check)
{

    Waktu waktu_baru(jam_check);
    if (!waktu_baru.isValid())
    {
        return true;
    }

    int start_baru = waktu_baru.totalMenit;
    int end_baru = start_baru + Durasi_Janjitemu;

    for (int i = 0; i < total_janji; ++i)
    {
        if (jadwal[i].id_dokter == id_dokter_check && jadwal[i].tanggal == tanggal_check)
        {
            Waktu waktu_lama(jadwal[i].jam);
            if (!waktu_lama.isValid())
                continue;

            int start_lama = waktu_lama.totalMenit;
            int end_lama = start_lama + Durasi_Janjitemu;

            if ((start_baru < end_lama) && (end_baru > start_lama))
            {
                return true;
            }
        }
    }
    return false;
}

// BuatJanjiTemu
void BuatJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter)
{
    if (total_janji >= MAX_DATA)
    {
        cout << "Kapasitas jadwal janji temu penuh!" << endl;
        return;
    }
    if (total_pasien == 0)
    {
        cout << "Belum ada data pasien. Silakan input data pasien terlebih dahulu." << endl;
        return;
    }
    if (total_dokter == 0)
    {
        cout << "Belum ada data dokter. Silakan input data dokter terlebih dahulu." << endl;
        return;
    }

    string no_pendaftaran_pasien, id_dokter_input, tanggal_input, nama_hari_janji_input, jam_input, keluhan_input;

    int idx_pasien = -1;
    bool found = false;
    cout << "--- Data Pasien Tersedia ---\n";
    ListPasien(pasien_arr, total_pasien);
    cout << "\nMasukkan No. Pendaftaran Pasien : ";
    getline(cin >> ws, no_pendaftaran_pasien);
    int p = 0;
    while (p < total_pasien && !found)
    {
        if (pasien_arr[p].no_pendaftaran == no_pendaftaran_pasien)
        {
            found = true;
        }
        else
        {
            p++;
        }
    }
    if (!found)
    {
        cout << "Pasien dengan No. Pendaftaran '" << no_pendaftaran_pasien << "' tidak ditemukan!" << endl;
        return;
    }
    found = false;
    int idx_dokter = -1;
    cout << "\n--- Data Dokter Tersedia ---\n";
    ListDokter(dokter_arr, total_dokter);
    cout << "\nMasukkan ID Dokter              : ";
    getline(cin >> ws, id_dokter_input);
    int d = 0;
    while (d < total_dokter && !found)
    {
        if (dokter_arr[d].id_dokter == id_dokter_input)
        {
            found = true;
        }
        else
        {
            d++;
        }
    }
    if (!found)
    {
        cout << "Dokter dengan ID '" << id_dokter_input << "' tidak ditemukan!" << endl;
        return;
    }
    cout << "Masukkan Nama Hari Janji Temu (Contoh: Senin, Selasa): ";
    getline(cin >> ws, nama_hari_janji_input);
    bool format_tanggal_check = false;
    do
    {
        cout << "Masukkan Tanggal (YYYY-MM-DD) : ";
        getline(cin >> ws, tanggal_input);

        if (tanggal_input.length() == 10 && tanggal_input[4] == '-' && tanggal_input[7] == '-' &&
            isdigit(tanggal_input[0]) && isdigit(tanggal_input[1]) && isdigit(tanggal_input[2]) && isdigit(tanggal_input[3]) &&
            isdigit(tanggal_input[5]) && isdigit(tanggal_input[6]) &&
            isdigit(tanggal_input[8]) && isdigit(tanggal_input[9]))
        {
            int tahun = stoi(tanggal_input.substr(0, 4));
            int bulan = stoi(tanggal_input.substr(5, 2));
            int hari = stoi(tanggal_input.substr(8, 2));

            if (bulan < 1 || bulan > 12)
            {
                cout << "Bulan tidak valid! Harus antara 1-12.\n";
                continue;
            }

            int max_hari;
            if (bulan == 2)
            {
                // Cek tahun kabisat untuk Februari
                if ((tahun % 400 == 0) || (tahun % 100 != 0 && tahun % 4 == 0))
                {
                    max_hari = 29;
                }
                else
                {
                    max_hari = 28;
                }
            }
            else if (bulan == 4 || bulan == 6 || bulan == 9 || bulan == 11)
            {
                max_hari = 30;
            }
            else
            {
                max_hari = 31;
            }

            if (hari < 1 || hari > max_hari)
            {
                cout << "Hari tidak valid untuk bulan tersebut! Maksimal hari: " << max_hari << "\n";
                continue;
            }
            format_tanggal_check = true;
        }
        else
        {
            cout << "Format tanggal tidak sesuai! Gunakan format YYYY-MM-DD.\n";
        }
    } while (!format_tanggal_check);

    cout << "Masukkan Jam (HH:MM 24 jam) : ";
    getline(cin >> ws, jam_input);
    Waktu cekWaktu(jam_input);
    if (!cekWaktu.isValid())
    {
        cout << "Format jam tidak valid (gunakan HH:MM). Pembuatan janji dibatalkan." << endl;
        return;
    }

    if (!CekJamPraktikDokter(id_dokter_input, nama_hari_janji_input, jam_input, dokter_arr, total_dokter))
    {
        return;
    }

    if (CekJadwalTabrakan(id_dokter_input, tanggal_input, jam_input))
    {
        cout << "\nJADWAL TABRAKAN!\nDokter " << dokter_arr[idx_dokter].nama_dokter
             << " sudah memiliki janji temu lain pada atau dekat dengan "
             << tanggal_input << " jam " << jam_input << "." << endl;
        cout << "Silakan pilih waktu atau dokter lain." << endl;
        return;
    }
    cout << "Masukkan Keluhan Pasien       : ";
    getline(cin >> ws, keluhan_input);
    jadwal[total_janji].no_pendaftaran_pasien = no_pendaftaran_pasien;
    jadwal[total_janji].id_dokter = id_dokter_input;
    jadwal[total_janji].tanggal = tanggal_input;
    jadwal[total_janji].jam = jam_input;
    jadwal[total_janji].keluhan = keluhan_input;
    total_janji++;
    cout << "\nJanji temu berhasil dibuat!" << endl;
}

void LihatJanjiTemu(const Pasien pasien_arr[], int total_pasien, const Dokter dokter_arr[], int total_dokter)
{
    if (total_janji == 0)
    {
        cout << "Belum ada janji temu yang terjadwal." << endl;
        return;
    }
    cout << "\n--- Daftar Semua Janji Temu (Durasi: " << Durasi_Janjitemu << " menit) ---\n";
    cout << "----------------------------------------------------------------------------------------------------------------------\n";
    cout << left
         << setw(5) << "No." << setw(18) << "No.Reg Pasien" << setw(25) << "Nama Pasien"
         << setw(12) << "ID Dokter" << setw(25) << "Nama Dokter"
         << setw(12) << "Tanggal" << setw(8) << "Jam" << "Keluhan" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < total_janji; i++)
    {
        string nama_pasien_janji = "Pasien tidak ditemukan";
        string nama_dokter_janji = "Dokter tidak ditemukan";

        for (int j = 0; j < total_pasien; ++j)
        {
            if (pasien_arr[j].no_pendaftaran == jadwal[i].no_pendaftaran_pasien)
            {
                nama_pasien_janji = pasien_arr[j].nama;
                break;
            }
        }
        for (int k = 0; k < total_dokter; ++k)
        {
            if (dokter_arr[k].id_dokter == jadwal[i].id_dokter)
            {
                nama_dokter_janji = dokter_arr[k].nama_dokter;
                break;
            }
        }

        cout << left
             << setw(5) << (i + 1)
             << setw(18) << jadwal[i].no_pendaftaran_pasien
             << setw(25) << nama_pasien_janji
             << setw(12) << jadwal[i].id_dokter
             << setw(25) << nama_dokter_janji
             << setw(12) << jadwal[i].tanggal
             << setw(8) << jadwal[i].jam
             << jadwal[i].keluhan << endl;
    }
    cout << "----------------------------------------------------------------------------------------------------------------------\n";
}

void SimpanJanjiKeFile()
{
    ofstream file("jadwal.csv");
    if (file.is_open())
    {
        for (int i = 0; i < total_janji; i++)
        {
            if (!jadwal[i].no_pendaftaran_pasien.empty())
            {
                file << jadwal[i].no_pendaftaran_pasien << ","
                     << jadwal[i].id_dokter << ","
                     << jadwal[i].tanggal << ","
                     << jadwal[i].jam << ","
                     << jadwal[i].keluhan << endl;
            }
        }
        file.close();
    }
    else
    {
        cout << "ERROR: Gagal membuka file untuk menyimpan jadwal janji temu!" << endl;
    }
}

void LoadJanjiDariFile()
{
    ifstream file("jadwal.csv");
    string line;
    total_janji = 0;
    if (!file.is_open())
        return;

    while (total_janji < MAX_DATA && getline(file, line))
    {
        stringstream ss(line);
        getline(ss, jadwal[total_janji].no_pendaftaran_pasien, ',');
        getline(ss, jadwal[total_janji].id_dokter, ',');
        getline(ss, jadwal[total_janji].tanggal, ',');
        getline(ss, jadwal[total_janji].jam, ',');
        getline(ss, jadwal[total_janji].keluhan);
        if (!jadwal[total_janji].no_pendaftaran_pasien.empty())
        {
            total_janji++;
        }
    }
    file.close();
}