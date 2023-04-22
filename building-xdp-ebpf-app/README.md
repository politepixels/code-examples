# Building eBPF XDP Programs - Linux Observability

This folder contains the code examples for the following video: https://www.youtube.com/watch?v=aD24HRMJ8cI

This Python script and eBPF program work together to count and report the number of packets per second passing through a specified network interface. 

It also drops packets with a specific source IP address (8.8.8.8) and logs the dropped packets' destination IP addresses. 

The script compiles and attaches the eBPF program to the network interface, sets up a signal handler to handle termination signals, and continuously polls the eBPF program for packet count and dropped packet information. 

When interrupted by the user or terminated through a SIGTERM signal, the script detaches the eBPF program from the network interface and exits gracefully.