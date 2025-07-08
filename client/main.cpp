#include <zmq.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

// Функция для логгирования
void log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&now_time);
    
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << message << std::endl;
}

struct Student {
    int id;
    std::string firstName;
    std::string lastName;
    std::string birthDate;
};

bool compareStudents(const Student& a, const Student& b) {
    if (a.lastName != b.lastName) 
        return a.lastName < b.lastName;

    if (a.firstName != b.firstName) 
        return a.firstName < b.firstName;
    
    return a.id < b.id;
}

int main() {
    log("Клиент запущен");
    
    zmq::context_t context(1);  // Создание контекста ZeroMQ
    zmq::socket_t socket(context, ZMQ_SUB);  // Создание SUB-сокета
    
    try {
        // Подключение к серверу
        socket.connect("tcp://localhost:5555");
        // Подписка на все сообщения
        socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        log("Подключено к tcp://localhost:5555");
    } catch (const zmq::error_t& e) {
        log("ОШИБКА подключения: " + std::string(e.what()));
        return 1;
    }

    // Ожидание сообщений
    log("Ожидание данных...");
    zmq::message_t reply;  // Объект для хранения сообщения
    
    try {
        socket.recv(reply, zmq::recv_flags::none);
        log("Данные получены (" + std::to_string(reply.size()) + " байт)");
    } catch (const zmq::error_t& e) {
        log("ОШИБКА получения данных: " + std::string(e.what()));
        return 1;
    }

    // Преобразование сообщения в строку
    std::string receivedData(static_cast<char*>(reply.data()), reply.size());
    std::vector<Student> students;
    std::istringstream dataStream(receivedData);
    std::string line;  // Переменная для хранения строки
    int line_num = 0;  // Счетчик строк для отладки
    
    while (std::getline(dataStream, line)) {
        line_num++;
        std::istringstream lineStream(line);
        Student s;
        char delimiter;  // Разделитель полей
        
        // Разбор строки: ID, Имя, Фамилия, Дата рождения
        if (lineStream >> s.id >> delimiter &&
            std::getline(lineStream, s.firstName, '|') &&
            std::getline(lineStream, s.lastName, '|') &&
            std::getline(lineStream, s.birthDate)) {
            
            // Добавление студента в вектор
            students.push_back(s);
        } else {
            log("ОШИБКА: Неверный формат строки #" + std::to_string(line_num) + ": " + line);
        }
    }

    log("Десериализовано записей: " + std::to_string(students.size()));

    // Сортировка студентов по ФИО
    std::sort(students.begin(), students.end(), compareStudents);
    
    log("Отсортированный список студентов:");
    // Перебор и вывод всех студентов
    for (const auto& student : students) {
        log("ID: " + std::to_string(student.id) + 
            ", Имя: " + student.firstName +
            ", Фамилия: " + student.lastName +
            ", Дата рождения: " + student.birthDate);
    }
    
    log("Клиент завершает работу");
    return 0;
}