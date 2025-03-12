#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
int8_t maps[] = {"/proc/%d/maps"};
int64_t get_baseaddr(pid_t pid);
int64_t ptrace_attach(pid_t pid);
int64_t ptrace_detach(pid_t pid);
void ptrace_read_mem(pid_t pid, int64_t offset, int8_t *buf, size_t size);
void ptrace_write_mem(pid_t pid, int64_t offset, int8_t *buf, size_t size);
int main()
{
    pid_t pid = 0;
    printf("pid:");
    scanf("%d", &pid);
    int64_t start_addr = get_baseaddr(pid);
    int8_t buf[100] = {0};
    ptrace_read_mem(pid, 0x4010, buf, 12);
    printf("%s\n", buf);
    ptrace_write_mem(pid, 0x4010, "Hacker", 6);
    // printf("%lx", start_addr);
}
void ptrace_write_mem(pid_t pid, int64_t offset, int8_t *buf, size_t size)
{
    if (ptrace_attach(pid) == -1)
    {
        return;
    }

    int64_t baseaddr = get_baseaddr(pid);
    int64_t target_addr = baseaddr + offset;

    for (size_t i = 0; i < size; i += 8)
    {
        ptrace(PTRACE_POKETEXT, pid, target_addr + i, *(int64_t *)&buf[i]);
    }
    if (ptrace_detach(pid) == -1)
    {
        return;
    }
}
void ptrace_read_mem(pid_t pid, int64_t offset, int8_t *buf, size_t size)
{
    if (ptrace_attach(pid) == -1)
    {
        return;
    }

    int64_t baseaddr = get_baseaddr(pid);
    int64_t target_addr = baseaddr + offset;

    for (size_t i = 0; i < size; i += 8)
    {
        *(int64_t *)&buf[i] = ptrace(PTRACE_PEEKTEXT, pid, target_addr + i, NULL);
    }
    if (ptrace_detach(pid) == -1)
    {
        return;
    }
}
int64_t get_baseaddr(pid_t pid)
{
    int8_t target_maps[64] = {0};
    sprintf(target_maps, maps, pid);
    FILE *ptarget_maps = fopen(target_maps, "r");
    if (!ptarget_maps)
    {
        perror("fopen");
        return 0;
    }
    int8_t line[256];
    int64_t start_addr = 0;
    while (fgets(line, sizeof(line), ptarget_maps) != NULL)
    {
        int64_t start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) == 2)
        {
            start_addr = start;
            fclose(ptarget_maps);
            return start_addr;
        }
    }
}
int64_t ptrace_attach(pid_t pid)
{
    if (pid)
    {
        int status = 0;
        if (ptrace(PTRACE_ATTACH, pid, NULL, 0) < 0)
        {
            return -1;
        }
        waitpid(pid, &status, WUNTRACED);
        return 0;
    }
    else
    {
        perror("Invalid pid");
    }
}
int64_t ptrace_detach(pid_t pid)
{
    if (pid)
    {
        if (ptrace(PTRACE_DETACH, pid, NULL, 0) < 0)
        {
            return -1;
        }
        return 0;
    }
    else
    {
        perror("Invalid pid");
    }
}