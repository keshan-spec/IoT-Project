import serial

arduino_port = "COM8"
baud = 115200
fileName = "readings.csv"

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)

file = open(fileName, "a")
print("Created file")


samples = 10  # how many samples to collect
print_labels = False
line = 0  # start at 0 because our header is 0 (not real data)

while line <= samples:
    # incoming = ser.read(9999)
    # if len(incoming) > 0:
    if print_labels:
        if line == 0:
            print("Printing Column Headers")
        else:
            print("Line " + str(line) + ": writing...")
    getData = str(ser.readline())
    data = getData[0:][:-2]
    print(data)

    file.write(data + "\\n")  # write data with a newline
    line += 1

print("Data collection complete!")
file.close()
