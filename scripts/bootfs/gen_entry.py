import argparse
import struct
import os

def main():
    parser = argparse.ArgumentParser(description="Generate BootFS header entry")
    
    parser.add_argument("file_path", type=str, help="Path to the file")
    parser.add_argument("name", type=str, help="Name of the file for the entry")
    parser.add_argument("type", type=float, help="The files type")
    parser.add_argument("initial_sector", type=float, help="The files first disk sector")
    
    args = parser.parse_args()

    file_size = os.path.getsize(args.file_path)

    entry_name = pad_string(args.name, 22).encode()
    entry_type = to_uint16(int(args.type))
    entry_sector_count = int(file_size / 512 + 1)
    entry_initial_sector = to_uint32(args.initial_sector)
    entry_last_sector = to_uint32(int(args.initial_sector + entry_sector_count))

    entry = entry_name + entry_type + entry_initial_sector + entry_last_sector

    print(entry.hex())

    print(f"Sector count: {entry_sector_count} [{int(args.initial_sector * 512)} - {int(args.initial_sector + entry_sector_count) * 512}]")

def pad_string(string, length):
    truncated_string = string[:length]
    padded_string = truncated_string.ljust(length, '\0')
    return padded_string

def to_uint32(number):
    bytes = struct.pack('<i', int(number))
    return bytes

def to_uint16(number):
    bytes = struct.pack('<h', number);
    return bytes;

if __name__ == "__main__":
    main()

