#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

void Log(const std::string&& message)
{
    std::ofstream logfile;
    
    // Открываем файл для записи логов.
    // Если файл существует, он будет очищен перед записью.
    logfile.open("/var/log/cpp_daemon.log");

    // Записываем текущее время в лог-файл
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    char time_buf[100]{};

    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now_c));

    std::cout << time_buf << ": " << message.c_str() << std::endl;
    logfile << time_buf << ": " << message.c_str() << std::endl;
    logfile.flush(); // Сбрасываем буфер, чтобы запись происходила сразу
}

int main()
{
    int server_fd{0};
    int optVal {1};

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        Log("Error: socket failed");
        return 1;
    }

    // Привязка к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optVal, sizeof(optVal)))
    {
        Log("Error: setsockopt");
        return 1;
    }

    int port {8080};

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        Log("Error: bind failed");
        return 1;
    }

    // Прослушивание порта
    if (listen(server_fd, 3) < 0)
    {
        Log("Error: listen");
        return 1;
    }

    Log("Сервер запущен на порту " + std::to_string(port));

    int newSocket{0};
    int addressSize = sizeof(address);

    while (true)
    {
        if ((newSocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addressSize)) < 0)
        {
            Log("Error: accept");
            continue;
        }

        // Обработка запроса (упрощенно)
        const int bufferSize {1024};
        char buffer[bufferSize]{};
        read(newSocket, buffer, bufferSize);
        Log("Получено сообщение: " + std::string(buffer));

        // Отправка ответа
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><a><img itemprop=\"image\" src=\"https://4kwallpapers.com/images/walls/thumbs_3t/19855.jpg\" height=\"720\" width=\"1280\"/></a></body></html>";
        send(newSocket, response.c_str(), response.length(), 0);
        close(newSocket);
    }

    return 0;
}