#!/bin/sh

if [ -d /media/PIA_boot ]; then
  echo "COPYING..."
  cp obj/kernel-at91.img /media/PIA_boot/image.bin
  umount /media/PIA_*
else
  echo "SD not inserted"
fi
