#include <zmq.hpp>   // Нужно установить libzmq-dev и cppzmq
#include <iostream>
#include <chrono>    // Для работы со временем
#include <thread>    // Для работы с потоками

int main() {
    // Создание контекста ZeroMQ
    zmq::context_t context(1);
    
    // Создание сокета PUB
    zmq::socket_t socket(context, ZMQ_PUB);
    
    // Слушает порт для подключений
    socket.bind("tcp://*:5555");
    std::cout << "Сервер запущен на порту 5555\n";

    // Время клиентам подключиться перед отправкой
    std::cout << "Ожидание подключения клиентов...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Подготовка и отправка сообщения
    std::string message = "Hello from ZMQ Server!";
    zmq::message_t zmq_message(message.size()); // Контейнер, без которого не получится отправить данные
    memcpy(zmq_message.data(), message.data(), message.size());
    
    // Отправка сообщения
    socket.send(zmq_message, zmq::send_flags::none);
    std::cout << "Сообщение отправлено: " << message << "\n";
    
    // 3 секунды чтобы клиент успел получить
    std::cout << "Ожидание получения клиентом...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Сервер завершает работу.\n";
    
    return 0;
}