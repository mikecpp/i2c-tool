import serial
import time
import font8x16

class SSD1306:
    WIDTH = 128
    HEIGHT = 64
    PAGES = HEIGHT // 8

    def __init__(self, ser, addr='3C'):
        self.ser = ser
        self.addr = addr
        self.buffer = [0] * (self.WIDTH * self.PAGES)

    def send_cmd(self, cmd_byte):
        cmd = f"write {self.addr} 02 00:{cmd_byte:02X}"
        # print(f"[send] {cmd}")  # Show sent text
        self.ser.write((cmd + '\n').encode())
        while True:
            line = self.ser.readline().decode(errors='ignore').strip()
            # print(f"[recv] {line}")  # Show received text
            if line and "OK" in line:
                break

    def send_data(self, data):
        data_str = ','.join([f"0x{b:02X}" for b in data])
        hex_len = f"0x{len(data) + 1:02X}"
        cmd = f"write {self.addr} {hex_len} 40:{data_str}"
        # print(f"[send] {cmd}")  # Show sent text
        self.ser.write((cmd + '\n').encode())
        while True:
            line = self.ser.readline().decode(errors='ignore').strip()
            # print(f"[recv] {line}")  # Show received text
            if line and "OK" in line:
                break

    def init(self):
        cmds = [
            0xAE, 0xA4, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14,
            0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB,
            0x40, 0xA6, 0xAF
        ]
        for cmd_byte in cmds:
            self.send_cmd(cmd_byte)

    def set_pixel(self, x, y, color):
        if x < 0 or x >= self.WIDTH or y < 0 or y >= self.HEIGHT:
            return
        byte = x + (y // 8) * self.WIDTH
        if color:
            self.buffer[byte] |= (1 << (y % 8))
        else:
            self.buffer[byte] &= ~(1 << (y % 8))

    def draw_text(self, x, y, text):
        for c in text:
            code = ord(c)
            if 32 <= code <= 127:
                font_bytes = font8x16.font8x16[code - 32]
            else:
                font_bytes = font8x16.font8x16[0]  # space
            for row in range(16):
                rowData = font_bytes[row]
                for col in range(8):
                    pixel = (rowData >> (7 - col)) & 0x1
                    self.set_pixel(x + col, y + row, pixel)
            x += 8

    def fill_screen(self, color):
        value = 0xFF if color else 0x00
        for i in range(len(self.buffer)):
            self.buffer[i] = value

    def draw_line(self, x1, y1, x2, y2):
        dx = abs(x2 - x1)
        dy = abs(y2 - y1)
        sx = 1 if x1 < x2 else -1
        sy = 1 if y1 < y2 else -1
        err = dx - dy
        while True:
            self.set_pixel(x1, y1, 1)
            if x1 == x2 and y1 == y2:
                break
            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                x1 += sx
            if e2 < dx:
                err += dx
                y1 += sy

    def display(self):
        for page in range(self.PAGES):
            self.send_cmd(0xB0 + page)
            self.send_cmd(0x00)
            self.send_cmd(0x10)
            data = self.buffer[self.WIDTH * page : self.WIDTH * (page + 1)]
            self.send_data(data)

if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ssd = SSD1306(ser)
    ssd.init()
    ssd.fill_screen(0)
    ssd.display()
    count = 1
    while True:
        ssd.fill_screen(0)
        ssd.draw_text(25, 0, "HELLO I2C")        
        ssd.draw_text(25, 20, f"COUNT: {count}")
        ssd.display()
        time.sleep(1)
        count += 1
