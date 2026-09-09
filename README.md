# format_json

Format JSON splitting over lines, or maintain lines and only normalise leading whitespace with the `--leading-ws` switch.

## Building

A C++20 compatible compiler is required.

```shell
git clone https://github.com/BenHanson/lexertl17
git clone https://github.com/BenHanson/parsertl17
git clone https://github.com/BenHanson/format_json
```

* Use the `Makefile` when building on Linux
* Use the `.sln` file when building with Visual Studio

## Usage

`format_json <pathname> [--help] [--indent <val>] [--leading-ws]`

 ## Switches

 `--help`

Display help

 `--indent <num>`

Indent by `num` of spaces (defaults to 2)

`--leading-ws`

Do not split lines as part of formatting
