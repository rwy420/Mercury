# BootOS Header located in sector 1 (0x200) - 17 (0x2200)

end_address = 17 * 0x200

boot_entry = "KERNEL+STAGE2\0".encode() + bytes.fromhex("000000000000000001") + bytes.fromhex("00000000") + bytes.fromhex("00004000") + bytes.fromhex("00");

with open("header.bin", "wb") as file:
    file.write(boot_entry);
