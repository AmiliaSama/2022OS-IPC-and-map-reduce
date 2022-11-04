#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

#define buffer_size 1024

string to_find;
int main(int argc, char *argv[]) {
    if (argc == 2) {
        puts("Please input the key word");
        cin >> to_find;
    } else {
        to_find = argv[2];
    }

    /*创建管道*/
    int fd_pass[2], fd_solve[2];
    if (pipe(fd_pass) == -1) {
        perror("pipe");
        return -1;
    }

    if (pipe(fd_solve) == -1) {
        perror("pipe");
        return -1;
    }

    /*创建进程*/
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        /*child*/
        close(fd_pass[1]); // 数据关闭写，保留读
        char buffer[buffer_size];
        char c;
        transform(to_find.begin(), to_find.end(), to_find.begin(), ::tolower);
        int len = to_find.size();
        string tmp = "";
        vector<string> ans;
        int i = 0;
        // FILE *fp = fopen("output.txt", "w");
        while (read(fd_pass[0], &c, 1)) {
            tmp += c;
            if (c == '\n') {
                string res = tmp;
                transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
                int pos = tmp.find(to_find);
                if (pos != tmp.npos) {
                    if (pos == 0 || !isalpha(tmp[pos - 1])) {
                        if (pos + to_find.size() == tmp.size() ||
                            !isalpha(tmp[pos + to_find.size()])) {
                            ans.push_back(res);
                            // printf("add %s", res.c_str());
                        }
                    }
                }
                tmp = "";
            }
        }
        close(fd_pass[0]);

        /*处理好的数据传回parent*/
        close(fd_solve[0]);
        for (auto i : ans) {
            write(fd_solve[1], i.c_str(), i.size());
        }
        close(fd_solve[1]);
    } else {
        /*parent*/
        FILE *fp;
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            puts("Failed to open file");
            return 0;
        }
        close(fd_pass[0]); // 关闭读，保留写
        char buffer[buffer_size];
        int i = 0;
        while (!feof(fp)) {
            buffer[0] = '\0';
            // 最后一行如果只是一个回车，会读不进buffer里，导致buffer还是上一行的数据，所以每次都手动清空
            fgets(buffer, buffer_size, fp);
            write(fd_pass[1], buffer, strlen(buffer));
        }
        close(fd_pass[1]); // 写入管道结束要关闭，否则子进程一直等待读取会阻塞

        close(fd_solve[1]);
        char c;
        string tmp = "";
        vector<string> ans;
        fp = fopen("output.txt", "w");
        while (read(fd_solve[0], &c, 1)) {
            tmp += c;
            if (c == '\n') {
                ans.push_back(tmp);
                tmp = "";
            }
        }
        sort(ans.begin(), ans.end());
        for (auto i : ans) {
            fprintf(fp, "%s", i.c_str());
        }
        close(fd_solve[0]);
        fclose(fp);
        wait(NULL);
    }
    return 0;
}