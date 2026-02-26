# QCNFA765 / WCN6855 Support

The Qualcomm QCNFA765 (chip: WCN6855 hw2.1, PCI ID 17cb:1103) uses the Linux
`ath11k_pci` driver and requires a one-time MSI configuration step before it
will initialize correctly under bhyve PCI passthrough.

## Background

`ath11k_pci` reads its interrupt MSI address and data from the PCI
configuration space.  Under bhyve, the configuration space reflects
guest-virtual values; if the driver programs the WiFi firmware with those
values the device hangs waiting for interrupts that the host cannot deliver.

The `linux-lts` kernel in this package adds two module parameters to
`ath11k_pci`:

    ath11k_pci.host_msi_vector_addr  (ulong)
    ath11k_pci.host_msi_vector_data  (uint)

When set, the driver uses these host-physical values instead of reading from
the configuration space.  The `wifibox-ath11k-setup` script detects the
correct values and injects them into `grub.cfg` automatically.

## Prerequisites

- `wifibox-alpine` installed with `FW_ATH11K` (the default)
- `/usr/local/etc/wifibox/bhyve.conf` has `passthru=` set to the QCNFA765's
  PCI bus/slot/function, e.g.:

      passthru=5/0/0

  Find the slot with: `pciconf -l | grep 17cb`

## One-time setup

Run as root (wifibox must not be running):

    # /usr/local/libexec/wifibox/wifibox-ath11k-setup

The script will:

1. Start the wifibox VM
2. Wait for `ath11k_pci` to program MSI (up to 30 seconds)
3. Read the host-physical MSI address and data from PCI config space
4. Stop the VM, patch `/usr/local/share/wifibox/grub.cfg`, and restart wifibox
5. Confirm the wifibox network interface appears

If successful, wifibox will continue to work across reboots without re-running
the script.

## After suspend/resume

The host may reassign the MSI vector after suspend/resume.  If wifibox fails
to start after a suspend cycle, re-run with `--force`:

    # /usr/local/libexec/wifibox/wifibox-ath11k-setup --force

`--force` removes the existing MSI parameters from `grub.cfg` and re-detects
them from scratch.

## Upgrading wifibox-alpine

`grub.cfg` is installed as a `@sample` file.  On upgrade, pkg will not
overwrite your existing `grub.cfg`, so the MSI parameters are preserved and
the setup script does not need to be re-run.

## Troubleshooting

- **"Timed out waiting for MSI Enable"** — verify `passthru=` in `bhyve.conf`
  matches the QCNFA765 slot, confirm `FW_ATH11K` firmware is present, and
  check that `linux64` kmod is loaded (`kldstat -n linux64`).
- **MSI address is zero** — bhyve may not have written config space yet; try
  increasing the polling timeout by setting `WAIT_MSI` in the environment.
- **wifibox interface does not appear** — check `/var/log/wifibox.log` and the
  output of `service wifibox status`.

## References

- https://github.com/pgj/freebsd-wifibox/issues/137
- https://github.com/pgj/freebsd-wifibox-alpine/blob/main/aports/linux-lts/0002-ath11k_pci-msi-parameters.patch
