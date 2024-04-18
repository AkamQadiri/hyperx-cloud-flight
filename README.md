# HyperX Cloud Flight

## Introduction

This project implements a simple command-line tool for monitoring the status of the [HyperX Cloud Flight Wireless Headset](https://hyperx.com/products/hyperx-cloud-flight?variant=41031691403421).

### Why?

You might wonder why create another solution when HyperX already provides [HyperX NGENUITY](https://hyperx.com/pages/ngenuity)? Well, the significant drawback of NGENUITY is its lack of support for Linux. That's why I created this tool, which is not only Linux-friendly but also easy to integrate into custom status bars. You can see an example of integration in my personal system configuration ([sb-headset](https://github.com/AkamQadiri/dotfiles/blob/master/dotfiles/.local/bin/sb-headset), [dwmblock-async](https://github.com/AkamQadiri/dwmblocks-async/blob/main/config.h)).

## Getting Started

### Prerequisites

#### hidraw

Ensure that you have `hidraw` installed on your system.

- Debian/Ubuntu

```bash
sudo apt install libhidapi-hidraw0
```

- Arch

```bash
sudo pacman -S hidapi
```

#### udev

Create a udev rule for the headset.
This can be done by creating a new file at `/etc/udev/rules.d/99-hyperx-cloud-flight.rules` and adding the following content:

```bash
KERNEL=="hidraw*", ATTRS{idVendor}=="0951", ATTRS{idProduct}=="16c4", MODE="0666"
KERNEL=="hidraw*", ATTRS{idVendor}=="0951", ATTRS{idProduct}=="1723", MODE="0666"
```

Once created, run:

```bash
udevadm control --reload-rules && udevadm trigger
```

or replug the wireless dongle.

### Build Instructions

To build the project, use the provided `makefile`:

```bash
make
```

This will compile the source code and generate the executable in the `bin` directory.

### Usage

The `cloud_flight` tool provides a command-line interface to interact with the HyperX Cloud Flight headset. It supports the following options:

- `-r`: Read and quit.
- `-h`: Display usage information.

Example usage:

```bash
./bin/cloud_flight -r
```

## Acknowledgment

I would like to clarify that I did not reverse engineer the packets myself. The required information was obtained from this [GitHub repository](https://github.com/kondinskis/hyperx-cloud-flight).

## Help

Feel free to [report any issues](https://github.com/AkamQadiri/hyperx-cloud-flight/issues) you may encounter while using this application.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.