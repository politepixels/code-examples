#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <bcc/proto.h>

struct data_t {
    u32 pid;
    u32 ppid;
    char comm[TASK_COMM_LEN];
};

BPF_PERF_OUTPUT(events);

int kprobe__sys_clone(void *ctx) {
    struct data_t data = {};
    struct task_struct *task;

    task = (struct task_struct *)bpf_get_current_task();
    data.pid = bpf_get_current_pid_tgid();
    data.ppid = task->real_parent->tgid;
    bpf_get_current_comm(&data.comm, sizeof(data.comm));

    events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}