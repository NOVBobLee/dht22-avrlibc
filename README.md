# dht22-avrlibc

A DHT22 library working on AVR MCUs, which is able to run multiple DHT22 devices
in the same time.

## What is DHT22

DHT22 (or AM2302) is a low-cost digital temperature and humidity sensor to
measure the temperature and relative humidity in the air.

## Requirement

1. Platform: AVR MCUs
2. Library: [avr-libc](https://www.nongnu.org/avr-libc/)

## How to use

1. Include the `dht22.h`
2. Use `dht22_init()` to specify the port, pin ,ddr the DHT22 device using
3. Use `dht22_getdata()` to get the temperature and relative humidity from DHT22

## License

`dht22-avrlibc` is released under the MIT license. Use of this source code is
governed by a MIT-style license that can be found in the LICENSE file.

