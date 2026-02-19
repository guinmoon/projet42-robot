import serial
import time

Import("env")

def before_upload(source, target, env):
    port = env.get('UPLOAD_PORT', 'COM7')
    try:
        ser = serial.Serial(port, 115200)
        ser.setDTR(False)
        ser.setRTS(False)
        time.sleep(0.1)
        ser.setDTR(True)
        ser.setRTS(False)
        time.sleep(0.1)
        ser.setDTR(False)
        ser.close()
        time.sleep(1)
    except:
        pass

env.AddPreAction("upload", before_upload)