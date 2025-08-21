# One-Time Pads Encryption

This repository implements an encryption and decryption system using the one-time pad (OTP) cipher. It includes C programs for encryption, decryption, key generation, and supporting utilities. The system consists of client-server applications for both encryption and decryption, as well as scripts and sample files for testing.

## Repository Structure

```
.
├── .idea/              # Project configuration (IDE settings)
├── Makefile            # Build automation for all programs
├── dec_client.c/h      # Decryption client implementation and header
├── dec_server.c/h      # Decryption server implementation and header
├── enc_client.c/h      # Encryption client implementation and header
├── enc_server.c/h      # Encryption server implementation and header
├── keygen.c/h          # One-time pad key generator and header
├── utility.c/h         # Shared utility functions and header
├── p5testscript        # Script for automated testing
├── plaintext1-5        # Example plaintext files for testing
```

## Main Components

- **Encryption Client & Server (`enc_client.c`, `enc_server.c`, `enc_client.h`, `enc_server.h`):**
  - Communicate over a network to encrypt data using a one-time pad key.
  - The client sends plaintext and key files to the server, which returns the ciphertext.

- **Decryption Client & Server (`dec_client.c`, `dec_server.c`, `dec_client.h`, `dec_server.h`):**
  - Perform decryption of ciphertext using the same one-time pad key.
  - The client sends ciphertext and key files to the server, which returns the decrypted plaintext.

- **Key Generator (`keygen.c`, `keygen.h`):**
  - Generates a random key file of specified length for use with the OTP cipher.

- **Utilities (`utility.c`, `utility.h`):**
  - Helper functions shared by the main client/server programs.

- **Makefile:**
  - Build targets for compiling all the clients, servers, and key generator programs.

- **Test Script (`p5testscript`):**
  - Bash or shell script to automate the compilation and testing of all components.

- **Sample Plaintext Files (`plaintext1`-`plaintext5`):**
  - Example text files to test encryption and decryption functionality.

## Usage

1. **Compile**:  
   Run `make` to build all binaries.

2. **Generate Key**:  
   `./keygen <keylength> > mykey`

3. **Start Servers**:  
   - Encryption: `./enc_server <port>`
   - Decryption: `./dec_server <port>`

4. **Run Clients**:  
   - Encryption: `./enc_client <plaintext> <key> <port>`
   - Decryption: `./dec_client <ciphertext> <key> <port>`

5. **Test**:  
   Run `./p5testscript` for automated tests.

## Notes

- The `.idea/` directory contains IDE configuration and can be ignored.
- All source code is in C.
- Only use keys once (per OTP requirements) and ensure keys are as long as the message.

## License

[Specify your license here]

---

*This README provides a summary of the structure and purpose of each file in the repository. For detailed usage, see comments within the code or the test script.*
