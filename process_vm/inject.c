#define _GNU_SOURCE // 需要定义_GNU_SOURCE才可以使用
#include <stdio.h>
#include <sys/uio.h>
int8_t maps[] = {"/proc/%d/maps"};
int64_t get_baseaddr(pid_t pid);
void process_vm_read();
void process_vm_write();
int main()
{
    pid_t pid = 0;
    printf("pid:");
    scanf("%d", &pid);
    int64_t start_addr = get_baseaddr(pid);
    int8_t buf[100] = {0};
    process_vm_read(pid, buf, 100, start_addr + 0x4010, 12);
    printf("%s\n", buf);
    process_vm_write(pid, "Hacker", 7, start_addr + 0x4010, 12);
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
void process_vm_read(pid_t pid, void *local_addr, size_t lsize, void *remote_addr, size_t rsize)
{
    struct iovec local;
    local.iov_base = local_addr;
    local.iov_len = lsize;
    struct iovec remote;
    remote.iov_base = remote_addr;
    remote.iov_len = rsize;
    process_vm_readv(pid, &local, 1, &remote, 1, 0);
}
void process_vm_write(pid_t pid, void *local_addr, size_t lsize, void *remote_addr, size_t rsize)
{
    struct iovec local;
    local.iov_base = local_addr;
    local.iov_len = lsize;
    struct iovec remote;
    remote.iov_base = remote_addr;
    remote.iov_len = rsize;
    process_vm_writev(pid, &local, 1, &remote, 1, 0);
}