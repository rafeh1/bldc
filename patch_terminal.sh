#!/bin/bash

FILE="terminal.c"

# Insert UL/LK/SP command registrations inside terminal_init()
sed -i '/void terminal_init(void)/a \
    terminal_register_command_callback("ul", "Unlock system", 0, terminal_cmd_enter_user_password); \
    terminal_register_command_callback("lk", "Lock system", 0, terminal_cmd_lock_system); \
    terminal_register_command_callback("sp", "Set password", 0, terminal_cmd_new_user_password);' "$FILE"

echo "terminal.c updated with UL/LK/SP commands."
