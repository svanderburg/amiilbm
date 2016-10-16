{ nixpkgs ? <nixpkgs>
, amigaosenvPath ? <amigaosenv>
, kickstartROMFile ? null
, baseDiskImage ? null
, useUAE ? true
, libiffJobset ? import ../libiff/release.nix { inherit nixpkgs kickstartROMFile baseDiskImage useUAE; buildForAmiga = true; }
, libilbmJobset ? import ../libilbm/release.nix { inherit nixpkgs kickstartROMFile baseDiskImage useUAE; buildForAmiga = true; }
, libamivideoJobset ? import ../libamivideo/release.nix { inherit nixpkgs kickstartROMFile baseDiskImage useUAE; buildForAmiga = true; }
}:

let
  pkgs = import nixpkgs {};
  
  version = builtins.readFile ./version;
  
  amigaosenv = import amigaosenvPath {
    inherit (pkgs) stdenv fetchurl lhasa uae fsuae procps bchunk cdrtools;
    inherit (pkgs.xorg) lndir;
  };
in
{
  build =
    let
      libiff = libiffJobset.build.m68k-amigaos.lib;
      libilbm = libilbmJobset.build.m68k-amigaos.lib;
      libamivideo = libamivideoJobset.build.m68k-amigaos;
    in
    amigaosenv.mkDerivation {
      name = "amiilbm";
      src = ./.;
      buildInputs = [ libiff libilbm libamivideo ];
      buildCommand = ''
        cd amiilbm
        make PREFIX=/OUT
        make PREFIX=/OUT install
      '';
      inherit kickstartROMFile baseDiskImage useUAE;
    };
}
