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
        perror("Error mapping files");
        exit(1);
    }

    shared1->size = 0;
    shared1->done = false;

    sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_CREAT | O_EXCL, 0666, 0);
    if (semaphore == SEM_FAILED)
    {
        std::cerr << "Ошибка открытия семафора" << std::endl;
        munmap(shared1, sizeof(SharedData));
        close(fd1);
        exit(1);
    }

    // Первой строчкой пользователь в консоль родительского процесса вводит имя файла,
    // которое будет использовано для открытия файла с таким именем на чтение
    std::string name;
    std::getline(std::cin, name);
    const int file = open(name.c_str(), O_RDONLY);

    // Родительский процесс создает дочерний процесс.
    const pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "ERROR: Error while creating child process" << std::endl;
        sem_close(semaphore);
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

    // Родительский процесс читает из pipe1 и прочитанное выводит в свой стандартный поток вывода.
    sem_wait(semaphore);
    std::string_view output(shared1->data);
    std::cout << output;

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
        std::cerr << "ERROR: Division by zero" << std::endl;
        munmap(shared1, sizeof(SharedData));
        close(fd1);
        sem_close(semaphore);
        return 1;
    }

    munmap(shared1, sizeof(SharedData));
    close(fd1);

    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);
    return 0;
}
