#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "common.h"
#include <cstdlib>
#include <sys/mman.h>
#include <semaphore.h>

int createMappedFile(const char* filename) {
    // Создаём файл (перезаписываем)
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Error creating mapped file");
        exit(1);
    }

    // Устанавливаем размер файла
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("Error setting file size");
        close(fd);
        exit(1);
    }

    return fd;
}

int main() {
    int fd1 = createMappedFile(MAPPED_FILE1);

    SharedData* shared1 = (SharedData*)mmap(nullptr, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, fd1, 0);
    if (shared1 == MAP_FAILED) {
        std::cerr << "Error mapping files" << std::endl;
        exit(1);
    }

    shared1->size = 0;
    shared1->done = false;

    // Первой строчкой пользователь в консоль родительского процесса вводит имя файла,
    // которое будет использовано для открытия файла с таким именем на чтение
    std::string name;
    std::getline(std::cin, name);
    const int file = open(name.c_str(), O_RDONLY);

    // Родительский процесс создает дочерний процесс.
    const pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "ERROR: Error while creating child process" << std::endl;
        munmap(shared1, sizeof(SharedData));
        close(fd1);
        return 1;
    }

    // Дочерний процесс
    if (pid == 0) {
        // Стандартный поток ввода дочернего процесса переопределяется открытым файлом.
        dup2(file, STDIN_FILENO);

        // Родительский и дочерний процесс должны быть представлены разными программами.
        execl("./child", "./child", nullptr);

        std::cerr << "ERROR: Can't execute child process" << std::endl;
        return 1;
    }

    // Родительский процесс ждёт завершение дочернего и выводит содержимое общей памяти в свой стандартный поток вывода.
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "Can't get status of child process" << std::endl;
        return 1;
    }

    std::string_view output(shared1->data);
    std::cout << output << std::endl;

    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
        std::cout << "ERROR: Division by zero" << std::endl;
        munmap(shared1, sizeof(SharedData));
        close(fd1);
        return 1;
    }

    munmap(shared1, sizeof(SharedData));
    close(fd1);
    return 0;
}
