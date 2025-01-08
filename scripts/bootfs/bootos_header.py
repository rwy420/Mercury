import os;

boot_entry = "KERNEL+STAGE2".encode() + bytes.fromhex("0000000000000000000001") + bytes.fromhex("A0000000") + bytes.fromhex("4A000000") + bytes.fromhex("53797374656d204d6f64756c6500000000000000000003004b00000068000000")

with open("header.bin", "wb") as file:
    print(os.path.realpath("header.bin"))
    file.write(boot_entry);
