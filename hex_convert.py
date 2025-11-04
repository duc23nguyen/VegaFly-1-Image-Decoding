import re

def clean_and_convert(input_file, output_file):
    all_bytes = []

    with open(input_file, "r", encoding="utf-8") as f:
        raw_text = f.read()

    for line in raw_text.splitlines():
        if not line.strip():
            continue

        # Remove prefix "1: [SANOSAT-2]" and timestamp "[hh:mm:ssR]"
        line = re.sub(r"^\d+:\s*\[SANOSAT-2\]\s*\[\d{2}:\d{2}:\d{2}R\]\s*", "", line)

        if not line:
            continue

        # Remove sequences of FF FF 00 00
        line = re.sub(r"(FF\s+FF\s+00\s+00\s*)+", "", line)

        # Split into hex bytes and collect them all
        all_bytes.extend(line.strip().split())

    # Convert all collected bytes into C-style hex
    c_array = ", ".join(f"0x{b}" for b in all_bytes)

    # Write to output file
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(f"{{{c_array}}}")


if __name__ == "__main__":
    input_file = "example.txt"     # input telemetry dump
    output_file = "output.txt"    # output C array

    clean_and_convert(input_file, output_file)
    print(f"✅ Conversion complete. All data saved in {output_file} as one frame.")
