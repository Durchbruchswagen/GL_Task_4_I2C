import struct
import time
import sys
import serial

class STM32Controller:
    def __init__(self, port="COM4", baudrate=9600):
        print(f"Opening connection on {port} @ {baudrate} baud...")
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.5,
        )
        time.sleep(1) 
        print("Connected! Type 'help' or 'h' to see available commands. Type 'exit' to quit.\n")

    def _send_packet(self, command: str, channel: int, payload: int):
        command_bytes = command.encode("ascii")
        packet = struct.pack("<cBH", command_bytes, channel, payload)
        self.ser.write(packet)
        
        time.sleep(0.1)
        self._read_state_feedback()

    def _read_state_feedback(self):
        expected_bytes = 18 * 2
        self.ser.timeout = 2.0
        if self.ser.in_waiting >= expected_bytes:
            raw_data = self.ser.read(expected_bytes)
            state_array = struct.unpack(f"<{18}H", raw_data)
            print(" -> STM32 Ack Received.")
            print(f"    Mode Reg Status: {bin(state_array[0])} | Prescaler/Freq: {state_array[1]} Hz")
            print(f"    Channels 0-7  Duty: {list(state_array[2:10])}")
            print(f"    Channels 8-15 Duty: {list(state_array[10:18])}")
        else:
            print(f"no data {expected_bytes}, {self.ser.in_waiting}")
        self.ser.timeout = 0.5
    def handle_input(self, user_input: str):
        parts = user_input.strip().split()
        if not parts:
            return

        cmd = parts[0].lower()

        if cmd in ("exit", "quit", "q"):
            print("Closing serial interface...")
            self.ser.close()
            sys.exit(0)

        elif cmd in ("help", "h"):
            print("Available Commands:")
            print("  s               -> Go to Sleep")
            print("  w               -> Wake up device")
            print("  o [0 or 1]      -> Output Enable pin configuration (0=Off, 1=On)")
            print("  f [frequency]   -> Set global PWM Frequency (0-65535)")
            print("  d [ch] [duty]   -> Set Channel (0-15) Duty Cycle (0-4095)")
            print("  exit / q        -> Close interactive terminal\n")

        elif cmd == "s":
            self._send_packet("s", 0, 0)

        elif cmd == "w":
            self._send_packet("w", 0, 0)

        elif cmd == "o":
            if len(parts) < 2 or parts[1] not in ("0", "1"):
                print("Syntax Error: Use 'o 0' (disable) or 'o 1' (enable)")
                return
            self._send_packet("o", int(parts[1]), 0)

        elif cmd == "f":
            if len(parts) < 2:
                print("Syntax Error: Use 'f [frequency_value]'")
                return
            try:
                freq = int(parts[1])
                if 0 <= freq <= 65535:
                    self._send_packet("f", 0, freq)
                else:
                    print("Value Error: Frequency must be 0 - 65535")
            except ValueError:
                print("Parsing Error: Frequency must be a valid integer")

        elif cmd == "d":
            if len(parts) < 3:
                print("Syntax Error: Use 'd [channel] [duty_cycle]'")
                return
            try:
                ch = int(parts[1])
                duty = int(parts[2])
                if (0 <= ch <= 15) and (0 <= duty <= 4095):
                    self._send_packet("d", ch, duty)
                else:
                    print("Value Error: Ensure channel is 0-15 and duty is 0-4095")
            except ValueError:
                print("Parsing Error: Channel and Duty values must be integers")
        else:
            print(f"Unknown Command '{cmd}'. Type 'help' for syntax mapping.")


def main():
    # Still allows setting non-default connection configs on launch if ever needed
    port = sys.argv[1] if len(sys.argv) > 1 else "COM4"
    baud = int(sys.argv[2]) if len(sys.argv) > 2 else 9600

    try:
        controller = STM32Controller(port=port, baudrate=baud)
        
        # Live interactive execution loop
        while True:
            try:
                user_entry = input("PCA9685 >>> ")
                controller.handle_input(user_entry)
            except KeyboardInterrupt:
                print("\nForced termination. Closing cleanly...")
                controller.ser.close()
                break
    except Exception as e:
        print(f"Failed to bind connection parameters: {e}")

if __name__ == "__main__":
    main()