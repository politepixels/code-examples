#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <bcc/proto.h>

struct clone_data_t {
    u32 pid;
    u32 ppid;
    char comm[TASK_COMM_LEN];
};

struct open_data_t {
    u32 pid;
    u64 timestamp;
    char comm[TASK_COMM_LEN];
    char filename[NAME_MAX];
};

BPF_PERF_OUTPUT(clone_events);
BPF_PERF_OUTPUT(open_events);

int kprobe__sys_clone(void *ctx) {
    struct clone_data_t data = {};
    struct task_struct *task;

    task = (struct task_struct *)bpf_get_current_task();
    data.pid = bpf_get_current_pid_tgid();
    data.ppid = task->real_parent->tgid;
    bpf_get_current_comm(&data.comm, sizeof(data.comm));

    clone_events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}

int kprobe__do_sys_openat2(struct pt_regs *ctx, int dfd, const char __user *filename, int flags, umode_t mode) {
    struct open_data_t data = {};

    data.pid = bpf_get_current_pid_tgid();
    data.timestamp = bpf_ktime_get_ns();
    bpf_get_current_comm(&data.comm, sizeof(data.comm));

    bpf_probe_read_user_str(&data.filename, sizeof(data.filename), filename);

    open_events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}