#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd)) {
        std::cerr << "ERROR: Pipe creation fault" << std::endl;
        return 1;
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
        return 1;
    }

    // Дочерний процесс
    if (pid == 0) {
        // Стандартный поток ввода дочернего процесса переопределяется открытым файлом.
        dup2(file, STDIN_FILENO);

        // Стандартный поток вывода дочернего процесса перенаправляется в pipe1
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        // Родительский и дочерний процесс должны быть представлены разными программами.
        execl("./child", "./child", nullptr);

        std::cerr << "ERROR: Can't execute child process" << std::endl;
        return 1;
    }

    // Родительский процесс
    close(fd[1]);

    // Родительский процесс читает из pipe1 и прочитанное выводит в свой стандартный поток вывода.
    char a[10];
    while (const ssize_t size = read(fd[0], a, sizeof(a) / sizeof(char))) {
        std::string str(a, size);
        std::cout << str;
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
        std::cerr << "ERROR: Division by zero" << std::endl;
        return 1;
    }

    close(fd[0]);
    return 0;
}
