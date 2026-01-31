# 🚀 Tesseract PHP Extension

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PHP Version](https://img.shields.io/badge/php-%3E%3D%208.0-blue.svg)](https://www.php.net/)
[![Tesseract OCR](https://img.shields.io/badge/OCR-Tesseract-orange.svg)](https://github.com/tesseract-ocr/tesseract)

A high-performance C++ PHP wrapper for the [libtesseract](https://github.com/tesseract-ocr/tesseract) OCR library.
Developed by **Operative IT**, this extension is an improved, feature-rich, and **PHP 8.x compatible** fork of the original work by [phplang/tesseract](https://github.com/phplang/tesseract).

---

## ✨ Features

- 🏎️ **High Performance**: Native C++ implementation for maximum speed.
- 🐘 **PHP 8 Ready**: Fully compatible with the latest PHP versions.
- 🔧 **Granular Control**: Fine-tune OCR Engine Modes (OEM) and Page Segmentation Modes (PSM).
- 🎯 **Region Processing**: Extract text from specific coordinates within an image.
- 📦 **Multi-Platform**: Agnostic build system for macOS, Debian/Ubuntu, and BSD.

---

## 💻 API Overview

```php
namespace Operativeit;

class Tesseract {
    public function __construct(?string $lang = "eng", int $oem = self::OEM_DEFAULT);
    public function setImage(string $filename): self;
    public function setPageSegMode(int $mode): self;
    public function setRectangle(int $left, int $top, int $width, int $height): self;
    public function getUTF8Text(): string;
}
```

---

## 🚀 Quick Start

### 1. Basic Extraction

```php
use Operativeit\Tesseract;

$tess = new Tesseract();
echo $tess->setImage("sample.png")->getUTF8Text();
```

### 2. Multi-Language & Region Extraction

```php
use Operativeit\Tesseract;

$tess = new Tesseract('spa', Tesseract::OEM_LSTM_ONLY);
echo $tess->setImage("invoice.jpg")
          ->setRectangle(100, 100, 500, 50) // x, y, width, height
          ->getUTF8Text();
```

---

## 📦 Installation & Build

### 🐧 Debian / Ubuntu

#### 1. Install Dependencies

```bash
sudo apt update && sudo apt install -y build-essential pkg-config git php-dev libtesseract-dev libleptonica-dev tesseract-ocr-eng
```

#### 2. Build & Install

```bash
cd tesseract-1.0.0
phpize
./configure
make
sudo make install
```

#### 3. Enable Extension

```bash
# Replace [8.x] with your exact version (e.g. 8.4)
echo "extension=tesseract.so" | sudo tee /etc/php/[8.x]/mods-available/tesseract.ini
sudo phpenmod tesseract
```

---

### 🏗️ Create Debian Package (.deb)

For professional deployment, you can generate a standard package:

```bash
# Install packaging tools
sudo apt install build-essential debhelper devscripts dh-php php-all-dev pkg-config

# Build the package
dpkg-buildpackage -us -uc -b

# Install the resulting .deb
sudo dpkg -i ../php-tesseract_1.0.0-1_amd64.deb
```

---

### 🍎 macOS (Apple Silicon & Intel)

Tested on **macOS Sequoia/Tahoe** with Homebrew.

```bash
# Requirements
brew install tesseract leptonica php

# Compilation
phpize
./configure
make
codesign -s - --force modules/tesseract.so
cp modules/tesseract.so /opt/homebrew/lib/php/pecl/$(php-config --extension-dir | xargs basename)/
```

#### Enable on macOS

```bash
# Add to your PHP configuration
echo "extension=tesseract.so" > /opt/homebrew/etc/php/[8.x]/conf.d/tesseract.ini
```

---

## 📚 Reference Tables

### Page Segmentation Modes (PSM)

| Constant            | Description                                 |
| :------------------ | :------------------------------------------ |
| `PSM_OSD_ONLY`      | Orientation and script detection only       |
| `PSM_AUTO`          | Fully automatic page segmentation (Default) |
| `PSM_SINGLE_COLUMN` | Assume a single column of text              |
| `PSM_SINGLE_BLOCK`  | Assume a single uniform block of text       |
| `PSM_SINGLE_LINE`   | Treat the image as a single text line       |
| `PSM_SINGLE_WORD`   | Treat the image as a single word            |
| `PSM_SINGLE_CHAR`   | Treat the image as a single character       |

### OCR Engine Modes (OEM)

| Constant                      | Description                          |
| :---------------------------- | :----------------------------------- |
| `OEM_TESSERACT_ONLY`          | Legacy engine only                   |
| `OEM_LSTM_ONLY`               | Neural nets LSTM engine only         |
| `OEM_TESSERACT_LSTM_COMBINED` | Legacy + LSTM engines                |
| `OEM_DEFAULT`                 | Based on what is available (Default) |

---

## ✅ Verification

Confirm the extension is loaded correctly:

```bash
php -m | grep tesseract
# or
php --re tesseract
```

---

## 📄 License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

Developed with ❤️ by [Operative IT](https://github.com/operativeit).
