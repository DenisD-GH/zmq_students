#include <zmq.hpp>   // Нужно установить libzmq-dev и cppzmq
#include <iostream>
#include <chrono>    // Для работы со временем
#include <thread>    // Для работы с потоками
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Данные о студенте
struct Student {
    int id;                 // ИД студента
    std::string firstName;  // Имя студента
    std::string lastName;   // Фамилия студента
    std::string birthDate;  // Дата рождения
};

// Функция для чтения студентов из файла
std::vector<Student> readStudentsFromFile(const std::string& filename) {
    std::vector<Student> students;  // Пустой вектор для студентов
    std::ifstream file(filename);   // Открываем файл для чтения
    
    // Проверка успешности открытия
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return students;  // Пустой вектор
    }

    std::string line;  // Строка из файла
    // Чтение файла построчно
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Student s;
        
        // Парсинг строки
        if (iss >> s.id >> s.firstName >> s.lastName >> s.birthDate) {
            students.push_back(s);  // Добавление студента
        } else {
            std::cerr << "Ошибка парсинга строки: " << line << std::endl;
        }
    }
    
    return students;
}

int main() {
    std::vector<Student> students = readStudentsFromFile("../data/student_file_1.txt");
    
    std::cout << "Прочитано студентов: " << students.size() << std::endl;
    
    // Данные студентов для проверки
    std::cout << "\nСписок студентов:\n";
    for (const auto& student : students) {
        std::cout << "ID: " << student.id 
                  << ", Имя: " << student.firstName
                  << ", Фамилия: " << student.lastName
                  << ", Дата рождения: " << student.birthDate
                  << std::endl;
    }

    /*
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUB);
    socket.bind("tcp://*:5555");
    std::cout << "Сервер запущен на порту 5555\n";
    ...
    */
    
    return 0;
}