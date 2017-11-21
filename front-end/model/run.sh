#!/bin/bash
#~/Projects/qklee/qemu-2.1.2/x86_64-softmmu/qemu-system-x86_64 -k us -enable-kvm -m 2048 -net nic,model=e1000 -net user -redir tcp:2022::22 -nographic ~/images/ubuntu-12.04.2-desktop-amd64.img
#../qemu-2.1.2/x86_64-softmmu/qemu-system-x86_64 -k us -enable-kvm -m 2048 -net nic,model=e1000 -net user -redir tcp:2022::22 -usbdevice serial::stdio -serial stdio ~/qfm/ubuntu-12.04.2-desktop-amd64.img
rm ../trace_parser/pci_dma_data
#../qemu-2.1.2/x86_64-softmmu/qemu-system-x86_64 -k us -m 2048 -net nic,model=e1000 -net user -redir tcp:2022::22 -usbdevice serial::stdio -serial stdio ~/qfm/ubuntu-12.04.2-desktop-amd64.img
#../qemu-2.1.2/x86_64-softmmu/qemu-system-x86_64 -k us -m 2048 -net nic,model=e1000 -net user -redir tcp:2022::22 -usbdevice serial::stdio -serial stdio ../img/crete.img

/home/jialuwang/crete/crete-dev/front-end/qemu-2.3/x86_64-softmmu/crete-qemu-2.3-system-x86_64 -k us -m 2048 -net nic,model=e1000  -serial stdio ~/qklee/img/crete.img


