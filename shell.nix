{ pkgs ? import <nixpkgs> {}}:

pkgs.mkShell {
  packages = [ 
    pkgs.cmake
    pkgs.clang-tools
    pkgs.ninja
  ];
}

