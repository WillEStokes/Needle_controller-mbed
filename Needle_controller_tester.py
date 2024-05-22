import socket
import struct
import time
from PyQt6.QtCore import QThread

IP_ADDRESS = "192.168.5.101"
TCP_PORT = 7851

FID_GET_STATUS = 0
FID_GET_SYSTEM_INFO = 1
FID_GET_FT_SENSOR_DATA = 2
FID_GET_ENCODER_SENSOR_DATA = 3
FID_GET_ALL_SENSOR_DATA = 4
FID_GET_ALL_SENSOR_DATA_MULTIPLE = 5
FID_START_ACQUISITION_STREAM = 6
FID_STOP_ACQUISITION_STREAM = 7
FID_RESET_ADC = 8
FID_CHECK_ADC = 9
FID_SET_ADC_CONVERSION_MODE = 10
FID_SET_ADC_DATA_RATE = 11

class DataStream(QThread):
    def __init__(self, socket):
        super().__init__()
        self.socket = socket
        self.stopped = False
        self.time = 0
        self.all_data = tuple([0] * 9)
        self.data_offset = tuple([0] * 9)

    def set_data_offset(self):
        self.data_offset = self.all_data

    def run(self):
        all_data_size = struct.calcsize('<ffffffffff')
        while not self.stopped:
            try:
                data = self.socket.recv(all_data_size)
                if not data:
                    break

                self.time = struct.unpack('<I', data[:4])[0]
                ft_data = struct.unpack('<ffffff', data[4:28])
                encoder_data = struct.unpack('<fff', data[28:])

                self.all_data = ft_data + encoder_data
                adjusted_data = tuple(x - offset for x, offset in zip(self.all_data, self.data_offset))

                # Overwrite the previous line in the terminal
                print(f"\rTime: {self.time}, Data: {adjusted_data}", end='')

            except Exception as e:
                print(f"\rError reading data from the Ethernet device: {e}", end='')
                break

    def stop(self):
        self.stopped = True

class K64F:
    def __init__(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.connected = False
        self.data_stream = None

    def init_ethernet(self):
        try:
            self.socket.connect((IP_ADDRESS, TCP_PORT))
            self.connected = True
            print("Connected to Ethernet device.")
        except Exception as e:
            print(f"Error connecting to Ethernet device: {e}")
            self.connected = False

    def start_data_stream(self):
        self.data_stream = DataStream(self.socket)
        self.data_stream.start()

    def stop_data_stream(self):
        if self.data_stream:
            self.data_stream.stop()

    def invoke_fid(self, fid):
        packet_length = 4
        error = 0
        message_header = struct.pack('<HBB', packet_length, fid, error)
        self.socket.sendall(message_header)

    def start_acquisition(self):
        if not self.connected:
            print("Not connected to the Ethernet device.")
            return

        self.invoke_fid(FID_START_ACQUISITION_STREAM)

        print("Start acquisition message sent.")
        self.start_data_stream()

    def stop_acquisition(self):
        if not self.connected:
            print("Not connected to the Ethernet device.")
            return

        self.invoke_fid(FID_STOP_ACQUISITION_STREAM)

        print("Stop acquisition message sent.")
        self.stop_data_stream()

    def set_data_offset(self):
        if self.data_stream:
            self.data_stream.set_data_offset()

    def close(self):
        if self.connected:
            self.socket.close()
            self.connected = False
            print("Connection to Ethernet device closed.")

if __name__ == "__main__":
    k64f = K64F()
    k64f.init_ethernet()
    k64f.start_acquisition()
    time.sleep(3)
    k64f.stop_acquisition()
    time.sleep(0.1)
    k64f.close()