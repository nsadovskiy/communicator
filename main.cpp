/**
 *
 * Компиляция: 
 *    g++ -std=c++0x -O2 -Wall main.cpp -I/home/sns/include/boost-1_53/ -L/home/sns/lib/ -lboost_system-gcc47-mt-sd-1_53 -o bin/main
 **/
#include <locale>
#include <iostream>
#include <boost/asio.hpp>

using std::wcerr;
using std::wcout;

int main(int argc, const char * argv[]) {

    ::setlocale(LC_ALL, "");

#if defined(BOOST_ASIO_HAS_EPOLL)
    wcout << L"Нормальный линукс, используется Epoll.\n";
#elif defined(BOOST_ASIO_HAS_IOCP)
    wcout << L"Поганая винда, используется IoCompletionPort.\n";
#elif defined(BOOST_ASIO_HAS_KQUEUE)
    wcout << L"Недобитая FreeBSD или гейская яблоось, используется kqueue.\n";
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
    wcout << L"Соляра, используется /dev/poll.\n";
#else
    wcout << L"Где я ?!?\n";
#endif

    if (argc < 4) {
        wcerr << L"Использование: communicator <адрес> <порт> <число_потоков>\n";
        wcerr << L"  <адрес>         - адрес привязки прослушивателя\n";
        wcerr << L"  <порт>          - порт прослушивателя\n";
        wcerr << L"  <число_потоков> - количество потоков, обрабатывающих запросы\n";
        wcerr << L"\n";
        wcerr << L"Пример:\n";
        wcerr << L"  communicator 0.0.0.0 4010 4\n";
        exit(1);
    }
}
