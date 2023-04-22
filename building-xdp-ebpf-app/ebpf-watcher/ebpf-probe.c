// Include necessary header files for eBPF, XDP, and networking
#include <uapi/linux/bpf.h>
#include <uapi/linux/if_ether.h>
#include <uapi/linux/if_packet.h>
#include <uapi/linux/ip.h>
#include <linux/in.h>
#include <bcc/helpers.h>

// Create a BPF_ARRAY named packet_count_map with a single __u64 element
// This map will store the packet count
BPF_ARRAY(packet_count_map, __u64, 1);

// Define the XDP program function, xdp_packet_counter
// This function is triggered for every incoming packet on the attached network interface
int xdp_packet_counter(struct xdp_md *ctx) {
    // Declare a key variable to access the packet_count_map
    __u32 key = 0;
    // Declare a pointer to a __u64 variable, which will point to the counter value in the packet_count_map
    __u64 *counter;

    // Lookup the counter value in the packet_count_map using the key
    counter = packet_count_map.lookup(&key);
    // If the lookup fails (counter is NULL), abort the XDP program
    if (!counter)
        return XDP_ABORTED;

    // Atomically increment the counter value by 1
    // This ensures that the counter is updated correctly even when multiple packets are processed concurrently
    __sync_fetch_and_add(counter, 1);

    // Pass the packet along to the next stage in the networking stack
    return XDP_PASS;
}