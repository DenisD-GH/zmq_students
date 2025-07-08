#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // Создание контекста ZeroMQ
    zmq::context_t context(1);
    
    // Создание сокета SUB
    zmq::socket_t socket(context, ZMQ_SUB);
    
    // Соединение с сервером
    socket.connect("tcp://localhost:5555");
    
    // Подписка на все сообщения от сервера
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    std::cout << "Клиент подключен к серверу, ожидание данных...\n";

    // Ожидание сообщений
    zmq::pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
    zmq::poll(items, 1, 3000); // Задержка 3 секунды (в миллисекундах)

    // Проверка наличия сообщения
    if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t reply;
        socket.recv(reply, zmq::recv_flags::none); // none - блокирующее получение
        
        // Преобразование и вывод сообщения
        std::string message = std::string(static_cast<char*>(reply.data()), reply.size());
        std::cout << "Получено сообщение: " << message << "\n";
    } else {
        std::cout << "Таймаут: сообщение не получено\n";
    }
    
    return 0;
}