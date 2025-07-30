import serial
import time

class DS3231:
    def __init__(self, ser, addr='68'):
        self.ser = ser
        self.addr = addr

    def bcd2dec(self, bcd):
        return (bcd // 16) * 10 + (bcd % 16)

    def dec2bcd(self, dec):
        return (dec // 10) * 16 + (dec % 10)

    def write_datetime(self, year, month, day, week, hour, minute, second):
        data = [
            self.dec2bcd(second),
            self.dec2bcd(minute),
            self.dec2bcd(hour),
            self.dec2bcd(week),
            self.dec2bcd(day),
            self.dec2bcd(month),
            self.dec2bcd(year % 100)
        ]
        data_str = ','.join([f"{b:02X}" for b in data])
        cmd = f"write {self.addr} 08 00:{data_str}"
        self.ser.write((cmd + '\n').encode())
        while True:
            line = self.ser.readline().decode(errors='ignore').strip()
            if line and "OK" in line:
                break

    def read_datetime(self):
        write_cmd = f"write {self.addr} 01 00"
        self.ser.write((write_cmd + '\n').encode())
        while True:
            line = self.ser.readline().decode(errors='ignore').strip()
            if line and "OK" in line:
                break
            if line and "ERROR" in line:
                return None

        read_cmd = f"read {self.addr} 07"
        self.ser.write((read_cmd + '\n').encode())
        while True:
            line = self.ser.readline().decode(errors='ignore').strip()
            if line.startswith("OK"):
                parts = line[2:].strip().split(',')
                parts = [p.strip() for p in parts if p.strip()]
                if len(parts) >= 7:
                    sec = self.bcd2dec(int(parts[0], 16))
                    minute = self.bcd2dec(int(parts[1], 16))
                    hour = self.bcd2dec(int(parts[2], 16))
                    week = self.bcd2dec(int(parts[3], 16))
                    day = self.bcd2dec(int(parts[4], 16))
                    month = self.bcd2dec(int(parts[5], 16))
                    year = self.bcd2dec(int(parts[6], 16))
                    return year, month, day, week, hour, minute, sec
            if line and "ERROR" in line:
                break
        return None

if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    rtc = DS3231(ser)

    now = time.localtime()
    rtc.write_datetime(
        year=now.tm_year,
        month=now.tm_mon,
        day=now.tm_mday,
        week=now.tm_wday + 1,  # DS3231: 1=Monday, 7=Sunday
        hour=now.tm_hour,
        minute=now.tm_min,
        second=now.tm_sec
    )
    time.sleep(1)
    while True:
        dt = rtc.read_datetime()
        if dt is not None:
            year, month, day, week, hour, minute, sec = dt
            print(f"20{year:02d}-{month:02d}-{day:02d} (W{week}) {hour:02d}:{minute:02d}:{sec:02d}")
        else:
            print("Failed to read datetime")
        time.sleep(1)
