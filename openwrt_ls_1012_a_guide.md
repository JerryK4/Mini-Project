# 🧰 Hướng dẫn build và nạp firmware OpenWRT cho LS1012ARDB

## 1. ✅ Yêu cầu chuẩn bị

| Hạng mục    | Mô tả                        |
| ----------- | ---------------------------- |
| Bo mạch     | LS1012ARDB (NXP Layerscape)  |
| Host PC     | Ubuntu 20.04/22.04           |
| Cáp UART    | Kết nối serial với board     |
| Ethernet    | Kết nối LAN giữa PC và board |
| TFTP server | Cài trên PC                  |
| U-Boot      | Cài sẵn trên board           |

## 2. 🔧 Cài đặt đối với host Ubuntu (build OpenWRT)

```bash
sudo apt update
sudo apt install -y build-essential clang flex bison g++ gawk \
gettext git libncurses-dev libssl-dev python3-distutils rsync unzip zlib1g-dev \
file wget subversion swig time python3 python3-pip ccache libelf-dev \
libtool-bin cmake
```

## 3. 👁️ Tải source OpenWRT cho LS1012ARDB

```bash
git clone https://git.openwrt.org/openwrt/openwrt.git
cd openwrt
git checkout v24.10.2  # Hoặc bản mới nhất
./scripts/feeds update -a
./scripts/feeds install -a
```

## 4. 🔺 Cấu hình OpenWRT cho LS1012ARDB

```bash
make menuconfig
```

- Target System: `NXP Layerscape`
- Subtarget: `armv8_64b`
- Target Profile: `Freescale LS1012A RDB board`
- Chọn các gói mong muốn (LuCI, openssh,...)

## 5. ⚖️ Build OpenWRT

```bash
make -j$(nproc)
```

Sau khi build xong, file firmware xuất hiện tại:

```bash
bin/targets/layerscape/armv8_64b/openwrt-layerscape-armv8_64b-fsl_ls1012a-rdb-squashfs-firmware.bin
```

## 6. 🔌 Chuẩn bị TFTP server

```bash
sudo apt install tftpd-hpa
sudo mkdir -p /var/lib/tftpboot
sudo cp openwrt-layerscape-*.bin /var/lib/tftpboot/
sudo chmod -R 777 /var/lib/tftpboot
sudo systemctl restart tftpd-hpa
```

## 7. 🛋️ Kết nối và cài đặt IP cho host và board

Để cấu hình bo mạch sử dụng TFTP, thực hiện các bước sau:

1. Kết nối cổng **ETH\_1** trên board với cùng mạng với máy chủ TFTP bằng cáp Ethernet (hoặc dùng ETH\_2 nếu muốn).
2. Dùng TeraTerm hoặc Minicom để mở kết nối UART.
3. Khởi động board và dừng tại giao diện U-Boot bằng cách nhấn Enter.
4. Cấu hình địa chỉ MAC (lấy từ nhãn dán trên board):

```bash
setenv ethaddr xx:xx:xx:xx:xx:xx     # cho ETH_1
setenv eth1addr xx:xx:xx:xx:xx:xx    # cho ETH_2 nếu cần
```

5. Cấu hình địa chỉ IP cho máy chủ TFTP (host Ubuntu):

```bash
setenv serverip 'IP máy chủ'
```

6. Cấu hình IP cho board (nên cùng lớp mạng với host):

```bash
setenv ipaddr 'IP board'
```

7. Chọn cổng Ethernet đang sử dụng để truyền dữ liệu:

```bash
setenv ethact pfe_eth0    # hoặc pfe_ethx nếu dùng ETH_x - x là số
saveenv
```

8. Kiểm tra kết nối mạng từ board đến host:

```bash
ping $serverip
```

## 8. 🚀 Nạp firmware OpenWRT vào NOR flash qua TFTP

**Tải firmware vào RAM:**

```bash
tftpboot 0xa0000000 openwrt-layerscape-armv8_64b-fsl_ls1012a-rdb-squashfs-firmware.bin
```

**Ghi NOR flash:**

```bash
sf probe 0:0
sf erase 0 +$filesize
sf write 0xa0000000 0 $filesize
```

**Khởi động lại:**

```bash
reset
```

## 9. 📅 Kết quả mong đợi

- Vào shell OpenWRT (bấm Enter)
- Sử dụng lệnh `ip a` để xem IP
- Đảm bảo firmware đã boot thành công


📅 **Tài liệu tham khảo:**

- [https://www.nxp.com/design/design-center/development-boards-and-designs/LS1012A-RDB](https://www.nxp.com/design/design-center/development-boards-and-designs/LS1012A-RDB)
- [https://git.openwrt.org/openwrt/openwrt.git](https://git.openwrt.org/openwrt/openwrt.git)
- [BMC Embedded Linux Team - BMC Technology and Services](https://bmctech.vn/)
---

🚀 Đã sẵn sàng build và nạp.

