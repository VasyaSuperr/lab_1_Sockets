#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <locale>

#pragma comment(lib, "ws2_32.lib")

void printError() {
    int err = WSAGetLastError();

    std::cout << "[ПОМИЛКА] Код: " << err << " | Опис: ";

    switch (err) {
    case WSANOTINITIALISED: std::cout << "WinSock не ініціалізовано"; break;
    case WSAENETDOWN: std::cout << "Збій мережі"; break;
    case WSAEADDRINUSE: std::cout << "Адреса вже використовується"; break;
    case WSAEFAULT: std::cout << "Некоректний параметр (namelen)"; break;
    case WSAEINPROGRESS: std::cout << "Виконується блокуюча операція"; break;
    case WSAEAFNOSUPPORT: std::cout << "Несумісне сімейство адрес"; break;
    case WSAEINVAL: std::cout << "Сокет уже прив'язаний"; break;
    case WSAENOBUFS: std::cout << "Недостатньо ресурсів"; break;
    case WSAENOTSOCK: std::cout << "Дескриптор не є сокетом"; break;
    case WSAEADDRNOTAVAIL: std::cout << "Адреса недоступна"; break;
    default: std::cout << "Невідома помилка";
    }

    std::cout << std::endl;
}

int main() {
    // Консоль
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Ukrainian");

    std::cout << "===== ЧАСТИНА 1: WinSock =====\n";

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cout << "Помилка WSAStartup: " << result << std::endl;
        return 1;
    }

    int major = LOBYTE(wsaData.wVersion);
    int minor = HIBYTE(wsaData.wVersion);

    std::cout << "Версія: " << major << "." << minor << std::endl;
    std::cout << "Опис: " << wsaData.szDescription << std::endl;

    int maxMajor = LOBYTE(wsaData.wHighVersion);
    int maxMinor = HIBYTE(wsaData.wHighVersion);

    std::cout << "Макс версія: " << maxMajor << "." << maxMinor << std::endl;
    std::cout << "Стан: WinSock готовий\n";

    std::cout << "\n===== ЧАСТИНА 2: СОКЕТИ =====\n";

    // Сокети
    SOCKET s1 = socket(AF_INET, SOCK_DGRAM, 0);
    SOCKET s2 = socket(AF_INET, SOCK_DGRAM, 0);
    SOCKET s3 = socket(AF_INET, SOCK_STREAM, 0);
    SOCKET s4 = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET; // IPv4

    // 1. Broadcast
    std::cout << "\n[Сокет 1: Broadcast]\n";
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_BROADCAST; // широкомовна адреса

    if (bind(s1, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        printError();

    // 2. НЕІСНУЮЧА IP → 10049 //inet_addr("127.0.0.1");
    std::cout << "\n[Сокет 2: 10.1.2.156]\n";
    addr.sin_port = htons(5001);
    addr.sin_addr.s_addr = inet_addr("10.1.2.156");

    if (bind(s2, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        printError();

    // 3. НЕІСНУЮЧА IP → 10049 //inet_addr("127.0.0.1");
    std::cout << "\n[Сокет 3: 10.1.2.157]\n";
    addr.sin_port = htons(5002);
    addr.sin_addr.s_addr = inet_addr("10.1.2.157");

    if (bind(s3, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        printError();

    // 4. ANY (успішний)
    std::cout << "\n[Сокет 4: ANY]\n";
    addr.sin_port = htons(5003); // порт
    addr.sin_addr.s_addr = INADDR_ANY; // локальна IP адреса

    if (bind(s4, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        printError();
    else
        std::cout << "Прив'язка успішна\n";

    // ПОМИЛКИ
    std::cout << "\n===== МОДЕЛЮВАННЯ ПОМИЛОК =====\n";

    // 1. WSAEINVAL
    std::cout << "\n(WSAEINVAL)\n";
    bind(s4, (sockaddr*)&addr, sizeof(addr));
    printError();

    // 2. WSAENOTSOCK
    std::cout << "\n(WSAENOTSOCK)\n";
    bind(-1, (sockaddr*)&addr, sizeof(addr));
    printError();

    // 3. WSAEFAULT
    std::cout << "\n(WSAEFAULT)\n";
    bind(s1, (sockaddr*)&addr, -1);
    printError();

    // 4. WSAEADDRINUSE
    std::cout << "\n(WSAEADDRINUSE)\n";
    SOCKET temp = socket(AF_INET, SOCK_STREAM, 0);
    bind(temp, (sockaddr*)&addr, sizeof(addr));
    bind(temp, (sockaddr*)&addr, sizeof(addr));
    printError();

    // 5. WSANOTINITIALISED
    std::cout << "\n(WSANOTINITIALISED)\n";
    WSACleanup();
    bind(s1, (sockaddr*)&addr, sizeof(addr));
    printError();

    // Закриття
    closesocket(s1);
    closesocket(s2);
    closesocket(s3);
    closesocket(s4);

    std::cout << "\nЗавершення роботи.\n";

    return 0;
}