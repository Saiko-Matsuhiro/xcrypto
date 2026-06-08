# C-File-Encryptor (XOR Cryptor)

A lightweight CLI tool written in C for encrypting and decrypting files using an obfuscated XOR-based cipher. It utilizes a custom 1024-byte key-stretching and hashing algorithm to maximize local data privacy.

## Features

- **XOR Encryption:** Rapid and lightweight file stream encoding/decoding.
- **Key-Stretching & Obfuscation:** Expands user-inputted passphrases into a secure 1024-byte state array using custom bitwise and modular arithmetic.
- **Split-Key Security:** Stores only a 32-byte proof-of-concept hash on the disk (`config.txt`) for authentication, while the actual encryption uses a hidden 256-byte trailing slice from the internal memory buffer.
- **Interactive CLI:** Process multiple files in a single session.

## Architecture & Security Workflow

1. **Initialization:** On the first run, the tool asks you to create a master password.
2. **Key Generation:** The password goes through `hash_to_pass()`, performing multi-pass non-linear mixing across a 1024-byte buffer.
3. **Verification:** The first 32 bytes of the resulting hash are saved to `config.txt` to verify future login attempts.
4. **Encryption Key:** The file stream is XORed using bytes `[767 to 1022]` of the expanded passphrase state. This data never touches the hard drive in plaintext.

## How to Build and Run

### Prerequisites
Make sure you have a C compiler (`gcc` or `clang`) installed on your system.

### Compilation
Compile the source code using the following terminal command:
```bash
gcc -o xcrypt main.c
```

### Running the App
Execute the compiled binary:
```bash
./xcrypt
```

### Usage Flow
1. Run the app and enter your master password.
2. Enter the exact filename (e.g., `data.txt`) you want to encrypt or decrypt.
3. The utility applies the XOR cipher in-place, rewriting the file content.
4. Type `exit` to safely close the session and clear the memory.

## Disclaimer
*This project is built for educational purposes to demonstrate low-level file