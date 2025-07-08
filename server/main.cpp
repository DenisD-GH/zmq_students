#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>

struct Student {
    int id;
    std::string firstName;
    std::string lastName;
    std::string birthDate;
    
    bool operator==(const Student& other) const {
        return firstName == other.firstName &&
               lastName == other.lastName &&
               birthDate == other.birthDate;
    }
};

namespace std {
    template<>
    struct hash<Student> {
        size_t operator()(const Student& s) const {
            return hash<string>()(s.firstName + "|" + s.lastName + "|" + s.birthDate);
        }
    };
}

std::vector<Student> readStudentsFromFile(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return students;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Student s;
        
        if (iss >> s.id >> s.firstName >> s.lastName >> s.birthDate) {
            students.push_back(s);
        } else {
            std::cerr << "Ошибка парсинга строки: " << line << std::endl;
        }
    }
    
    return students;
}

int main() {
    // Чтение файлов
    std::vector<Student> students1 = readStudentsFromFile("../data/student_file_1.txt");
    std::cout << "Прочитано студентов из файла 1: " << students1.size() << std::endl;
    
    std::vector<Student> students2 = readStudentsFromFile("../data/student_file_2.txt");
    std::cout << "Прочитано студентов из файла 2: " << students2.size() << std::endl;

    // Объединение списков
    std::vector<Student> allStudents;
    allStudents.reserve(students1.size() + students2.size());
    allStudents.insert(allStudents.end(), students1.begin(), students1.end());
    allStudents.insert(allStudents.end(), students2.begin(), students2.end());
    
    std::cout << "Всего студентов до удаления дубликатов: " << allStudents.size() << std::endl;
    
    // Удаление дубликатов
    std::unordered_set<Student> uniqueSet(allStudents.begin(), allStudents.end());
    std::vector<Student> uniqueStudents(uniqueSet.begin(), uniqueSet.end());
    std::cout << "Уникальных студентов после удаления дубликатов: " << uniqueStudents.size() << std::endl;
    
    // Вывод студентов
    std::cout << "\nУникальные студенты:\n";
    for (const auto& student : uniqueStudents) {
        std::cout << "ID: " << student.id 
                  << ", Имя: " << student.firstName
                  << ", Фамилия: " << student.lastName
                  << ", Дата рождения: " << student.birthDate
                  << std::endl;
    }

    // Настройка ZeroMQ
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUB);
    socket.bind("tcp://*:5555");
    std::cout << "Сервер запущен на порту 5555\n";

    // Даем время клиентам подключиться
    std::cout << "Ожидание подключения клиентов...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Сериализация данных
    std::string serializedData;
    for (const auto& student : uniqueStudents) {
        serializedData += std::to_string(student.id) + "|" +
                          student.firstName + "|" +
                          student.lastName + "|" +
                          student.birthDate + "\n";
    }

    // Отправка данных
    zmq::message_t message(serializedData.size());
    memcpy(message.data(), serializedData.data(), serializedData.size());
    socket.send(message, zmq::send_flags::none);
    std::cout << "Данные студентов отправлены\n";

    // Ожидание перед завершением
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    return 0;
}