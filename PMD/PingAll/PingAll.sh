#!/bin/bash
# from AI Overview

# Function to ping an IP address in the background
ping_ip() {
    ip="$1"
    ping -c 1 "$ip" > /dev/null 2>&1 &
}

# Read IP addresses from a file (or provide them as arguments)
if [ $# -gt 0 ]; then
    ips=("$@")
else
    read -p "Enter IP addresses (separated by space): " ips
fi

# Ping each IP address in a separate thread
for ip in "${ips[@]}"; do
    ping_ip "$ip" &
done

# Wait for all background processes to finish
wait