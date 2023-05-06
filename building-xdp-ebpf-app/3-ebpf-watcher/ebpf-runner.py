# Import necessary libraries
from bcc import BPF
from time import sleep
from pathlib import Path
import signal
import ctypes
import socket
import struct


class TerminateSignal(Exception):
    pass


# Signal handler for SIGTERM
def handle_sigterm(signum, frame):
    raise TerminateSignal("Received SIGTERM, terminating...")


# Load and compile the eBPF program from the source file
def load_bpf_program():
    bpf_source = Path('ebpf-probe.c').read_text()
    bpf = BPF(text=bpf_source)
    return bpf


# Attach the eBPF program to the specified interface
def attach_xdp_program(bpf, interface):
    xdp_fn = bpf.load_func("xdp_packet_counter", BPF.XDP)
    bpf.attach_xdp(interface, xdp_fn, 0)
    return bpf


# Detach the eBPF program from the specified interface
def detach_xdp_program(bpf, interface):
    bpf.remove_xdp(interface, 0)


def print_debug_event(cpu, data, size):
    dest_ip = ctypes.cast(data, ctypes.POINTER(ctypes.c_uint32)).contents.value
    print(f"Packet to {socket.inet_ntoa(struct.pack('!L', dest_ip))} dropped")

# Main function to execute the script
def main():
    # Register the signal handler for SIGTERM
    signal.signal(signal.SIGTERM, handle_sigterm)

    # Define the network interface to monitor
    INTERFACE = "enp5s0"

    # Load the eBPF program and attach it to the network interface
    bpf = load_bpf_program()
    attach_xdp_program(bpf, INTERFACE)

    # Access the packet_count_map defined in the eBPF program
    packet_count_map = bpf.get_table("packet_count_map")
    bpf["debug_events"].open_perf_buffer(print_debug_event)

    try:
        print("Counting packets, press Ctrl+C to stop...")
        prev_total_packets = 0
        while True:
            # Sleep for 1 second before checking the packet count again
            sleep(1)
            total_packets = 0
            # Iterate over keys in the packet_count_map and sum their values
            for key in packet_count_map.keys():
                counter = packet_count_map[key]
                if counter:
                    total_packets += counter.value

            # Calculate the number of packets received per second
            packets_per_second = total_packets - prev_total_packets
            prev_total_packets = total_packets
            print(f"Packets per second: {packets_per_second}")
            bpf.perf_buffer_poll(1)
    except (KeyboardInterrupt, TerminateSignal) as e:
            print(f"{e}. Interrupting eBPF runner.")
    finally:
        print("Detaching eBPF program and exiting.")
        # Detach the eBPF program from the network interface and clean up when the script is terminated
        detach_xdp_program(bpf, INTERFACE)


# Execute the main function when the script is run directly
if __name__ == "__main__":
    main()