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
make
sudo make install
```

This builds `cloud_flight` into `bin/`, installs it to `/usr/local/bin`, and adds a udev rule so the headset is readable without root. `PREFIX` and `DESTDIR` are honored for packaging. To remove the executable and the rule:

```bash
sudo make uninstall
```

## Usage

```bash
cloud_flight -r
```

Options:

- `-r`: print the current battery status once and exit. Exits non-zero if the headset is off or no battery report arrives within a second, so scripts can tell a missing reading from a real one.
- `-h`: show usage.

Run without options to keep monitoring until the headset powers off. The tool exits non-zero if the connection to the headset is lost.

## Acknowledgment

The packet format was taken from [kondinskis/hyperx-cloud-flight](https://github.com/kondinskis/hyperx-cloud-flight); I did not reverse engineer it myself.

## License

Licensed under the MIT License; see [LICENSE](LICENSE).
