amiilbm
=======
`amiilbm` is a package containing an ILBM viewer for AmigaOS, using my own ILBM
parser library instead of the native AmigaOS `iffparser.library`.

Features
========
`ilbmviewer` offers various interesting features:

* Supports the ECS chipset as well as the more advanced AGA chipset
* Knows how to deal with IFF scrap files; IFF files containing multiple images, or multiple FORM chunks
* Supports Extra Half-Brite (EHB), HAM, hires and interlace screen properties
* Supports non-interleaved ACBM images
* Displaying Deluxe Paint PC images, by converting chunky graphics into planar graphics data
* Color cycling (both Deluxe Paint and Graphicraft color range chunks are supported)

Prerequisites
=============

In order to build and use this package, the following libraries are required:

* [Geek Gadgets](http://geekgadgets.back2roots.org), containing GCC, GNU Make and various other required build utilities
* `libiff`, in order to parse IFF files
* `libilbm`, in order to understand ILBM application chunks, byte-run compression/decompression and to conveniently access ILBM image properties
* `libamivideo`, in order to convert chunky graphics data into planar graphics data

Furthermore, you also need Kickstart / Workbench 3.0 or higher

Installation
============
Compilation and installation of this package can be done using various tools
from the Geek Gadgets suite. The procedure is straight forward:

```bash
$ make
$ make install
```

License
=======
This package is available under the MIT license
