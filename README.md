# HyperX Cloud Flight

## Introduction

This project implements a simple command-line tool for monitoring the status of the [HyperX Cloud Flight Wireless Headset](https://hyperx.com/products/hyperx-cloud-flight?variant=41031691403421).

### Why?

You might wonder why create another solution when HyperX already provides [HyperX NGENUITY](https://hyperx.com/pages/ngenuity)? Well, the significant drawback of NGENUITY is its lack of support for Linux. That's why I created this tool, which is not only Linux-friendly but also easy to integrate into custom status bars. You can see an example of integration in my personal system configuration ([headset-battery](https://github.com/AkamQadiri/dotfiles/blob/master/dotfiles/.config/i3blocks/scripts/headset-battery), [i3blocks](https://github.com/AkamQadiri/dotfiles/blob/master/dotfiles/.config/i3blocks/config)).

## Getting Started

### Prerequisites

#### hidraw

Ensure that you have `hidraw` installed on your system.

- Debian/Ubuntu

```bash
sudo apt install libhidapi-dev
```

- Arch

```bash
sudo pacman -S hidapi
```

### Build Instructions

To build the project, use the provided `makefile`:

```bash
sudo make
```

This will compile the source code and generate the executable in the `/usr/local/bin` directory.

### Usage

The `cloud_flight` tool provides a command-line interface to interact with the HyperX Cloud Flight headset. It supports the following options:

- `-r`: Read and quit.
- `-h`: Display usage information.

Example usage:

```bash
cloud_flight -r
```

## Acknowledgment

I would like to clarify that I did not reverse engineer the packets myself. The required information was obtained from this [GitHub repository](https://github.com/kondinskis/hyperx-cloud-flight).

## Help

Feel free to [report any issues](https://github.com/AkamQadiri/hyperx-cloud-flight/issues) you may encounter while using this application.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
