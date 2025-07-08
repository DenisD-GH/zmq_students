#include <zmq.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

struct Student {
    int id;
    std::string firstName;
    std::string lastName;
    std::string birthDate;
};

// Функция для сравнения студентов
bool compareStudents(const Student& a, const Student& b) {
    if (a.lastName != b.lastName) 
        return a.lastName < b.lastName;
    
    if (a.firstName != b.firstName) 
        return a.firstName < b.firstName;
    
    return a.id < b.id; // Добавлено недостающее возвращаемое значение
}

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
    
    try {
        socket.connect("tcp://localhost:5555");
        socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        std::cout << "Клиент подключен к серверу, ожидание данных...\n";
    } catch (const zmq::error_t& e) {
        std::cerr << "Ошибка подключения: " << e.what() << std::endl;
        return 1;
    }

    // Ожидание сообщения
    zmq::message_t reply;
    try {
        socket.recv(reply, zmq::recv_flags::none);
        std::cout << "Данные получены успешно\n";
    } catch (const zmq::error_t& e) {
        std::cerr << "Ошибка получения данных: " << e.what() << std::endl;
        return 1;
    }

    // Десериализация данных
    std::string receivedData(static_cast<char*>(reply.data()), reply.size());
    std::vector<Student> students;
    std::istringstream dataStream(receivedData);
    std::string line;
    
    while (std::getline(dataStream, line)) {
        std::istringstream lineStream(line);
        Student s;
        char delimiter;
        
        if (lineStream >> s.id >> delimiter &&
            std::getline(lineStream, s.firstName, '|') &&
            std::getline(lineStream, s.lastName, '|') &&
            std::getline(lineStream, s.birthDate)) {
            
            students.push_back(s);
        } else {
            std::cerr << "Ошибка разбора строки: " << line << std::endl;
        }
    }

    // Сортировка студентов
    std::sort(students.begin(), students.end(), compareStudents);
    
    // Вывод результатов
    std::cout << "\nОтсортированный список студентов:\n";
    for (const auto& student : students) {
        std::cout << "ID: " << student.id 
                  << ", Имя: " << student.firstName
                  << ", Фамилия: " << student.lastName
                  << ", Дата рождения: " << student.birthDate
                  << std::endl;
    }
    
    return 0;
}