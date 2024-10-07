# CopyCompareSample

A high-efficiency tool for testing the read and write accuracy of storage devices, implemented as a dynamic link library (DLL) in C++.

## Overview

The **Copy-Compare** program is designed to verify the correctness of data read/write operations on storage devices. It performs the following fundamental functions:

1. **Writing Data**: Writes test data to the storage device.
2. **Reading Data**: Reads data back from the storage device.
3. **Comparing Data**: Compares the written data with the read data to ensure accuracy.

The program offers various testing options and is optimized for high performance and multi-threaded execution.

## Key Features

- **Increased Read/Write Efficiency**: Achieves a 30% improvement in read/write efficiency by leveraging Microsoft APIs to drive command queues, outperforming traditional SCSI/ATA passthrough methods.
- **Multi-threaded Execution**: Supports simultaneous operations on up to 23 logical devices, maximizing testing throughput.
- **Dynamic Link Library**: Implemented as a DLL for easy integration into other applications and systems.
- **Customizable Testing Options**: Provides multiple parameters to tailor the testing process according to specific requirements.

## Requirements

- **Operating System**: Windows
- **Development Environment**: Microsoft Visual Studio
- **Programming Language**: C++

## Installation

1. **Clone the Repository**

   ```bash
   git clone https://github.com/everpalm/CopyCompareSample.git
   ```

2. **Open the Solution**

   - Navigate to the cloned directory.
   - Open `CopyCompareSample.sln` with Microsoft Visual Studio.

3. **Build the Solution**

   - Build the project to generate the DLL and example application.

## Usage

### Integrating the DLL

Include the DLL in your project and utilize the provided API functions to perform copy-compare operations on storage devices.

### Example Application

An example application is included to demonstrate how to use the DLL functions. Refer to the source code in the `examples` directory for implementation details.

### Running Tests

- Configure the testing parameters as needed.
- Execute the test to perform write, read, and compare operations.
- Review the results to verify the accuracy of the storage device.

## Applied Skills

- **Microsoft Visual Studio C++ Framework**
- **Dynamic Link Libraries (DLL)**
- **Multi-threaded Programming**

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Commit your changes with clear descriptions.
4. Submit a pull request to the `main` branch.

## License

This project is licensed under the [MIT License](LICENSE).

## Contact

For questions or suggestions, please open an issue on the repository or contact the maintainer:

- **Name**: Jaron Cheng
- **Email**: everpalm@yahoo.com.tw

---

Thank you for using **CopyCompareSample**! Your feedback and contributions are greatly appreciated.
