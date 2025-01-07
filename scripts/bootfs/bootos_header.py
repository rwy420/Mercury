# BootOS Header located in sector 1 (0x200) - 17 (0x2200)

end_address = 17 * 0x200

boot_entry = "KERNEL+STAGE2".encode() + bytes.fromhex("0000000000000000000001") + bytes.fromhex("0000000A") + bytes.fromhex("0000004A");

with open("header.bin", "wb") as file:
    file.write(boot_entry);
