class label:
    def __init__(self, name = "[LABEL]",
                 shift = 0, mask = 0xFFFFFFFF, comment = "", annotation = "", *args, **kwargs):
        self.name = name
        self.shift = shift
        self.mask = mask
        self.comment = comment
        self.annotation = annotation

class labelbits:
    def __init__(self, labels = [], bits = 32, *args, **kwargs):
        self.bits = bits
        self.labels = labels

    def cover(self, binary, item):
        masked = ""
        mask = item.mask << item.shift
        for i in range(self.bits -1, -1, -1):
            if ((mask >> i) & 1):
                masked += str((binary >> i) & 1)
            else:
                masked += "-"
        return masked

    def mode(self, binary):
        mapping = ["PULL_NONE", "PULL_UP", "PULL_DOWN", "PULL_BOTH"]
        if (binary >= 0) and (binary < len(mapping)):
            return mapping[binary]
        else:
            return str(binary)

    def info(self, binary, indent = 32):
        print("<<<LabelBits>>>")
        raw = format(binary, "#0" + str(self.bits + 2) + "b")[2:]
        print(("Raw Data:\t").expandtabs(indent) + str(raw) + " [LSB]\n")
        for item in self.labels:
            data = (binary >> item.shift) & item.mask
            line = str(item.name) + ":\t" + self.cover(binary, item)
            comment = "//" + str(item.comment)
            print(comment + "\n" + line.expandtabs(indent) + " [" +
                  self.mode(data) + "]\t" + item.annotation + "\n")

PULL_LABELS = \
[
    label("GPIO_PUP_PDN_CNTRL16", 0,  0b11, "GPIO 16"),
    label("GPIO_PUP_PDN_CNTRL17", 2,  0b11, "GPIO 17"),
    label("GPIO_PUP_PDN_CNTRL18", 4,  0b11, "GPIO 18"),
    label("GPIO_PUP_PDN_CNTRL19", 6,  0b11, "GPIO 19"),
    label("GPIO_PUP_PDN_CNTRL20", 8,  0b11, "GPIO 20"),
    label("GPIO_PUP_PDN_CNTRL21", 10, 0b11, "GPIO 21"),
    label("GPIO_PUP_PDN_CNTRL22", 12, 0b11, "GPIO 22", "//JTAG TRST (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL23", 14, 0b11, "GPIO 23", "//JTAG RTCK (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL24", 16, 0b11, "GPIO 24", "//JTAG TDO  (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL25", 18, 0b11, "GPIO 25", "//JTAG TCK  (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL26", 20, 0b11, "GPIO 26", "//JTAG TDI  (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL27", 22, 0b11, "GPIO 27", "//JTAG TMS  (PULL_NONE)"),
    label("GPIO_PUP_PDN_CNTRL28", 24, 0b11, "GPIO 28"),
    label("GPIO_PUP_PDN_CNTRL29", 26, 0b11, "GPIO 29"),
    label("GPIO_PUP_PDN_CNTRL30", 28, 0b11, "GPIO 30"),
    label("GPIO_PUP_PDN_CNTRL31", 30, 0b11, "GPIO 31"),
]

GPIO = labelbits(PULL_LABELS)
GPIO.info(0x19AAAAAA) # Offset 0xE8
