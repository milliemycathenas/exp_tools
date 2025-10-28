import numpy as np
import os

def read_spe_file(filepath, dtype=np.int32, header_size=8192):
    with open(filepath, 'rb') as f:
        f.read(header_size)
        data = f.read()
        spectrum = np.frombuffer(data, dtype=dtype)
    return spectrum

def save_csv(data, output_path):
    np.savetxt(output_path, data, fmt='%d', delimiter='\n')
    print(f"CSV file saved (no headers): {output_path}")

def main():
    input_path = input("Enter the path to the .spe file: ").strip()
    output_path = input("Enter the output CSV file name (e.g., output.csv): ").strip()

    if not os.path.exists(input_path):
        print("Error: File does not exist.")
        return

    if not output_path.endswith(".csv"):
        output_path += ".csv"

    try:
        spectrum = read_spe_file(input_path)
        save_csv(spectrum, output_path)
    except Exception as e:
        print(f"Error during processing: {e}")

if __name__ == "__main__":
    main()
