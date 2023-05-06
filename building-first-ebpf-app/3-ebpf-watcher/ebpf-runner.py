#!/usr/bin/env python3

from bcc import BPF
from pathlib import Path


def process_clone_event(cpu, data, size):
    event = bpf["clone_events"].event(data)
    print(f"Process {event.comm.decode()} (PID: {event.pid}, PPID: {event.ppid}) called sys_clone")


def process_open_event(cpu, data, size):
    event = bpf["open_events"].event(data)
    print(
        f"[{event.timestamp / 1e9:.6f}] Process {event.comm.decode()} (PID: {event.pid}) opened file: {event.filename.decode()}")


bpf_source = Path('ebpf-probe.c').read_text()
bpf = BPF(text=bpf_source)

bpf["clone_events"].open_perf_buffer(process_clone_event)
bpf["open_events"].open_perf_buffer(process_open_event)
print("Monitoring sys_clone and file open events... Press Ctrl+C to exit.")

while True:
    try:
        bpf.perf_buffer_poll()
    except KeyboardInterrupt:
        break
