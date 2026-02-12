import pandas as pd
import serial
import time

CSV_FILE = "flt_0015.csv"
SERIAL_PORT = "COM6"
BAUD_RATE = 115200

df = pd.read_csv(CSV_FILE)

# Convert microseconds to seconds / brand new column
df["Time(s)"] = df["Time(us)"] / 1000000

ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
# wait for arduino to restart
time.sleep(2)

print("Starting Data Transfer...")

start_time = time.time()

# Sea Level Pressure
P0 = 101325.0

for _, row in df.iterrows():
    target_time = row["Time(s)"]
    altitude = row["Altitude(m)"]

    # Wait until time to send value
    while (time.time() - start_time) < target_time:
        pass
    
    # Calculate Pressure
    pressure = P0 * (1 - altitude / 44330.0) ** (1 / 0.1903)

    ser.write(f"{target_time} {pressure}\n".encode())
    
    # Read response from Arduino
    response = ser.readline().decode().strip()
    
    print("", response)

print("Data transfer complete")
ser.close()