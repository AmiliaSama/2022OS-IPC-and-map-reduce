#include "signal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/shm.h"
#include "sys/types.h"
#include "unistd.h"
#include <bits/stdc++.h>
using namespace std;
string to_find;
void myfun(int signum) { return; }
int main(int argc, char *argv[]) // 从命令行输入2个参数 inputfile keyword
{
    if (argc == 2) {
        puts("Please input the key word");
        cin >> to_find;
    } else {
        to_find = argv[2];
    }
    int shmid;
    int key;
    char *p;
    int pid;
    shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0777);
    if (shmid < 0) {
        printf("创建共享内存失败\n");
        return -1;
    }
    printf("创建共享内存成功 shmid=%d\n", shmid);
    pid = fork(); // 创建进程pid
    if (pid > 0)  // 这是父进程
    {
        FILE *fp = fopen(argv[1], "r");
        signal(SIGUSR2,
               myfun); // 处理函数，因为默认是终止，去处理子进程发过来的xh
        p = (char *)shmat(shmid, NULL, 0);
        if (p == NULL) {
            printf("父进程映射失败\n");
            return -2;
        }
        while (!feof(fp)) {
            // printf("父进程开始写\n");
            fgets(p, 1024, fp);
            kill(pid, SIGUSR1); // 给内核发信号去告诉子进程去读
            pause();            // 等待子进程读
        }
    }
    if (pid == 0) {
        vector<string> v;
        signal(SIGUSR1, myfun); // 去处理父进程发过来的信号
        p = (char *)shmat(shmid, NULL, 0);
        if (p == NULL) {
            printf("子进程映射失败\n");
            return -3;
        }
        while (1) // 循环读
        {
            pause(); // 等待父进程写，当信号被内核唤醒后开始读
            // printf("子进程开始读data:%s", p);
            v.push_back(p);
            kill(getppid(), SIGUSR2); // 读完了也要给内核发信号告诉父进程读完了
        }
    }
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}