{ nixpkgs ? <nixpkgs>
, amigaosenvPath ? <amigaosenv>
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
    { libiff ? ((import ../libiff/release.nix { buildForAmiga = true; }).build {}).m68k-amigaos.lib
    , libilbm ? ((import ../libilbm/release.nix { buildForAmiga = true; }).build {}).m68k-amigaos.lib {}
    , libamivideo ? ((import ../libamivideo/release.nix { buildForAmiga = true; }).build {}).m68k-amigaos
    }:
  
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
