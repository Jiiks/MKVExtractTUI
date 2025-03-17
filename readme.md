
[![Build and Test](https://github.com/Jiiks/MKVExtractTUI/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/Jiiks/MKVExtractTUI/actions/workflows/cmake-single-platform.yml) [![AutoRelease](https://github.com/Jiiks/MKVExtractTUI/actions/workflows/build-and-release.yml/badge.svg)](https://github.com/Jiiks/MKVExtractTUI/actions/workflows/build-and-release.yml)

# Mkv(Sub)ExtractTUI

Mkv(Sub)ExtractTUI is a terminal-based user interface for extracting subtitles from MKV containers.

Progressbar as of 0.1.1a
![gif2](https://github.com/user-attachments/assets/cbb9d6ea-2412-458d-b6b2-14e12c9c0a32)

![gif](https://github.com/user-attachments/assets/65053a8a-cfd2-4f8a-a5c0-2bacc802fe32)

## Features

- **Terminal UI:** Easy navigation and control for users who prefer working in the terminal.

- **Track Information:** View detailed information about each track, including track index, language, codec, and more.

- **Custom Naming:** Customize the output file names based on track properties.

- **Multi-Track Support:** Extract multiple subtitle tracks simultaneously.

## Usage
Download from releases as binary.
To launch Mkv(Sub)ExtractTUI, simply run the binary.

Latest version automatically with curl:
```bash
curl -s https://api.github.com/repos/Jiiks/MKVExtractTUI/releases/latest | grep "browser_download_url" | cut -d : -f 2,3 | xargs -I {} sh -c 'curl -L -O {}; chmod +x $(basename {})'
```

Custom input directory:
`mkvetui -i <path>`
otherwise current working directory is used.

`mkvetui -h` for help.

Command line switches:

`-n | --nogui` `Run guiless and extract all tracks that are tagged to auto check.`

`-a | --all` `auto check all tracks.`

`-q | --quiet` `suppress output.`

`-f | --fastupdate` `Fast update gui where applicable.`

Command line options:

`-i | --input <path>` `Specify input file or directory.`

`-p | --pattern <pattern>` `Specify custom renaming pattern. Default "fn.flags.lang.ext"`

```bash
fn = replaced with filename.
flags = replaced with flags such as sdh.
lang = replaced with language tag such as eng.
ext = replaced with extension extension such as srt.
```

`-l | --lang <tags>` `Auto check all tracks matching language tags separated with [,]. Default: "en,eng"`

Settings are located in `/home/user/.config/MkvExtractTUI`or`XDG_CONFIG_HOME/MkvExtractTUI`

## Issues
- If you have an issue with some file please post the `mkvmerge -J <file>` output to [issues](https://github.com/Jiiks/MKVExtractTUI/issues).

## TODO
- [x] Most of the command line options
- [ ] Guiless mode
- [ ] Settings store
- [x] Support for other than srt(still requires some testing)
- [ ] Confirm overwrite
- [ ] Extractor screen automatic scrolling
- [x] Extractor total progress/finished prompt
- [ ] Multi track extract with tracks 1:1.srt 2:2.srt 3:3.srt

## Dependencies

- `ncurses`: Library for creating terminal-based user interfaces.
    
- `mkvtoolnix`: Set of tools to create, alter, and inspect Matroska files.

You can install the dependencies using your package manager. For example, on Debian-based systems:

```bash
sudo apt-get install libncurses5-dev mkvtoolnix
```

## Building

Clone the repository and build the project:

```bash
git clone https://github.com/Jiiks/MkvExtractTUI.git
cd MKVExtractTUI
make build
make bar #(build and run)
```

## Contributing

Contributions are welcome! If you find a bug or have a feature request, please open an [issue](https://github.com/Jiiks/MKVExtractTUI/issues) on the GitHub repository. For code contributions, please fork the repository and submit a pull request.

Use either the `dev` or the latest version branch eg. `0.1.2`. Preferably `dev` but it might lag behind the latest version.
Also if relevant document your changes in `CHANGELOG.md`.
Generally when changes are made to the latest version branch they're immediately merged into `dev`.

## License

This project is licensed under the [MIT](https://opensource.org/license/mit) License. See the [LICENSE](https://github.com/Jiiks/MKVExtractTUI/blob/master/LICENSE) file for more details.

## Third party

[ncurses](https://invisible-island.net/ncurses/), [cJSON](https://github.com/DaveGamble/cJSON), mkvtoolnix
