{ nixpkgs ? <nixpkgs>
, amigaosenvPath ? <amigaosenv>
, libiffJobset ? import ../libiff/release.nix { inherit nixpkgs; buildForAmiga = true; }
, libilbmJobset ? import ../libilbm/release.nix { inherit nixpkgs; buildForAmiga = true; }
, libamivideoJobset ? import ../libamivideo/release.nix { inherit nixpkgs; buildForAmiga = true; }
}:

let
  pkgs = import nixpkgs {};
  
  version = builtins.readFile ./version;
  
  amigaosenv = import amigaosenvPath {
    inherit (pkgs) stdenv uae procps;
    lndir = pkgs.xorg.lndir;
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
    };
}
