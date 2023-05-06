#!/usr/bin/env python3

from bcc import BPF
from pathlib import Path


def process_event(cpu, data, size):
    event = bpf["events"].event(data)
    print(f"Process {event.comm.decode()} (PID: {event.pid}, PPID: {event.ppid}) called sys_clone")


bpf_source = Path('ebpf-probe.c').read_text()
bpf = BPF(text=bpf_source)

bpf["events"].open_perf_buffer(process_event)
print("Monitoring sys_clone events... Press Ctrl+C to exit.")

while True:
    try:
        bpf.perf_buffer_poll()
    except KeyboardInterrupt:
        break
