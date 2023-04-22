# Building eBPF Programs - Linux Observability

This folder contains the code examples for the following video: https://www.youtube.com/watch?v=CprgVJeJ-04

This Python script and eBPF program work together to monitor the occurrences of sys_clone and sys_openat system calls. 

The script uses the Prometheus Python client to expose counters for these calls as metrics, and starts an HTTP server on port 3000 to serve these metrics. 

The eBPF program hooks into the kernel's sys_clone and sys_openat2 functions using kprobes, collecting information about the process making the calls and the files being opened. 

The script processes these events, printing the process information and incrementing the Prometheus counters. 

The script runs continuously, polling the eBPF program for new events until interrupted by the user with Ctrl+C.