# Proyek Akhir C++: Pemantauan Denyut Jantung Wearable

Simulasi sistem pemantauan denyut jantung wearable (client-server) menggunakan C++. Client mengirim data denyut jantung, server menganalisis anomali (bradikardia/takikardia), dan menyimpan data.

## Fitur Utama
- Client-server (TCP/IP, Winsock di Windows).
- Multithreading di server untuk menangani banyak client.
- Deteksi anomali dan perhitungan deviasi.
- Penyimpanan data mentah ke data_log.bin.
- Penyimpanan anomali terurut ke anomaly_log.json (menggunakan nlohmann/json).
- Sinkronisasi akses file dengan std::mutex.

## Syarat
1.  *Compiler C++17* (MinGW-w64 GCC/G++ atau MSVC untuk Windows; GCC/Clang untuk Linux/macOS).
2.  *CMake* (versi 3.10+).

## Instruksi Build (Menggunakan CMake)

1.  *Buka Terminal/Command Prompt.*
2.  *Navigate ke Direktori Root Proyek.*
3.  *Buat dan Masuk ke Direktori Build:*
    bash
    mkdir build
    cd build
    
4.  **Konfigurasi Proyek (dari dalam folder build):**
    *   Windows (MinGW): cmake -G "MinGW Makefiles" ..
    *   Windows (Visual Studio, dari Developer Command Prompt): cmake ..
    *   Linux/macOS: cmake ..
5.  **Build Proyek (dari dalam folder build):**
    bash
    cmake --build .
    
    Executable (server.exe/server dan client.exe/client) akan ada di folder build.

## Cara Menjalankan

1.  *Jalankan Server Dahulu:*
    *   Dari dalam folder build, jalankan:
        *   Windows: .\server.exe
        *   Linux/macOS: ./server
    *   Server akan listening di port 8888

2.  *Jalankan Client (di terminal baru):*
    *   Dari dalam folder build, jalankan:
        *   Windows: .\client.exe
        *   Linux/macOS: ./client
    *   Ulangi untuk beberapa client jika diinginkan.

3.  *Output:*
    *   Log anomali akan disimpan di anomaly_log.json (di direktori tempat server dijalankan).
    *   Log data mentah disimpan di data_log.bin.

4.  *Menghentikan:* Tekan Ctrl+C di terminal masing-masing.