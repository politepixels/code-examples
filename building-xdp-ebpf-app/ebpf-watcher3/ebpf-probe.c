#include <uapi/linux/bpf.h>
#include <uapi/linux/if_ether.h>
#include <uapi/linux/if_packet.h>
#include <uapi/linux/ip.h>
#include <linux/in.h>
#include <bcc/helpers.h>

BPF_ARRAY(packet_count_map, __u64, 1);
BPF_PERF_OUTPUT(debug_events);

// The function takes in a pointer to an XDP metadata struct and a destination IP address
static int drop_packet_to_destination(struct xdp_md *ctx, __be32 blocked_ip) {
    // Extract the end of the packet data from the metadata struct
    void *data_end = (void *)(long)ctx->data_end;

    // Extract the start of the packet data from the metadata struct
    void *data = (void *)(long)ctx->data;

    // Get a pointer to the Ethernet header of the packet
    struct ethhdr *eth = data;

    // If the Ethernet header extends beyond the end of the packet data, pass the packet
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    // If the Ethernet header does not indicate an IP packet, pass the packet
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;

    // Get a pointer to the IP header of the packet
    struct iphdr *iph = (struct iphdr *)(data + ETH_HLEN);

    // If the IP header extends beyond the end of the packet data, pass the packet
    if ((void *)(iph + 1) > data_end)
        return XDP_PASS;

    // If the destination IP address of the packet matches the specified IP address, drop the packet
    if (iph->saddr == blocked_ip) {
        __be32 saddr_copy = iph->saddr;
        debug_events.perf_submit(ctx, &saddr_copy, sizeof(saddr_copy));

        return XDP_DROP;
    }

    // Otherwise, pass the packet
    return XDP_PASS;
}

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

    // Call the drop_packet_to_destination function with the desired destination IP (8.8.8.8)
    __be32 blocked_ip = (8 << 24) | (8 << 16) | (8 << 8) | 8;
    return drop_packet_to_destination(ctx, blocked_ip);
}