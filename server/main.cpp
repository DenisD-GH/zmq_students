#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>    // Подключение контейнера для хранения уникальных студентов
#include <ctime>            // Подключение библиотеки для работы со временем
#include <iomanip>          // Подключение манипуляторов ввода/вывода

// Функция для логгирования с временной меткой
void log(const std::string& message) {
    // Получение текущего времени
    auto now = std::chrono::system_clock::now();
    // Преобразование времени в формат time_t
    auto now_time = std::chrono::system_clock::to_time_t(now);
    // Преобразование в локальное время
    auto tm = *std::localtime(&now_time);
    
    // Вывод времени и сообщения
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << message << std::endl;
}

// Структура для хранения данных о студенте
struct Student {
    int id;                 // ИД студента
    std::string firstName;  // Имя студента
    std::string lastName;   // Фамилия студента
    std::string birthDate;  // Дата рождения студента
    
    // Оператор сравнения для определения дубликатов
    bool operator==(const Student& other) const {
        return firstName == other.firstName &&
               lastName == other.lastName &&
               birthDate == other.birthDate;
    }
};

// Хеш-функции для структуры Student
namespace std {
    template<>
    struct hash<Student> {
        size_t operator()(const Student& s) const {
            return hash<string>()(s.firstName + "|" + s.lastName + "|" + s.birthDate);
        }
    };
}

// Функция для чтения студентов из файла
std::vector<Student> readStudentsFromFile(const std::string& filename) {
    // Создание вектора для хранения студентов
    std::vector<Student> students;
    // Открытие файла для чтения
    std::ifstream file(filename);
    
    // Проверка успешности открытия файла
    if (!file.is_open()) {
        // Логирование ошибки открытия файла
        log("ОШИБКА: Не удалось открыть файл: " + filename);
        return students;  // Возврат пустого вектора
    }

    std::string line;  // Переменная для хранения строки из файла
    int line_num = 0;  // Счетчик строк
    
    // Чтение файла построчно
    while (std::getline(file, line)) {
        line_num++;
        std::istringstream iss(line);
        Student s;
        
        // Парсинг строки: ID, Имя, Фамилия, Дата рождения
        if (iss >> s.id >> s.firstName >> s.lastName >> s.birthDate) {
            // Добавление студента в вектор
            students.push_back(s);
        } else {
            // Логирование ошибки формата строки
            log("ОШИБКА: Неверный формат строки #" + std::to_string(line_num) + ": " + line);
        }
    }
    
    return students;
}

int main() {
    log("Сервер запущен");
    
    const std::string file1 = "../data/student_file_1.txt";
    const std::string file2 = "../data/student_file_2.txt";
    
    // Чтение первого файла
    log("Чтение файла: " + file1);
    std::vector<Student> students1 = readStudentsFromFile(file1);
    log("Прочитано студентов: " + std::to_string(students1.size()));
    
    // Чтение второго файла
    log("Чтение файла: " + file2);
    std::vector<Student> students2 = readStudentsFromFile(file2);
    log("Прочитано студентов: " + std::to_string(students2.size()));

    // Объединение списков студентов
    std::vector<Student> allStudents;
    allStudents.reserve(students1.size() + students2.size());
    allStudents.insert(allStudents.end(), students1.begin(), students1.end());
    allStudents.insert(allStudents.end(), students2.begin(), students2.end());
    
    log("Всего студентов до удаления дубликатов: " + std::to_string(allStudents.size()));
    
    // Удаление дубликатов с помощью unordered_set
    std::unordered_set<Student> uniqueSet(allStudents.begin(), allStudents.end());

    // Преобразование обратно в вектор
    std::vector<Student> uniqueStudents(uniqueSet.begin(), uniqueSet.end());
    log("Уникальных студентов: " + std::to_string(uniqueStudents.size()));

    zmq::context_t context(1);  // Создание контекста ZeroMQ
    zmq::socket_t socket(context, ZMQ_PUB);  // Создание PUB-сокета
    
    try {
        // Привязка к порту 5555
        socket.bind("tcp://*:5555");
        log("Прослушивание на порту 5555");
    } catch (const zmq::error_t& e) {
        // Обработка ошибки привязки сокета
        log("ОШИБКА привязки сокета: " + std::string(e.what()));
        return 1;
    }

    // Ожидание подключения клиентов
    log("Ожидание подключения клиентов...");

    // Пауза 3 секунды для подключения клиентов
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Преобразование данных
    std::string serializedData;
    for (const auto& student : uniqueStudents) {
        serializedData += std::to_string(student.id) + "|" +
                          student.firstName + "|" +
                          student.lastName + "|" +
                          student.birthDate + "\n";
    }

    // Отправка данных
    try {
        // Создание сообщения нужного размера
        zmq::message_t message(serializedData.size());
        // Копирование данных в сообщение
        memcpy(message.data(), serializedData.data(), serializedData.size());
        // Отправка сообщения
        socket.send(message, zmq::send_flags::none);
        log("Данные отправлены (" + std::to_string(serializedData.size()) + " байт)");
    } catch (const zmq::error_t& e) {
        log("ОШИБКА отправки данных: " + std::string(e.what()));
    }

    // Пауза перед завершением
    std::this_thread::sleep_for(std::chrono::seconds(1));
    log("Сервер завершает работу");
    
    return 0;
}