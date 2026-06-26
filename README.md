# UART protocol
- First byte is one of the command (one ascii symbol)
- Second byte is channel number in case of "change duty cycle" command
- Two remaining bytes are lower and higher bits of number (frequency, duty cycle value)
