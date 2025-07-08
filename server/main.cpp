#include <zmq.hpp>   // Нужно установить libzmq-dev и cppzmq
#include <iostream>
#include <chrono>    // Для работы со временем
#include <thread>    // Для работы с потоками
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set> // Для хранения уникальных студентов

// Данные о студенте
struct Student {
    int id;                 // ИД студента
    std::string firstName;  // Имя студента
    std::string lastName;   // Фамилия студента
    std::string birthDate;  // Дата рождения
    
    // Оператор сравнения для определения дубликатов
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
            // Комбинирование строк для создания уникального хеша
            return hash<string>()(s.firstName + "|" + s.lastName + "|" + s.birthDate);
        }
    };
}

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
    // Чтение первого файла со студентами
    std::vector<Student> students1 = readStudentsFromFile("../data/student_file_1.txt");
    std::cout << "Прочитано студентов из файла 1: " << students1.size() << std::endl;
    
    // Чтение второго файла со студентами
    std::vector<Student> students2 = readStudentsFromFile("../data/student_file_2.txt");
    std::cout << "Прочитано студентов из файла 2: " << students2.size() << std::endl;

    // Объединение двух списков студентов
    std::vector<Student> allStudents;
    allStudents.reserve(students1.size() + students2.size()); // Оптимизация памяти
    allStudents.insert(allStudents.end(), students1.begin(), students1.end());
    allStudents.insert(allStudents.end(), students2.begin(), students2.end());
    
    std::cout << "Всего студентов до удаления дубликатов: " << allStudents.size() << std::endl;
    
    // Удаление дубликатов с помощью unordered_set
    std::unordered_set<Student> uniqueSet(allStudents.begin(), allStudents.end());
    
    // Преобразование обратно в вектор для удобства
    std::vector<Student> uniqueStudents(uniqueSet.begin(), uniqueSet.end());
    std::cout << "Уникальных студентов после удаления дубликатов: " << uniqueStudents.size() << std::endl;
    
    // Вывод уникальных студентов для проверки
    std::cout << "\nУникальные студенты:\n";
    for (const auto& student : uniqueStudents) {
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