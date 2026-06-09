# HyperX Cloud Flight

A command-line tool for monitoring the status of the [HyperX Cloud Flight Wireless Headset](https://hyperx.com/products/hyperx-cloud-flight?variant=41031691403421) on Linux. It prints battery level, charging state, volume changes, mute state, and power events, one event per line.

## Why

HyperX's own [NGENUITY](https://hyperx.com/pages/ngenuity) software doesn't run on Linux. This tool covers that gap. Its line-based output drops straight into a custom status bar; see the [headset-battery script](https://github.com/AkamQadiri/dotfiles/blob/master/dotfiles/.config/i3blocks/scripts/headset-battery) and [i3blocks config](https://github.com/AkamQadiri/dotfiles/blob/master/dotfiles/.config/i3blocks/config) in my dotfiles for an example.

## Prerequisites

Install the HIDAPI development package:

- Debian/Ubuntu: `sudo apt install libhidapi-dev`
- Arch: `sudo pacman -S hidapi`

## Build and install

```bash
sudo make
```

This compiles the source, installs the `cloud_flight` executable to `/usr/local/bin`, and adds a udev rule so the headset is readable without root. To remove both:

```bash
sudo make uninstall
```

## Usage

```bash
cloud_flight -r
```

Options:

- `-r`: read the current status once and exit.
- `-h`: show usage.

Run without options to keep monitoring until the headset powers off.

## Acknowledgment

The packet format was taken from [kondinskis/hyperx-cloud-flight](https://github.com/kondinskis/hyperx-cloud-flight); I did not reverse engineer it myself.

## License

Licensed under the MIT License; see [LICENSE](LICENSE).
