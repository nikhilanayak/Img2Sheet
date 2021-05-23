# Img2Sheet, A WASM-Based App To Convert Any Image To A Spreadsheet

## Usage
Img2Sheet is already running at [img2sheet.herokuapp.com](img2sheet.herokuapp.com). If you want to compile it yourself, follow the Instructions Below.

## Compiling
The following steps have only been tested on Linux but the compiled .wasm file is cross-platform

1. Install emsdk

    To install emsdk, follow [these](https://emscripten.org/docs/getting_started/downloads.html) steps and run emsdk_env so `em++` is available in the current path

2. Clone this repository
    ```
    git clone https://github.com/IndianBoy6/Img2Sheet.git
    ```
3. Build
    ```bash
    make build
    ```
4. Run (requires python3 to run web server. For other webservers, just serve the `public` directory)
    ```bash
    make run
    ```

## Contributing
Right now, there's an annoying bug where c++ `sigsegv`s when uploading a second image after the first one is done. If you want to contribute, trying to solve that would be great.