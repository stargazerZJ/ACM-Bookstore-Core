#!/usr/bin/env python3
import sys

def read_file(file_path, offset):
    try:
        with open(file_path, 'rb') as file:
            print("Reading file: " + file_path)
            file_size = len(file.read())
            if file_size % 4 != 0:
                print("Error: File size is not divisible by 4")
                sys.exit(1)
            print ("File size: " + str(file_size // 4) + " integers")

            file.seek(offset)

            while True:
                data = file.read(4)
                if not data:
                    break
                integer_value = int.from_bytes(data, byteorder='little', signed=True)
                print("{:4d}".format(integer_value), end=" ")
            print()

    except FileNotFoundError:
        print("Error: File not found")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py file_path [offset]")
        sys.exit(1)

    file_path = sys.argv[1]
    offset = int(sys.argv[2]) if len(sys.argv) == 3 else 0
    read_file(file_path, offset)
