# pico-smart-nuke

This is a modified version of the official `flash_nuke` for clearing Pico memory, which includes:

1. Error handling
2. Memory verification
3. LED status codes

WARNING!
This tool will completely erase the flash memory of your Pico.

- **Interrupt Protection**: Disables interrupts during memory operations to prevent crashes.
- **Verification**: Reads the memory back to ensure it was successfully erased.
- **Custom Marker**: Leaves a `"RASPBERRY CLEANED"` signature in the first page of memory.
- **Status Feedback**: Uses the onboard LED to report the result of the operation.

LED blink codes:
| Blinks | Code Constant | Meaning |
| :--- | :--- | :--- |
| **2** | `ERR_NO_ID` | Error: Flash ID is invalid or unreadable. |
| **3** | `ERR_BAD_SIZE` | Error: Flash size is smaller than expected. |
| **4** | `OK_CLEANED` | Success: Memory is completely erased and verified. |
| **5** | `ERR_VERIFY` | Error: Verification failed (memory might be corrupted). |
