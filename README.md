# 🧰 Hướng Dẫn Build Yocto Cho i.MX8M Mini EVK Tích Hợp OpenSC

Tài liệu này hướng dẫn chi tiết cách xây dựng hệ điều hành nhúng **Yocto Linux** cho bo mạch **NXP i.MX8M Mini EVK**, sử dụng BSP chính thức từ NXP (`imx-manifest`) và tích hợp [**OpenSC**](https://github.com/bmctechvn/OpenSC) – bộ thư viện hỗ trợ truy cập thẻ thông minh và giao diện PKCS#11.

---

## 📋 Mục Lục

- [📌 Yêu Cầu Hệ Thống](#-yêu-cầu-hệ-thống)  
- [📁 1. Khởi Tạo Dự Án Yocto Với NXP BSP](#1-khởi-tạo-dự-án-yocto-với-nxp-bsp)  
- [🔧 2. Cấu Hình Môi Trường Build](#-2-cấu-hình-môi-trường-build)  
- [🧱 3. Thêm Layer OpenSC Tuỳ Chỉnh](#-3-thêm-layer-opensc-tuỳ-chỉnh)  
- [⚙️ 4. Cấu Hình File Trong Thư Mục conf Nằm Trong bld-xwayland](#️-4-cấu-hình-file-localconf)  
- [🔨 5. Thực Hiện Build Yocto](#-5-thực-hiện-build-yocto)  
- [💾 6. Ghi Image Vào Thẻ SD Hoặc eMMC](#-6-ghi-image-vào-thẻ-sd-hoặc-emmc)  
- [🧪 7. Kiểm Tra OpenSC Trên Bo Mạch](#-7-kiểm-tra-opensc-trên-bo-mạch)  
- [🧑‍💻 Người Đóng Góp](#-người-đóng-góp)

---

## 📌 Yêu Cầu Hệ Thống

### Phần mềm cần cài đặt:

```bash
sudo apt update
sudo apt install gawk wget git-core diffstat unzip texinfo gcc \
     build-essential chrpath socat cpio python3 python3-pip python3-pexpect \
     xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa \
     libsdl1.2-dev pylint3 xterm curl
```

### Hệ thống khuyến nghị:
- **Dung lượng ổ cứng**: Tối thiểu 100GB  
- **Hệ điều hành**: Ubuntu 20.04 hoặc 22.04 (tránh 24.04 nếu gặp lỗi liên quan đến namespace)

---

## 📁 1. Khởi Tạo Dự Án Yocto Với NXP BSP

```bash
mkdir <release>
cd <release>
repo init -u https://github.com/nxp-imx/imx-manifest -b <branch name> [ -m <release manifest>]
repo sync
```

>Truy cập https://github.com/nxp-imx/imx-manifest để có bản mới nhất và hướng dẫn cụ thể

---

## 🔧 2. Cấu Hình Môi Trường Build

```bash
DISTRO=fsl-imx-xwayland MACHINE=imx8mmevk source imx-setup-release.sh -b bld-xwayland
```

Lệnh trên sẽ tạo thư mục build có tên `bld-xwayland`.

---

## 🧱 3. Thêm Layer OpenSC Tuỳ Chỉnh

```bash
cd ../sources
mkdir -p /meta-bmctechvn/recipes-security/opensc
```
Tạo file opensc_git.bb để tạo recipe

```bitbake
# Mô tả gói
SUMMARY = "OpenSC tools and libraries for smart cards"  # Mô tả ngắn gọn: bộ công cụ và thư viện cho thẻ thông minh
DESCRIPTION = "Tools and libraries to access smart cards using PKCS#15 and PKCS#11 standards."  # Mô tả đầy đủ hơn về chức năng
HOMEPAGE = "https://github.com/bmctechvn/OpenSC"  # Trang chủ của dự án
LICENSE = "LGPL-2.1-or-later"  # Giấy phép sử dụng mã nguồn mở
LIC_FILES_CHKSUM = "file://COPYING;md5=cb8aedd3bced19bd8026d96a8b6876d7"  # Kiểm tra hợp lệ file license

# Lấy mã nguồn từ GitHub nhánh chính
SRC_URI = "git://github.com/bmctechvn/OpenSC.git;branch=main;protocol=https"
SRCREV = "${AUTOREV}"  # Luôn lấy commit mới nhất từ nhánh main
S = "${WORKDIR}/git"  # Thư mục chứa mã nguồn sau khi fetch

# Các gói phụ thuộc để biên dịch (build-time dependencies)
DEPENDS = "pcsc-lite openssl glib-2.0 zlib"
DEPENDS += "gnu-config-native autoconf-native automake-native libtool-native"  # Các công cụ autotools cần thiết cho build

# Kế thừa class autotools và pkgconfig để tự động configure, build, install
inherit autotools pkgconfig

# Tuỳ chọn cấu hình khi chạy ./configure
EXTRA_OECONF = " \
    --disable-strict \         # Tắt kiểm tra nghiêm ngặt (strict checks)
    --disable-assert \         # Tắt các câu lệnh assert
    --disable-notify \         # Tắt hỗ trợ thông báo (không cần thiết trên embedded)
    --enable-pcsc \            # Bật hỗ trợ PC/SC để giao tiếp với smartcard reader
    --disable-doc \            # Không build tài liệu để giảm thời gian build
"

# Loại bỏ -Werror để tránh việc warning trở thành lỗi build
CFLAGS:remove = "-Werror"
CXXFLAGS:remove = "-Werror"

# Các gói cần thiết khi chạy runtime (chạy OpenSC trên thiết bị)
RDEPENDS:${PN} += "glib-2.0 openssl zlib pcsc-lite"

# Chỉ định các file nhị phân và thư viện sẽ được đóng gói vào gói chính (${PN})
FILES:${PN} += " \
    ${libdir}/*.so.* \                         # Thư viện động (.so) với số phiên bản
    ${libdir}/pkcs11/*.so.* \                 # Các module pkcs11 trong thư mục con
    ${libdir}/opensc-pkcs11.so \              # Module pkcs11 chính
    ${libdir}/onepin-opensc-pkcs11.so \       # Module một PIN
    ${libdir}/pkcs11-spy.so \                 # Module spy để debug
    ${datadir}/bash-completion \              # Hỗ trợ bash completion cho terminal
"

# Gói phát triển (${PN}-dev) chỉ chứa các file dùng cho lập trình:
# - Thư viện .so không version (symlink)
# - File header (.h)
# - File pkgconfig (.pc)
FILES:${PN}-dev = " \
    ${libdir}/*.so \
    ${libdir}/pkcs11/*.so \
    ${libdir}/pkgconfig/*.pc \
    ${includedir} \
"

# Tắt các cảnh báo QA nếu upstream cung cấp file .so là ELF thực (không chỉ là symlink)
INSANE_SKIP:${PN}-dev += "dev-elf"
INSANE_SKIP:${PN} += "dev-so dev-deps"

# Cho phép mở rộng lớp để build cho native (chạy trên máy host)
BBCLASSEXTEND = "native"

# Patch nhỏ để sửa lỗi khi build liên quan đến khai báo hàm getopt
do_configure:prepend() {
    sed -i 's/int argc, char \* argv\[\]/int argc, char * const argv[]/' ${S}/src/getopt.h
}
```
Trong thư mục meta-bmctechvn tạo thư mục conf chứa file layer.conf
```conf
# meta-bmctechvn layer.conf

# Thêm thư mục này vào BBPATH
BBPATH .= ":${LAYERDIR}"

# Định nghĩa các tập tin .bb và .bbappend trong layer này
BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
             ${LAYERDIR}/recipes-*/*/*.bbappend"

# Phiên bản layer bạn có thể đặt tùy ý
LAYERVERSION = "1"
LAYERVERSION_meta-bmctechvn = "1"

# Danh sách các bản Yocto (bblayerseries) tương thích với layer này
LAYERSERIES_COMPAT_meta-bmctechvn = "dunfell gatesgarth warrior kirkstone hardknott walnascar"

# Ưu tiên layer (priority), layer nào lớn hơn sẽ được ưu tiên hơn
BBFILE_PRIORITY_meta-bmctechvn = "9"

# Collection name của layer này
BBFILE_COLLECTIONS += "meta-bmctechvn"
BBFILE_PATTERN_meta-bmctechvn := "^${LAYERDIR}/"

# Các layer phụ thuộc (nếu có)
LAYERSERIES_DEPENDS_meta-bmctechvn = "meta-poky meta-openembedded"

# Kích hoạt các .bbappend từ các layer khác nếu có
BBFILES_DYNAMIC += " \
    clang-layer:${LAYERDIR}/dynamic-layers/clang-layer/recipes-*/*/*.bb \
    clang-layer:${LAYERDIR}/dynamic-layers/clang-layer/recipes-*/*/*.bbappend \
    meta-python:${LAYERDIR}/dynamic-layers/meta-python/recipes-*/*/*.bb \
    meta-python:${LAYERDIR}/dynamic-layers/meta-python/recipes-*/*/*.bbappend \
    multimedia-layer:${LAYERDIR}/dynamic-layers/multimedia-layer/recipes-*/*/*.bb \
    multimedia-layer:${LAYERDIR}/dynamic-layers/multimedia-layer/recipes-*/*/*.bbappend \
    networking-layer:${LAYERDIR}/dynamic-layers/networking-layer/recipes-*/*/*.bb \
    networking-layer:${LAYERDIR}/dynamic-layers/networking-layer/recipes-*/*/*.bbappend \
    gnome-layer:${LAYERDIR}/dynamic-layers/gnome-layer/recipes-*/*/*.bb \
    gnome-layer:${LAYERDIR}/dynamic-layers/gnome-layer/recipes-*/*/*.bbappend \
    perl-layer:${LAYERDIR}/dynamic-layers/perl-layer/recipes-*/*/*.bb \
    perl-layer:${LAYERDIR}/dynamic-layers/perl-layer/recipes-*/*/*.bbappend \
    selinux:${LAYERDIR}/dynamic-layers/selinux/recipes-*/*/*.bb \
    selinux:${LAYERDIR}/dynamic-layers/selinux/recipes-*/*/*.bbappend \
"

# Thêm thư mục chứa giấy phép (nếu có)
LICENSE_PATH += "${LAYERDIR}/licenses"

# Định nghĩa các nhà cung cấp ưu tiên (nếu có)
PREFERRED_RPROVIDER_libdevmapper = "lvm2"
PREFERRED_RPROVIDER_libdevmapper-native = "lvm2-native"
PREFERRED_RPROVIDER_nativesdk-libdevmapper = "nativesdk-lvm2"

# Loại bỏ các gói không cần thiết khỏi ảnh (nếu có)
SIGGEN_EXCLUDERECIPES_ABISAFE += " \
    fbset-modes \
    gpsd-machine-conf \
    distro-feed-configs \
    ca-certificates \
    pointercal \
"

SIGGEN_EXCLUDE_SAFE_RECIPE_DEPS += " \
    android-tools->android-tools-conf \
    usb-modeswitch-data->usb-modeswitch \
    lmsensors->lmsensors-config \
    phoronix-test-suite->bash \
    phoronix-test-suite->python3 \
    phoronix-test-suite->php \
    phoronix-test-suite->lsb-release \
    phoronix-test-suite->util-linux \
    phoronix-test-suite->busybox \
    phoronix-test-suite->shared-mime-info \
    phoronix-test-suite->desktop-file-utils \
"


```

---

## ⚙️ 4. Cấu Hình File trong thư mục conf nằm trong bld-xwayland

Mở file `conf/local.conf` và thêm dòng sau vào cuối file để tiến hành cài thư viện:

```conf
CORE_IMAGE_EXTRA_INSTALL += "opensc opensc-dev"
```

> Bạn cũng có thể thêm `usbutils`, `pcsc-lite`, `gdb`, v.v. nếu cần.

Sau đó chỉnh sửa file `bblayers.conf` trong /source/conf:

```bash
cd ../bld-xwayland
echo "BBLAYERS += "${BSPDIR}/sources/meta-bmctechvn" >> conf/bblayers.conf
```
---

## 🔨 5. Thực Hiện Build Yocto

### Image tối giản:

```bash
bitbake imx-image-core
```

### Hoặc image có giao diện đồ họa và đa phương tiện:

```bash
bitbake imx-image-multimedia
```

### Hoặc image có đa phương tiện và machine learning và Qt

```bash
bitbake imx-image-full
```

> Lưu ý: quá trình build đầu tiên có thể mất **vài giờ**.

---

## 💾 6. Ghi Image Vào Thẻ SD Hoặc eMMC

Sau khi build thành công, image sẽ nằm tại:

```bash
tmp/deploy/images/imx8mmevk/
```

### Ghi image vào thẻ SD:

```bash
unzstd imx-image-multimedia-imx8mmevk.rootfs-Z.wic.zst
sudo dd if=imx-image-multimedia-imx8mmevk.rootfs-Z.wic of=/dev/sdX bs=1M conv=fsync status=progress
sync
sudo eject /dev/sdX
```
> Thay `imx-image-multimedia-imx8mmevk.rootfs-Z`bằng tên bản được build mới nhất.
> Thay `/dev/sdX` bằng đúng thiết bị thẻ SD (sử dụng lsblk khi cắm thẻ nhớ).

---

## 🧪 7. Kiểm Tra OpenSC Trên Bo Mạch

Sau khi boot:

```bash
opensc-tool --version
```

Kỳ vọng:

```
OpenSC 0.23.0
```

Kiểm tra thư viện PKCS#11:

```bash
ls /usr/lib/opensc-pkcs11.so
ls /usr/lib/pkcs11/opensc-pkcs11.so
```

Thử truy cập token (nếu có smartcard/USB token):

```bash
pkcs11-tool --module /usr/lib/opensc-pkcs11.so -L
```

---

## 🧑‍💻 Người Đóng Góp

- 🔧 Layer OpenSC: [bmctechvn/OpenSC](https://github.com/bmctechvn/OpenSC)  
- 📦 BSP Yocto chính thức: [nxp-imx/imx-manifest](https://github.com/nxp-imx/imx-manifest)  
- ✍️ Tài liệu: BMC Embedded Linux Team - BMC Technology and Services
